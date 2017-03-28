/*
 * Copyright (c) 2013  Luis R. Rodriguez <mcgrof@do-not-panic.com>
 *
 * Backport ww mutex for older kernels. This is not supported when
 * DEBUG_MUTEXES or DEBUG_LOCK_ALLOC is enabled.
 *
 * Taken from: kernel/mutex.c - via linux-stable v3.11-rc2
 *
 * Mutexes: blocking mutual exclusion locks
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2004, 2005, 2006 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * Many thanks to Arjan van de Ven, Thomas Gleixner, Steven Rostedt and
 * David Howells for suggestions and improvements.
 *
 *  - Adaptive spinning for mutexes by Peter Zijlstra. (Ported to mainline
 *    from the -rt tree, where it was originally implemented for rtmutexes
 *    by Steven Rostedt, based on work by Gregory Haskins, Peter Morreale
 *    and Sven Dietrich.
 *
 * Also see Documentation/mutex-design.txt.
 */

#include <linux/mutex.h>
#include <linux/ww_mutex.h>
#include <asm/mutex.h>
#include <linux/sched.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0)
#include <linux/sched/rt.h>
#endif
#include <linux/export.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/debug_locks.h>
#include <linux/version.h>

/*
 * A negative mutex count indicates that waiters are sleeping waiting for the
 * mutex.
 */
#define	MUTEX_SHOW_NO_WAITER(mutex)	(atomic_read(&(mutex)->count) >= 0)

#define spin_lock_mutex(lock, flags) \
	do { spin_lock(lock); (void)(flags); } while (0)
#define spin_unlock_mutex(lock, flags) \
	do { spin_unlock(lock); (void)(flags); } while (0)
#define mutex_remove_waiter(lock, waiter, ti) \
	__list_del((waiter)->list.prev, (waiter)->list.next)

#ifdef CONFIG_SMP
static inline void mutex_set_owner(struct mutex *lock)
{
	lock->owner = current;
}

static inline void mutex_clear_owner(struct mutex *lock)
{
	lock->owner = NULL;
}
#else
static inline void mutex_set_owner(struct mutex *lock)
{
}

static inline void mutex_clear_owner(struct mutex *lock)
{
}
#endif


#ifdef CONFIG_MUTEX_SPIN_ON_OWNER
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0) /* 2bd2c92c and 41fcb9f2 */
/*
 * In order to avoid a stampede of mutex spinners from acquiring the mutex
 * more or less simultaneously, the spinners need to acquire a MCS lock
 * first before spinning on the owner field.
 *
 * We don't inline mspin_lock() so that perf can correctly account for the
 * time spent in this lock function.
 */
struct mspin_node {
	struct mspin_node *next ;
	int		  locked;	/* 1 if lock acquired */
};
#define	MLOCK(mutex)	((struct mspin_node **)&((mutex)->spin_mlock))

static noinline
void mspin_lock(struct mspin_node **lock, struct mspin_node *node)
{
	struct mspin_node *prev;

	/* Init node */
	node->locked = 0;
	node->next   = NULL;

	prev = xchg(lock, node);
	if (likely(prev == NULL)) {
		/* Lock acquired */
		node->locked = 1;
		return;
	}
	ACCESS_ONCE(prev->next) = node;
	smp_wmb();
	/* Wait until the lock holder passes the lock down */
	while (!ACCESS_ONCE(node->locked))
		arch_mutex_cpu_relax();
}

static void mspin_unlock(struct mspin_node **lock, struct mspin_node *node)
{
	struct mspin_node *next = ACCESS_ONCE(node->next);

	if (likely(!next)) {
		/*
		 * Release the lock by setting it to NULL
		 */
		if (cmpxchg(lock, node, NULL) == node)
			return;
		/* Wait until the next pointer is set */
		while (!(next = ACCESS_ONCE(node->next)))
			arch_mutex_cpu_relax();
	}
	ACCESS_ONCE(next->locked) = 1;
	smp_wmb();
}

/*
 * Mutex spinning code migrated from kernel/sched/core.c
 */

static inline bool owner_running(struct mutex *lock, struct task_struct *owner)
{
	if (lock->owner != owner)
		return false;

	/*
	 * Ensure we emit the owner->on_cpu, dereference _after_ checking
	 * lock->owner still matches owner, if that fails, owner might
	 * point to free()d memory, if it still matches, the rcu_read_lock()
	 * ensures the memory stays valid.
	 */
	barrier();

	return owner->on_cpu;
}

/*
 * Look out! "owner" is an entirely speculative pointer
 * access and not reliable.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
static noinline
#endif
int mutex_spin_on_owner(struct mutex *lock, struct task_struct *owner)
{
	rcu_read_lock();
	while (owner_running(lock, owner)) {
		if (need_resched())
			break;

		arch_mutex_cpu_relax();
	}
	rcu_read_unlock();

	/*
	 * We break out the loop above on need_resched() and when the
	 * owner changed, which is a sign for heavy contention. Return
	 * success only when lock->owner is NULL.
	 */
	return lock->owner == NULL;
}

/*
 * Initial check for entering the mutex spinning loop
 */
static inline int mutex_can_spin_on_owner(struct mutex *lock)
{
	int retval = 1;

	rcu_read_lock();
	if (lock->owner)
		retval = lock->owner->on_cpu;
	rcu_read_unlock();
	/*
	 * if lock->owner is not set, the mutex owner may have just acquired
	 * it and not set the owner yet or the mutex has been released.
	 */
	return retval;
}
#else /* Backport 2bd2c92c: help keep backport_mutex_lock_common() clean */

struct mspin_node {
};
#define	MLOCK(mutex) NULL

static noinline
void mspin_lock(struct mspin_node **lock, struct mspin_node *node)
{
}

static void mspin_unlock(struct mspin_node **lock, struct mspin_node *node)
{
}

static inline bool owner_running(struct mutex *lock, struct task_struct *owner)
{
}

int mutex_spin_on_owner(struct mutex *lock, struct task_struct *owner)
{
}

static inline int mutex_can_spin_on_owner(struct mutex *lock)
{
	return 1;
}
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0) */
#endif /* CONFIG_MUTEX_SPIN_ON_OWNER */

/*
 * Release the lock, slowpath:
 */
static inline void
__mutex_unlock_common_slowpath(atomic_t *lock_count, int nested)
{
	struct mutex *lock = container_of(lock_count, struct mutex, count);
	unsigned long flags;

	spin_lock_mutex(&lock->wait_lock, flags);
	mutex_release(&lock->dep_map, nested, _RET_IP_);
	/* debug_mutex_unlock(lock); */

	/*
	 * some architectures leave the lock unlocked in the fastpath failure
	 * case, others need to leave it locked. In the later case we have to
	 * unlock it here
	 */
	if (__mutex_slowpath_needs_to_unlock())
		atomic_set(&lock->count, 1);

	if (!list_empty(&lock->wait_list)) {
		/* get the first entry from the wait-list: */
		struct mutex_waiter *waiter =
				list_entry(lock->wait_list.next,
					   struct mutex_waiter, list);

		/* debug_mutex_wake_waiter(lock, waiter); */

		wake_up_process(waiter->task);
	}

	spin_unlock_mutex(&lock->wait_lock, flags);
}

/*
 * Release the lock, slowpath:
 */
static __used noinline void
__mutex_unlock_slowpath(atomic_t *lock_count)
{
	__mutex_unlock_common_slowpath(lock_count, 1);
}

/**
 * ww_mutex_unlock - release the w/w mutex
 * @lock: the mutex to be released
 *
 * Unlock a mutex that has been locked by this task previously with any of the
 * ww_mutex_lock* functions (with or without an acquire context). It is
 * forbidden to release the locks after releasing the acquire context.
 *
 * This function must not be used in interrupt context. Unlocking
 * of a unlocked mutex is not allowed.
 */
void __sched ww_mutex_unlock(struct ww_mutex *lock)
{
	/*
	 * The unlocking fastpath is the 0->1 transition from 'locked'
	 * into 'unlocked' state:
	 */
	if (lock->ctx) {
		if (lock->ctx->acquired > 0)
			lock->ctx->acquired--;
		lock->ctx = NULL;
	}

	__mutex_fastpath_unlock(&lock->base.count, __mutex_unlock_slowpath);
}
EXPORT_SYMBOL_GPL(ww_mutex_unlock);

static inline int __sched
__mutex_lock_check_stamp(struct mutex *lock, struct ww_acquire_ctx *ctx)
{
	struct ww_mutex *ww = container_of(lock, struct ww_mutex, base);
	struct ww_acquire_ctx *hold_ctx = ACCESS_ONCE(ww->ctx);

	if (!hold_ctx)
		return 0;

	if (unlikely(ctx == hold_ctx))
		return -EALREADY;

	if (ctx->stamp - hold_ctx->stamp <= LONG_MAX &&
	    (ctx->stamp != hold_ctx->stamp || ctx > hold_ctx)) {
		return -EDEADLK;
	}

	return 0;
}

static __always_inline void ww_mutex_lock_acquired(struct ww_mutex *ww,
						   struct ww_acquire_ctx *ww_ctx)
{
	ww_ctx->acquired++;
}

/*
 * after acquiring lock with fastpath or when we lost out in contested
 * slowpath, set ctx and wake up any waiters so they can recheck.
 *
 * This function is never called when CONFIG_DEBUG_LOCK_ALLOC is set,
 * as the fastpath and opportunistic spinning are disabled in that case.
 */
static __always_inline void
ww_mutex_set_context_fastpath(struct ww_mutex *lock,
			       struct ww_acquire_ctx *ctx)
{
	unsigned long flags;
	struct mutex_waiter *cur;

	ww_mutex_lock_acquired(lock, ctx);

	lock->ctx = ctx;

	/*
	 * The lock->ctx update should be visible on all cores before
	 * the atomic read is done, otherwise contended waiters might be
	 * missed. The contended waiters will either see ww_ctx == NULL
	 * and keep spinning, or it will acquire wait_lock, add itself
	 * to waiter list and sleep.
	 */
	smp_mb(); /* ^^^ */

	/*
	 * Check if lock is contended, if not there is nobody to wake up
	 */
	if (likely(atomic_read(&lock->base.count) == 0))
		return;

	/*
	 * Uh oh, we raced in fastpath, wake up everyone in this case,
	 * so they can see the new lock->ctx.
	 */
	spin_lock_mutex(&lock->base.wait_lock, flags);
	list_for_each_entry(cur, &lock->base.wait_list, list) {
		/* debug_mutex_wake_waiter(&lock->base, cur); */
		wake_up_process(cur->task);
	}
	spin_unlock_mutex(&lock->base.wait_lock, flags);
}

/**
 * backport_schedule_preempt_disabled - called with preemption disabled
 *
 * Backports c5491ea7. This is not exported so we leave it
 * here as this is the only current core user on backports.
 * Although available on >= 3.4 its only for in-kernel code so
 * we provide our own.
 *
 * Returns with preemption disabled. Note: preempt_count must be 1
 */
static void __sched backport_schedule_preempt_disabled(void)
{
	preempt_enable_no_resched();
	schedule();
	preempt_disable();
}

/*
 * Lock a mutex (possibly interruptible), slowpath:
 */
static __always_inline int __sched
__backport_mutex_lock_common(struct mutex *lock, long state,
			     unsigned int subclass,
			     struct lockdep_map *nest_lock, unsigned long ip,
			     struct ww_acquire_ctx *ww_ctx)
{
	struct task_struct *task = current;
	struct mutex_waiter waiter;
	unsigned long flags;
	int ret;

	preempt_disable();
	mutex_acquire_nest(&lock->dep_map, subclass, 0, nest_lock, ip);

#ifdef CONFIG_MUTEX_SPIN_ON_OWNER
	/*
	 * Optimistic spinning.
	 *
	 * We try to spin for acquisition when we find that there are no
	 * pending waiters and the lock owner is currently running on a
	 * (different) CPU.
	 *
	 * The rationale is that if the lock owner is running, it is likely to
	 * release the lock soon.
	 *
	 * Since this needs the lock owner, and this mutex implementation
	 * doesn't track the owner atomically in the lock field, we need to
	 * track it non-atomically.
	 *
	 * We can't do this for DEBUG_MUTEXES because that relies on wait_lock
	 * to serialize everything.
	 *
	 * The mutex spinners are queued up using MCS lock so that only one
	 * spinner can compete for the mutex. However, if mutex spinning isn't
	 * going to happen, there is no point in going through the lock/unlock
	 * overhead.
	 */
	if (!mutex_can_spin_on_owner(lock))
		goto slowpath;

	for (;;) {
		struct task_struct *owner;
		struct mspin_node  node;

		if (!__builtin_constant_p(ww_ctx == NULL) && ww_ctx->acquired > 0) {
			struct ww_mutex *ww;

			ww = container_of(lock, struct ww_mutex, base);
			/*
			 * If ww->ctx is set the contents are undefined, only
			 * by acquiring wait_lock there is a guarantee that
			 * they are not invalid when reading.
			 *
			 * As such, when deadlock detection needs to be
			 * performed the optimistic spinning cannot be done.
			 */
			if (ACCESS_ONCE(ww->ctx))
				break;
		}

		/*
		 * If there's an owner, wait for it to either
		 * release the lock or go to sleep.
		 */
		mspin_lock(MLOCK(lock), &node);
		owner = ACCESS_ONCE(lock->owner);
		if (owner && !mutex_spin_on_owner(lock, owner)) {
			mspin_unlock(MLOCK(lock), &node);
			break;
		}

		if ((atomic_read(&lock->count) == 1) &&
		    (atomic_cmpxchg(&lock->count, 1, 0) == 1)) {
			lock_acquired(&lock->dep_map, ip);
			if (!__builtin_constant_p(ww_ctx == NULL)) {
				struct ww_mutex *ww;
				ww = container_of(lock, struct ww_mutex, base);

				ww_mutex_set_context_fastpath(ww, ww_ctx);
			}

			mutex_set_owner(lock);
			mspin_unlock(MLOCK(lock), &node);
			preempt_enable();
			return 0;
		}
		mspin_unlock(MLOCK(lock), &node);

		/*
		 * When there's no owner, we might have preempted between the
		 * owner acquiring the lock and setting the owner field. If
		 * we're an RT task that will live-lock because we won't let
		 * the owner complete.
		 */
		if (!owner && (need_resched() || rt_task(task)))
			break;

		/*
		 * The cpu_relax() call is a compiler barrier which forces
		 * everything in this loop to be re-loaded. We don't need
		 * memory barriers as we'll eventually observe the right
		 * values at the cost of a few extra spins.
		 */
		arch_mutex_cpu_relax();
	}
slowpath:
#endif
	spin_lock_mutex(&lock->wait_lock, flags);

	/* We don't support DEBUG_MUTEXES on the backport */
	/* debug_mutex_lock_common(lock, &waiter); */
	/* debug_mutex_add_waiter(lock, &waiter, task_thread_info(task)); */

	/* add waiting tasks to the end of the waitqueue (FIFO): */
	list_add_tail(&waiter.list, &lock->wait_list);
	waiter.task = task;

	if (MUTEX_SHOW_NO_WAITER(lock) && (atomic_xchg(&lock->count, -1) == 1))
		goto done;

	lock_contended(&lock->dep_map, ip);

	for (;;) {
		/*
		 * Lets try to take the lock again - this is needed even if
		 * we get here for the first time (shortly after failing to
		 * acquire the lock), to make sure that we get a wakeup once
		 * it's unlocked. Later on, if we sleep, this is the
		 * operation that gives us the lock. We xchg it to -1, so
		 * that when we release the lock, we properly wake up the
		 * other waiters:
		 */
		if (MUTEX_SHOW_NO_WAITER(lock) &&
		   (atomic_xchg(&lock->count, -1) == 1))
			break;

		/*
		 * got a signal? (This code gets eliminated in the
		 * TASK_UNINTERRUPTIBLE case.)
		 */
		if (unlikely(signal_pending_state(state, task))) {
			ret = -EINTR;
			goto err;
		}

		if (!__builtin_constant_p(ww_ctx == NULL) && ww_ctx->acquired > 0) {
			ret = __mutex_lock_check_stamp(lock, ww_ctx);
			if (ret)
				goto err;
		}

		__set_task_state(task, state);

		/* didn't get the lock, go to sleep: */
		spin_unlock_mutex(&lock->wait_lock, flags);
		backport_schedule_preempt_disabled();
		spin_lock_mutex(&lock->wait_lock, flags);
	}

done:
	lock_acquired(&lock->dep_map, ip);
	/* got the lock - rejoice! */
	mutex_remove_waiter(lock, &waiter, current_thread_info());
	mutex_set_owner(lock);

	if (!__builtin_constant_p(ww_ctx == NULL)) {
		struct ww_mutex *ww = container_of(lock,
						      struct ww_mutex,
						      base);
		struct mutex_waiter *cur;

		/*
		 * This branch gets optimized out for the common case,
		 * and is only important for ww_mutex_lock.
		 */

		ww_mutex_lock_acquired(ww, ww_ctx);
		ww->ctx = ww_ctx;

		/*
		 * Give any possible sleeping processes the chance to wake up,
		 * so they can recheck if they have to back off.
		 */
		list_for_each_entry(cur, &lock->wait_list, list) {
			/* debug_mutex_wake_waiter(lock, cur); */
			wake_up_process(cur->task);
		}
	}

	/* set it to 0 if there are no waiters left: */
	if (likely(list_empty(&lock->wait_list)))
		atomic_set(&lock->count, 0);

	spin_unlock_mutex(&lock->wait_lock, flags);

	/* debug_mutex_free_waiter(&waiter); */
	preempt_enable();

	return 0;

err:
	mutex_remove_waiter(lock, &waiter, task_thread_info(task));
	spin_unlock_mutex(&lock->wait_lock, flags);
	/* debug_mutex_free_waiter(&waiter); */
	mutex_release(&lock->dep_map, 1, ip);
	preempt_enable();
	return ret;
}

static noinline int __sched
__ww_mutex_lock_slowpath(struct ww_mutex *lock, struct ww_acquire_ctx *ctx)
{
	return __backport_mutex_lock_common(&lock->base, TASK_UNINTERRUPTIBLE, 0,
					    NULL, _RET_IP_, ctx);
}

static noinline int __sched
__ww_mutex_lock_interruptible_slowpath(struct ww_mutex *lock,
					    struct ww_acquire_ctx *ctx)
{
	return __backport_mutex_lock_common(&lock->base, TASK_INTERRUPTIBLE, 0,
					    NULL, _RET_IP_, ctx);
}

/**
 * __mutex_fastpath_lock_retval - try to take the lock by moving the count
 *				  from 1 to a 0 value
 * @count: pointer of type atomic_t
 *
 * For backporting purposes we can't use the older kernel's
 * __mutex_fastpath_lock_retval() since upon failure of a fastpath
 * lock we want to call our a failure routine with more than one argument, in
 * this case the context for ww mutexes. Refer to commit a41b56ef the
 * argument increase. It'd be painful to backport all asm code for the
 * supported architectures so instead lets penalize the backport ww mutex
 * fastpath lock with the not so efficient generic atomic_dec_return()
 * implementation.
 *
 * Change the count from 1 to a value lower than 1. This function returns 0
 * if the fastpath succeeds, or -1 otherwise.
 */
static inline int
__backport_mutex_fastpath_lock_retval(atomic_t *count)
{
	if (unlikely(atomic_dec_return(count) < 0))
		return -1;
	return 0;
}

int __sched
__ww_mutex_lock(struct ww_mutex *lock, struct ww_acquire_ctx *ctx)
{
	int ret;

	might_sleep();

	ret = __backport_mutex_fastpath_lock_retval(&lock->base.count);

	if (likely(!ret)) {
		ww_mutex_set_context_fastpath(lock, ctx);
		mutex_set_owner(&lock->base);
	} else
		ret = __ww_mutex_lock_slowpath(lock, ctx);
	return ret;
}
EXPORT_SYMBOL_GPL(__ww_mutex_lock);

int __sched
__ww_mutex_lock_interruptible(struct ww_mutex *lock, struct ww_acquire_ctx *ctx)
{
	int ret;

	might_sleep();

	ret = __backport_mutex_fastpath_lock_retval(&lock->base.count);

	if (likely(!ret)) {
		ww_mutex_set_context_fastpath(lock, ctx);
		mutex_set_owner(&lock->base);
	} else
		ret = __ww_mutex_lock_interruptible_slowpath(lock, ctx);
	return ret;
}
EXPORT_SYMBOL_GPL(__ww_mutex_lock_interruptible);

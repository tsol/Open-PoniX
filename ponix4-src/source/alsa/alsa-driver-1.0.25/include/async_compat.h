#ifndef _ASYNC_COMPAT_H
#define _ASYNC_COMPAT_H

typedef u64 async_cookie_t;
typedef void (async_func_ptr) (void *data, async_cookie_t cookie);

/* just do synchronous calls */
static inline async_cookie_t
async_schedule_domain(async_func_ptr *ptr, void *data, struct list_head *list)
{
	ptr(data, 0);
	return 0;
}

static inline void async_synchronize_full(void)
{
}

static inline async_cookie_t async_schedule(async_func_ptr *ptr, void *data)
{
	return async_schedule_domain(ptr, data, NULL);
}

static inline void async_synchronize_full_domain(struct list_head *list)
{
}

static inline void async_synchronize_cookie(async_cookie_t cookie)
{
}

static inline void async_synchronize_cookie_domain(async_cookie_t cookie,
						   struct list_head *list)
{
}

#endif

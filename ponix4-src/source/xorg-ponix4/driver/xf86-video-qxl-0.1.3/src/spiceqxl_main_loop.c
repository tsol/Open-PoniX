/*
 * Copyright 2011 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/time.h>

#include <spice.h>
#include "spiceqxl_main_loop.h"

static int spiceqxl_main_loop_debug = 0;

#define DPRINTF(x, format, ...) { \
    if (x <= spiceqxl_main_loop_debug) { \
        printf("%s: " format "\n" , __FUNCTION__, ## __VA_ARGS__); \
    } \
}

/* From ring.h */
typedef struct Ring RingItem;
typedef struct Ring {
    RingItem *prev;
    RingItem *next;
} Ring;

static inline void ring_init(Ring *ring)
{
    ring->next = ring->prev = ring;
}

static inline void ring_item_init(RingItem *item)
{
    item->next = item->prev = NULL;
}

static inline int ring_item_is_linked(RingItem *item)
{
    return !!item->next;
}

static inline int ring_is_empty(Ring *ring)
{
    assert(ring->next != NULL && ring->prev != NULL);
    return ring == ring->next;
}

static inline void ring_add(Ring *ring, RingItem *item)
{
    assert(ring->next != NULL && ring->prev != NULL);
    assert(item->next == NULL && item->prev == NULL);

    item->next = ring->next;
    item->prev = ring;
    ring->next = item->next->prev = item;
}

static inline void __ring_remove(RingItem *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
    item->prev = item->next = 0;
}

static inline void ring_remove(RingItem *item)
{
    assert(item->next != NULL && item->prev != NULL);
    assert(item->next != item);

    __ring_remove(item);
}

static inline RingItem *ring_get_head(Ring *ring)
{
    RingItem *ret;

    assert(ring->next != NULL && ring->prev != NULL);

    if (ring_is_empty(ring)) {
        return NULL;
    }
    ret = ring->next;
    return ret;
}

static inline RingItem *ring_get_tail(Ring *ring)
{
    RingItem *ret;

    assert(ring->next != NULL && ring->prev != NULL);

    if (ring_is_empty(ring)) {
        return NULL;
    }
    ret = ring->prev;
    return ret;
}

static inline RingItem *ring_next(Ring *ring, RingItem *pos)
{
    RingItem *ret;

    assert(ring->next != NULL && ring->prev != NULL);
    assert(pos);
    assert(pos->next != NULL && pos->prev != NULL);
    ret = pos->next;
    return (ret == ring) ? NULL : ret;
}

static inline RingItem *ring_prev(Ring *ring, RingItem *pos)
{
    RingItem *ret;

    assert(ring->next != NULL && ring->prev != NULL);
    assert(pos);
    assert(pos->next != NULL && pos->prev != NULL);
    ret = pos->prev;
    return (ret == ring) ? NULL : ret;
}

#define RING_FOREACH_SAFE(var, next, ring)                    \
    for ((var) = ring_get_head(ring),                         \
         (next) = (var) ? ring_next(ring, (var)) : NULL;      \
            (var);                                            \
            (var) = (next),                                   \
            (next) = (var) ? ring_next(ring, (var)) : NULL)

/**/

#define NOT_IMPLEMENTED printf("%s not implemented\n", __func__);

static SpiceCoreInterface core;

typedef struct SpiceTimer {
    OsTimerPtr xorg_timer;
    SpiceTimerFunc func;
    void *opaque; // also stored in xorg_timer, but needed for timer_start
} Timer;

static CARD32 xorg_timer_callback(
    OsTimerPtr xorg_timer,
    CARD32 time,
    pointer arg)
{
    SpiceTimer *timer = (SpiceTimer*)arg;

    timer->func(timer->opaque);
    return 0; // if non zero xorg does a TimerSet, we don't want that.
}

static SpiceTimer* timer_add(SpiceTimerFunc func, void *opaque)
{
    SpiceTimer *timer = calloc(sizeof(SpiceTimer), 1);

    timer->xorg_timer = TimerSet(NULL, 0, 1e9 /* TODO: infinity? */, xorg_timer_callback, timer);
    timer->func = func;
    timer->opaque = opaque;
    return timer;
}

static void timer_start(SpiceTimer *timer, uint32_t ms)
{
    TimerSet(timer->xorg_timer, 0 /* flags */, ms, xorg_timer_callback, timer);
}

static void timer_cancel(SpiceTimer *timer)
{
    TimerCancel(timer->xorg_timer);
}

static void timer_remove(SpiceTimer *timer)
{
    TimerFree(timer->xorg_timer);
    free(timer);
}

struct SpiceWatch {
    RingItem link;
    int fd;
    int event_mask;
    SpiceWatchFunc func;
    void *opaque;
    int remove;
};

Ring watches;

int watch_count = 0;

static SpiceWatch *watch_add(int fd, int event_mask, SpiceWatchFunc func, void *opaque)
{
    SpiceWatch *watch = malloc(sizeof(SpiceWatch));

    DPRINTF(0, "adding %p, fd=%d at %d", watch,
        fd, watch_count);
    watch->fd = fd;
    watch->event_mask = event_mask;
    watch->func = func;
    watch->opaque = opaque;
    watch->remove = FALSE;
    ring_item_init(&watch->link);
    ring_add(&watches, &watch->link);
    watch_count++;
    return watch;
}

static void watch_update_mask(SpiceWatch *watch, int event_mask)
{
    DPRINTF(0, "fd %d to %d", watch->fd, event_mask);
    watch->event_mask = event_mask;
}

static void watch_remove(SpiceWatch *watch)
{
    DPRINTF(0, "remove %p (fd %d)", watch, watch->fd);
    watch->remove = TRUE;
    watch_count--;
}

static void channel_event(int event, SpiceChannelEventInfo *info)
{
    NOT_IMPLEMENTED
}

static int set_watch_fds(fd_set *rfds, fd_set *wfds)
{
    SpiceWatch *watch;
    RingItem *link;
    RingItem *next;
    int max_fd = -1;

    RING_FOREACH_SAFE(link, next, &watches) {
        watch = (SpiceWatch*)link;
        if (watch->event_mask & SPICE_WATCH_EVENT_READ) {
            FD_SET(watch->fd, rfds);
            max_fd = watch->fd > max_fd ? watch->fd : max_fd;
        }
        if (watch->event_mask & SPICE_WATCH_EVENT_WRITE) {
            FD_SET(watch->fd, wfds);
            max_fd = watch->fd > max_fd ? watch->fd : max_fd;
        }
    }
    return max_fd;
}

/*
 * called just before the X server goes into select()
 * readmask is just an fdset on linux, but something totally different on windows (etc).
 * DIX has a comment about it using a special type to hide this (so we break that here)
 */
static void xspice_block_handler(pointer data, OSTimePtr timeout, pointer readmask)
{
    /* set all our fd's */
    set_watch_fds((fd_set*)readmask, (fd_set*)readmask);
}

/*
 * xserver only calles wakeup_handler with the read fd_set, so we
 * must either patch it or do a polling select ourselves, this is the
 * later approach. Since we are already doing a polling select, we
 * already select on all (we could avoid selecting on the read since
 * that *is* actually taken care of by the wakeup handler).
 */
static void select_and_check_watches(void)
{
    fd_set rfds, wfds;
    int max_fd = -1;
    SpiceWatch *watch;
    RingItem *link;
    RingItem *next;
    struct timeval timeout;
    int retval;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    max_fd = set_watch_fds(&rfds, &wfds);
    watch = (SpiceWatch*)watches.next;
    timeout.tv_sec = timeout.tv_usec = 0;
    retval = select(max_fd + 1, &rfds, &wfds, NULL, &timeout);
    if (retval) {
        RING_FOREACH_SAFE(link, next, &watches) {
            watch = (SpiceWatch*)link;
            if (!watch->remove && (watch->event_mask & SPICE_WATCH_EVENT_READ)
                 && FD_ISSET(watch->fd, &rfds)) {
                watch->func(watch->fd, SPICE_WATCH_EVENT_READ, watch->opaque);
            }
            if (!watch->remove && (watch->event_mask & SPICE_WATCH_EVENT_WRITE)
                 && FD_ISSET(watch->fd, &wfds)) {
                watch->func(watch->fd, SPICE_WATCH_EVENT_WRITE, watch->opaque);
            }
            if (watch->remove) {
                ring_remove(&watch->link);
                free(watch);
            }
        }
    }
}

static int no_write_watches(Ring *w)
{
    SpiceWatch *watch;
    RingItem *link;
    RingItem *next;

    RING_FOREACH_SAFE(link, next, w) {
        watch = (SpiceWatch*)link;
        if (!watch->remove && (watch->event_mask & SPICE_WATCH_EVENT_WRITE))
            return 0;
    }

    return 1;
}

static void xspice_wakeup_handler(pointer data, int nfds, pointer readmask)
{
    if (!nfds && no_write_watches(&watches)) {
        return;
    }
    select_and_check_watches();
}

SpiceCoreInterface *basic_event_loop_init(void)
{
    ring_init(&watches);
    bzero(&core, sizeof(core));
    core.base.major_version = SPICE_INTERFACE_CORE_MAJOR;
    core.base.minor_version = SPICE_INTERFACE_CORE_MINOR; // anything less then 3 and channel_event isn't called
    core.timer_add = timer_add;
    core.timer_start = timer_start;
    core.timer_cancel = timer_cancel;
    core.timer_remove = timer_remove;
    core.watch_add = watch_add;
    core.watch_update_mask = watch_update_mask;
    core.watch_remove = watch_remove;
    core.channel_event = channel_event;
    return &core;
}

void xspice_register_handlers(void)
{
    RegisterBlockAndWakeupHandlers(xspice_block_handler, xspice_wakeup_handler, 0);
}

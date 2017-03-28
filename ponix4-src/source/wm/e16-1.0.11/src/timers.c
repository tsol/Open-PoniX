/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2006-2012 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#include "e16-ecore_list.h"
#include "timers.h"

struct _timer {
   unsigned int        in_time;
   unsigned int        at_time;
   struct _timer      *next;
   int                 (*func) (void *data);
   void               *data;
   char                again;
};

static int
tdiff(unsigned int t1, unsigned int t2)
{
   return (int)(t1 - t2);
}

static Timer       *q_first = NULL;

static void
_TimerSet(Timer * timer)
{
   Timer              *ptr, *pptr;

   if (EDebug(EDBUG_TYPE_TIMERS) > 1)
      Eprintf("%s %p: func=%p data=%p\n", __func__, timer, timer->func,
	      timer->data);

   /* if there is no queue it becomes the queue */
   if (!q_first)
     {
	q_first = timer;
	timer->next = NULL;
     }
   else
     {
	pptr = NULL;
	for (ptr = q_first; ptr; pptr = ptr, ptr = ptr->next)
	  {
	     if (tdiff(ptr->at_time, timer->at_time) > 0)
		break;
	  }
	if (pptr)
	   pptr->next = timer;
	else
	   q_first = timer;
	timer->next = ptr;
     }
}

static void
_TimerDel(Timer * timer)
{
   if (EDebug(EDBUG_TYPE_TIMERS))
      Eprintf("%s %p: func=%p data=%p\n", __func__, timer, timer->func,
	      timer->data);
   Efree(timer);
}

Timer              *
TimerAdd(int dt_ms, int (*func) (void *data), void *data)
{
   Timer              *timer;

   timer = EMALLOC(Timer, 1);
   if (!timer)
      return NULL;

   timer->in_time = (unsigned int)dt_ms;
   timer->at_time = GetTimeMs() + dt_ms;
   timer->func = func;
   timer->data = data;

   if (EDebug(EDBUG_TYPE_TIMERS))
      Eprintf("%s %p: func=%p data=%p: %8d\n", __func__, timer,
	      timer->func, timer->data, dt_ms);

   _TimerSet(timer);		/* Add to timer queue */

   return timer;
}

void
TimersRun(unsigned int t_ms)
{
   Timer              *timer, *q_old, *q_run;

   timer = q_first;
   if (!timer)
      return;			/* No timers pending */

   q_run = q_old = timer;
   for (; timer; timer = q_first)
     {
	if (tdiff(timer->at_time, t_ms) > 0)
	   break;

	if (EDebug(EDBUG_TYPE_TIMERS))
	   Eprintf("%s - run %p: func=%p data=%p: %8d\n", __func__, timer,
		   timer->func, timer->data, timer->at_time - t_ms);

	q_first = timer->next;

	/* Run this callback */
	timer->again = timer->func(timer->data);
	q_run = timer;
     }

   if (q_old != q_first)
     {
	/* At least one timer has run */
	q_run->next = NULL;	/* Terminate expired timer list */

	/* Re-schedule/remove timers that have run */
	for (timer = q_old; timer; timer = q_old)
	  {
	     q_old = timer->next;
	     if (timer->again)
	       {
		  timer->at_time += timer->in_time;
		  _TimerSet(timer);	/* Add to timer queue */
	       }
	     else
	       {
		  _TimerDel(timer);
	       }
	  }
     }

   if (EDebug(EDBUG_TYPE_TIMERS) > 1)
     {
	for (timer = q_first; timer; timer = timer->next)
	   Eprintf("%s - pend %p: func=%p data=%p: %8d (%d)\n", __func__,
		   timer, timer->func, timer->data, timer->at_time - t_ms,
		   timer->in_time);
     }
}

unsigned int
TimersRunNextIn(unsigned int t_ms)
{
   Timer              *timer;
   unsigned int        tn;

   timer = q_first;

   /* If the next (rescheduled) timer is already expired, set timeout time
    * to 1 ms. This avoids starving the fd's and should maintain the intended
    * (mean) timer rate.
    * The (mean) amount of work done in a timer function should of course not
    * exceed the timeout time. */
   if (timer)
      tn = (int)(timer->at_time - t_ms) > 0 ? timer->at_time - t_ms : 1;
   else
      tn = 0;

   if (EDebug(EDBUG_TYPE_TIMERS))
      Eprintf("%s - next in %8u\n", __func__, tn);

   return tn;
}

void
TimerDel(Timer * timer)
{
   Timer              *qe, *ptr, *pptr;

   pptr = NULL;
   for (ptr = q_first; ptr; pptr = ptr, ptr = ptr->next)
     {
	qe = ptr;
	if (qe != timer)
	   continue;

	/* Match - remove it from the queue */
	if (pptr)
	   pptr->next = qe->next;
	else
	   q_first = qe->next;

	/* free it */
	_TimerDel(timer);
	break;
     }
}

void
TimerSetInterval(Timer * timer, int dt_ms)
{
   timer->in_time = (unsigned int)dt_ms;
}

/*
 * Idlers
 */
static Ecore_List  *idler_list = NULL;

typedef void        (IdlerFunc) (void *data);

struct _idler {
   IdlerFunc          *func;
   void               *data;
};

Idler              *
IdlerAdd(IdlerFunc * func, void *data)
{
   Idler              *id;

   id = EMALLOC(Idler, 1);
   if (!id)
      return NULL;

   id->func = func;
   id->data = data;

   if (!idler_list)
      idler_list = ecore_list_new();

   ecore_list_append(idler_list, id);

   return id;
}

void
IdlerDel(Idler * id)
{
   ecore_list_node_remove(idler_list, id);
   Efree(id);
}

static void
_IdlerRun(void *_id, void *prm __UNUSED__)
{
   Idler              *id = (Idler *) _id;

   if (EDebug(EDBUG_TYPE_IDLERS) > 1)
      Eprintf("%s: func=%p\n", __func__, id->func);
   id->func(id->data);
}

void
IdlersRun(void)
{
   if (EDebug(EDBUG_TYPE_IDLERS))
      Eprintf("%s B\n", __func__);
   ecore_list_for_each(idler_list, _IdlerRun, NULL);
   if (EDebug(EDBUG_TYPE_IDLERS))
      Eprintf("%s E\n", __func__);
}

/*
 * Animators
 */
#define DEBUG_ANIMATORS 0
static Ecore_List  *animator_list = NULL;
static Timer       *animator_timer = NULL;

typedef int         (AnimatorFunc) (void *data);

struct _animator {
   AnimatorFunc       *func;
   void               *data;
};

static void
_AnimatorRun(void *_an, void *prm __UNUSED__)
{
   Animator           *an = (Animator *) _an;
   int                 again;

#if DEBUG_ANIMATORS > 2
   Eprintf("%s: %p\n", __func__, an);
#endif
   again = an->func(an->data);
   if (!again)
      AnimatorDel(an);
}

static int
AnimatorsRun(void *data __UNUSED__)
{
   static unsigned int tms_last = 0;
   unsigned int        dt_ms;

   dt_ms = Mode.events.time_ms - tms_last;
   if (2 * dt_ms < Conf.animation.step)
     {
#if DEBUG_ANIMATORS > 1
	Eprintf("%s: dt=%.3f - SKIP\n", __func__, 1e-3f * dt_ms);
#endif
	return 1;
     }

   ecore_list_for_each(animator_list, _AnimatorRun, NULL);

#if DEBUG_ANIMATORS > 1
   Eprintf("%s: dt=%.3f run=%.3f\n", __func__, 1e-3f * dt_ms,
	   1e-3f * (GetTimeMs() - Mode.events.time_ms));
#endif

   tms_last = Mode.events.time_ms;

   if (ecore_list_count(animator_list))
     {
	TimerSetInterval(animator_timer, Conf.animation.step);
	return 1;
     }
   else
     {
	animator_timer = NULL;
	return 0;
     }
}

Animator           *
AnimatorAdd(AnimatorFunc * func, void *data)
{
   Animator           *an;

   an = EMALLOC(Animator, 1);
   if (!an)
      return NULL;

#if DEBUG_ANIMATORS
   Eprintf("%s: %p func=%p data=%p\n", __func__, an, func, data);
#endif
   an->func = func;
   an->data = data;

   if (!animator_list)
      animator_list = ecore_list_new();

   ecore_list_append(animator_list, an);

   if (ecore_list_count(animator_list) == 1)
     {
	if (Conf.animation.step <= 0)
	   Conf.animation.step = 1;
	/* Animator list was empty - Add to timer qeueue */
	TIMER_ADD(animator_timer, Conf.animation.step, AnimatorsRun, NULL);
     }

   return an;
}

void
AnimatorDel(Animator * an)
{
#if DEBUG_ANIMATORS
   Eprintf("%s: %p func=%p data=%p\n", __func__, an, an->func, an->data);
#endif

   ecore_list_node_remove(animator_list, an);
   Efree(an);

   if (ecore_list_count(animator_list) == 0)
     {
	/* Animator list is empty - Remove from timer qeueue */
	TIMER_DEL(animator_timer);
     }
}

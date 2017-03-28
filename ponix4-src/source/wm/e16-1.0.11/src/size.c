/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2003-2012 Kim Woelders
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
#include "ewins.h"
#include "hints.h"
#include "screen.h"
#include "slide.h"

#define DEBUG_SIZE 0
#if DEBUG_SIZE
#define Dprintf Eprintf
#else
#define Dprintf(fmt...)
#endif

#define ENABLE_SMART_MAXIMISE	1

#define MAX_ABSOLUTE     0	/* Fill screen */
#define MAX_AVAILABLE    1	/* Expand until don't cover */
#define MAX_CONSERVATIVE 2	/* Expand until something */
#define MAX_XINERAMA     3	/* Fill Xinerama screen */

static int
_ignore(const EWin * ewin, int type)
{
   if (ewin->state.iconified || EoIsFloating(ewin) ||
       ewin->props.ignorearrange ||
       (type == MAX_AVAILABLE && !ewin->props.never_use_area))
      return 1;
   return 0;
}

static void
_get_span_x(const EWin * ewin, int type, EWin * const *lst, int num,
	    int *px1, int *px2)
{
   int                 i, sx1 = *px1, sx2 = *px2;
   int                 wx1, wx2, tx1, tx2;
   EWin               *e;

   wx1 = EoGetX(ewin);
   wx2 = wx1 + EoGetW(ewin);

   for (i = 0; i < num; i++)
     {
	e = lst[i];
	if (e == ewin || _ignore(e, type) ||
	    !SPANS_COMMON(EoGetY(ewin), EoGetH(ewin), EoGetY(e), EoGetH(e)))
	   continue;

	tx1 = EoGetX(e);
	tx2 = tx1 + EoGetW(e);
	if (tx2 <= wx1 && tx2 > sx1)
	   sx1 = tx2;
	else if (tx1 >= wx2 && tx1 < sx2)
	   sx2 = tx1;
     }

   *px1 = sx1;
   *px2 = sx2;
}

static void
_get_span_y(const EWin * ewin, int type, EWin * const *lst, int num,
	    int *py1, int *py2)
{
   int                 i, sy1 = *py1, sy2 = *py2;
   int                 wy1, wy2, ty1, ty2;
   const EWin         *e;

   wy1 = EoGetY(ewin);
   wy2 = wy1 + EoGetH(ewin);

   for (i = 0; i < num; i++)
     {
	e = lst[i];
	if (e == ewin || _ignore(e, type) ||
	    !SPANS_COMMON(EoGetX(ewin), EoGetW(ewin), EoGetX(e), EoGetW(e)))
	   continue;

	ty1 = EoGetY(e);
	ty2 = ty1 + EoGetH(e);
	if (ty2 <= wy1 && ty2 > sy1)
	   sy1 = ty2;
	else if (ty1 >= wy2 && ty1 < sy2)
	   sy2 = ty1;
     }

   *py1 = sy1;
   *py2 = sy2;
}

#if ENABLE_SMART_MAXIMISE
/*
 * Smart window sizing algorithm.
 * Based on pareto frontiers and constraint unification.
 * Blame it on Dan Manjarres.
 *
 * The algorithm is to treat the center of the window being maximized as the
 * center of a cartesian coordinate system, and to find a pareto frontier
 * in each quadrant of the cartesian plane. The frontier is found from points
 * that are the corners of other windows or the nearest point on the edge of a
 * window that spans 2 quadrants. Windows that span more than 2 quadrants are
 * ignored since they already overlap with the original window.
 *
 * The frontiers encode a discrete set of possible window edges, which are sorted
 * and tested in sequence until the one with the largest area is found.
 *
 * Filtering is done to ignore windows that are visually "under"
 * a window that is overlapped so that hidden windows are not counted as
 * constraining maximization. Windows that have the never use flag are not
 * ignored in this manner. Windows with aspect ratio hints are respected, and
 * placed as close as possible to their original position. All windows may be
 * slightly shifted to get more area, as the only point that represents the
 * window is its center, around which the constraints are placed. The new
 * center may be located anywhere in region of the screen defined by partial
 * orders of windows along x and y, and conflicts between a windows existing edges
 * and resizing the window are completely bypassed. In other words you don't have
 * to grab the mouse to move the window before maximizing it, and you don't need the
 * mouse to maximizing it either. Alt-Tab and <maximize key command> are all you need.
 *
 * computation proceeeds as
 *
 * 1) list all windows
 * 2) filter out windows that overlap maximizing window, and all windows under them.
 * 3) record the 4 corners of each window in a constraint array
 * 4) compute the nearest point of any window that crosses the x or y axis and store
 * 	it as a  constraint.
 * 5) make 4 copies of the array and adjust for different search directions
 * 6) for each copy filter in preparation for pareto criteria for each corner of the window
 * 6a) for upper corners ignore windows that are below window center
 * 6b) for lower corners ignore windows that are above below window center
 * 6c) for left  corners ignore windows that are right of window center
 * 6d) for right corners ignore windows that are left of window center
 * 7) perform pareto frontier filtering for each quadrant
 *     after this step the remaining points  are the pareto frontier for each corner
 *     of the window in isolation, and form a not-too-nonconvex hull topologically equal
 *     to a circle around the original window's center.
 *
 *     Each discrete x value and y value represents a potential location
 *     for sides of the window, and each window side will touch
 *     at least one of its potential corner points.
 *
 * 8) combine the adjacent corners for each side into arrays holding potential
 *    window edge locations and maximum spans sorted by distance from center.
 * 9) for each pair of top and bottom constraints find the available area from
 *    the left and right constraints.
 * 10) adjust for aspect ratio.
 * 11) pick the biggest one
 * 12) and on the seventh day rest
 * */

typedef struct {
   int                 dx;
   int                 dy;
   int                 dominated;
} point;

static int
_gti(const void *p1, const void *p2)
{
   return (*(int *)p1) - *((int *)p2);
}

static int
_lti(const void *p1, const void *p2)
{
   return (*(int *)p2) - *((int *)p1);
}

static int
_gtx(const void *p1, const void *p2)
{
   return ((point *) p1)->dx - ((point *) p2)->dx;
}

static int
_ltx(const void *p1, const void *p2)
{
   return ((point *) p2)->dx - ((point *) p1)->dx;
}

#if 0
static int
_gty(const void *p1, const void *p2)
{
   return ((point *) p1)->dy - ((point *) p2)->dy;
}

static int
_lty(const void *p1, const void *p2)
{
   return ((point *) p2)->dy - ((point *) p1)->dy;
}
#endif

static void
sort_ints(int *is, int n, int ascending)
{
   if (ascending >= 0)
      qsort(is, n, sizeof(int), _gti);
   else
      qsort(is, n, sizeof(int), _lti);
}

static void
sort_points_x(point * ps, int n, int ascending)
{
   if (ascending >= 0)
      qsort(ps, n, sizeof(point), _gtx);
   else
      qsort(ps, n, sizeof(point), _ltx);
}

#if 0
static void
sort_points_y(point * ps, int n, int ascending)
{
   if (ascending >= 0)
      qsort(ps, n, sizeof(point), _gty);
   else
      qsort(ps, n, sizeof(point), _lty);
}
#endif

static void
uniq_ints(int *is, int *n)
{
   int                 i, j;

   for (i = 0, j = 0; i < *n - 1; i++)
     {
	if (is[i] != is[i + 1])
	  {
	     is[j] = is[i];
	     j++;
	  }
     }
   is[j] = is[i];
   j++;
   *n = j;
}

static void
filter_points(point * p, int *np, int max_dx, int max_dy)
{
   int                 i, j, n = *np;

   /* this is step 6 from above */
   for (i = 0; i < n; i++)
     {
	if (p[i].dx < 0)
	   p[i].dominated = 1;
	if (p[i].dy < 0)
	   p[i].dominated = 1;
	/* clip to screen */
	if (p[i].dx > max_dx)
	   p[i].dx = max_dx;
	if (p[i].dy > max_dy)
	   p[i].dy = max_dy;
     }

   /* this is step 7 from above */
   for (i = 0; i < n; i++)
     {
	if (p[i].dominated)
	   continue;

	for (j = 0; j < n; j++)
	  {
	     if (i == j)
		continue;	/* self dominance = no */
	     if ((p[i].dx <= p[j].dx) && (p[i].dy <= p[j].dy))
	       {
		  p[j].dominated = 1;
	       }
	  }
     }

   /* actual pareto frontier filtering */
   for (i = 0, j = 0; i < n; i++)
     {
	if (!p[i].dominated)
	  {
	     p[j] = p[i];
	     j++;
	  }
     }

   *np = j;
}

#define _get_span_xy pareto_maximizer
static void
pareto_maximizer(EWin * ewin, int type, EWin * const *lst, int num,
		 int *avail_x1, int *avail_x2, int *avail_y1, int *avail_y2)
{
   int                 x, y, w, h;
   int                 cx, cy;	/* center */
   int                 i, j, k;
   point              *constraints_tr = NULL;	/* top right   */
   point              *constraints_tl = NULL;	/* top left    */
   point              *constraints_br = NULL;	/* botom right */
   point              *constraints_bl = NULL;	/* botom left  */
   int                 num_tr, num_tl, num_br, num_bl;
   int                *top_ds = NULL;
   int                *bottom_ds = NULL;
   point              *left_ds = NULL;
   point              *right_ds = NULL;
   int                *tc, *bc;
   point              *lc, *rc;	/* temp cursors */
   int                 num_t, num_b, num_l, num_r;
   float               aspect;
   EWin              **filtered_lst, **stacked_lst, *pe, *pe2;
   char               *done_stacking_flag;
   char               *stacked_above_flag;
   int                 num_stacked, stacked_above;
   int                 area, new_area;
   int                 recenter, new_recenter;
   int                 td, bd, ld, rd;	/* displacement to maximized edges */

   Dprintf("searching within %d-%d %d-%d\n",
	   *avail_x1, *avail_x2, *avail_y1, *avail_y2);
   x = EoGetX(ewin);
   y = EoGetY(ewin);
   h = EoGetH(ewin);
   w = EoGetW(ewin);
   cx = x + w / 2;
   cy = y + h / 2;

   /* center must be within available region */
   cx = (cx >= *avail_x2) ? *avail_x2 - 1 : cx;
   cy = (cy >= *avail_y2) ? *avail_y2 - 1 : cy;
   cx = (cx < *avail_x1) ? *avail_x1 : cx;
   cy = (cy < *avail_y1) ? *avail_y1 : cy;

   filtered_lst = EMALLOC(EWin *, num);
   stacked_lst = EMALLOC(EWin *, num + 1);
   stacked_above_flag = ECALLOC(char, num);
   done_stacking_flag = ECALLOC(char, num);

   if (!filtered_lst || !stacked_lst || !done_stacking_flag ||
       !stacked_above_flag)
      goto freedom;

   stacked_lst[0] = ewin;
   num_stacked = 1;
   /* ignore windows already overlapping ours and any windows UNDER them */

   /* start by detecting windows we overlap */
   for (i = 0, stacked_above = 0; i < num; i++)
     {
	pe = lst[i];

	if (pe == ewin)
	  {
	     stacked_above = 1;
	  }
	if ((pe == ewin) || _ignore(pe, type))
	  {
	     done_stacking_flag[i] = 1;
	     Dprintf("ignoring #%d %s\n", i, EwinGetTitle(pe));
	     continue;
	  }

	if (SPANS_COMMON(x + 1, w - 2, EoGetX(pe), EoGetW(pe)) &&
	    SPANS_COMMON(y + 1, h - 2, EoGetY(pe), EoGetH(pe)))
	  {
	     stacked_lst[num_stacked] = pe;
	     stacked_above_flag[num_stacked] = stacked_above;
	     num_stacked++;
	     done_stacking_flag[i] = 1;
	     Dprintf("overlap #%d %s\n", i, EwinGetTitle(pe));
	  }
	else
	  {
	     Dprintf("do not overlap #%d %s\n", i, EwinGetTitle(pe));
	  }
     }

   /* extend the stacked list to windows that are UNDER other items in the stacked list */
   for (i = 1; i < num_stacked; i++)
     {
	int                 sx, sy, sw, sh;

	pe2 = stacked_lst[i];
	sx = EoGetX(pe2);
	sy = EoGetY(pe2);
	sh = EoGetH(pe2);
	sw = EoGetW(pe2);

	/* find stacked_lst window pe2 in the general window list */
	for (j = 0; j < num; j++)
	  {
	     pe = lst[j];
	     if (pe == pe2)
		break;
	  }
	if (stacked_above_flag[i])
	  {
	     Dprintf("metaoverlap testing from understacked %s\n",
		     EwinGetTitle(pe));
	  }
	else
	  {
	     Dprintf("NOT metaoverlap testing from overstacked %s\n",
		     EwinGetTitle(pe));
	     continue;
	  }

	for (; j < num; j++)
	  {
	     pe = lst[j];

	     if (done_stacking_flag[j])
		continue;
	     if (pe->props.never_use_area)
	       {
		  Dprintf("not using area %s\n", EwinGetTitle(pe));
		  continue;
	       }

	     if (SPANS_COMMON(sx + 1, sw - 2, EoGetX(pe), EoGetW(pe)) &&
		 SPANS_COMMON(sy + 1, sh - 2, EoGetY(pe), EoGetH(pe)))
	       {
		  /* the list is already top down so if it overlaps it's also under */
		  /* we hope! */
		  stacked_lst[num_stacked] = pe;
		  stacked_above_flag[num_stacked] = 1;
		  num_stacked++;
		  done_stacking_flag[j] = 1;
		  Dprintf("metaoverlap #%d %s\n", j, EwinGetTitle(pe));
	       }
	     else
	       {
		  Dprintf("no metaoverlap #%d %s\n", j, EwinGetTitle(pe));
	       }
	  }
     }

   /* copy remaining windows to our working set */
   for (i = 0, j = 0; i < num; i++)
     {
	pe = lst[i];

	if (done_stacking_flag[i])
	  {
	     Dprintf("no stacked constraint from #%d %s\n", i,
		     EwinGetTitle(pe));
	     continue;
	  }

	if ((pe == ewin) || _ignore(pe, type) ||
	    /* ignore windws that do not overlap with current search area */
	    !(SPANS_COMMON(*avail_x1, *avail_x2 - *avail_x1,
			   EoGetX(pe), EoGetW(pe)) &&
	      SPANS_COMMON(*avail_y1, *avail_y2 - *avail_y1,
			   EoGetY(pe), EoGetH(pe))))
	  {
	     Dprintf("no constraint from %s\n", EwinGetTitle(pe));
	     continue;
	  }
	Dprintf("constraint from %s\n", EwinGetTitle(pe));
	filtered_lst[j] = pe;
	j++;
     }

   /* allocate memory to hold constraints */
   num = j + 1;
   num_tl = num_bl = num_br = num_tr = num * 5;
   constraints_tl = EMALLOC(point, num * 5);
   constraints_tr = EMALLOC(point, num * 5);
   constraints_bl = EMALLOC(point, num * 5);
   constraints_br = ECALLOC(point, num * 5);

   if (!constraints_tl || !constraints_tr || !constraints_bl || !constraints_br)
      goto freedom;

   for (i = 1; i < num; i++)
     {
	pe = filtered_lst[i - 1];
	/* store window corners as candidate constraints */
	constraints_br[5 * i + 0].dx = EoGetX(pe);
	constraints_br[5 * i + 0].dy = EoGetY(pe);
	constraints_br[5 * i + 1].dx = EoGetX(pe) + EoGetW(pe);
	constraints_br[5 * i + 1].dy = EoGetY(pe);
	constraints_br[5 * i + 2].dx = EoGetX(pe) + EoGetW(pe);
	constraints_br[5 * i + 2].dy = EoGetY(pe) + EoGetH(pe);
	constraints_br[5 * i + 3].dx = EoGetX(pe);
	constraints_br[5 * i + 3].dy = EoGetY(pe) + EoGetH(pe);

	/* if window occupies 2 quadrants add a constraint for the
	 * intersection of the x or y axis and the window */
	if (SPANS_COMMON(EoGetX(pe), EoGetW(pe), cx, 1))
	  {
	     Dprintf("got horiz edge contraint\n");
	     constraints_br[5 * i + 4].dx = cx;
	     if (EoGetY(pe) > cy)
	       {
		  constraints_br[5 * i + 4].dy = EoGetY(pe);
	       }
	     else
	       {
		  constraints_br[5 * i + 4].dy = EoGetY(pe) + EoGetH(pe);
	       }
	  }
	else if (SPANS_COMMON(EoGetY(pe), EoGetH(pe), cy, 1))
	  {
	     Dprintf("got ver edge contraint\n");
	     constraints_br[5 * i + 4].dy = cy;
	     if (EoGetX(pe) > cx)
	       {
		  constraints_br[5 * i + 4].dx = EoGetX(pe);
	       }
	     else
	       {
		  constraints_br[5 * i + 4].dx = EoGetX(pe) + EoGetW(pe);
	       }
	  }
	else
	  {
	     constraints_br[5 * i + 4].dominated = 1;
	     Dprintf("got no edge constraint from win #%d %s\n", i - 1,
		     EwinGetTitle(pe));
	  }
     }

   /* add constraints to keep the window on-screen */
   constraints_br[0].dx = cx;
   constraints_br[0].dy = *avail_y1 - 1;
   constraints_br[1].dx = cx;
   constraints_br[1].dy = *avail_y2 + 1;
   constraints_br[2].dx = *avail_x1 - 1;
   constraints_br[2].dy = cy;
   constraints_br[3].dx = *avail_x2 + 1;
   constraints_br[3].dy = cy;
   constraints_br[4].dominated = 1;

   /* subtract out center to get distance to constraints */
   for (i = 0; i < num_tr; i++)
     {
	constraints_br[i].dx -= cx;
	constraints_br[i].dy -= cy;
     }

   /* make 4 copies: one for each corner to optimize */
   memcpy(constraints_tl, constraints_br, sizeof(point) * num_tl);
   memcpy(constraints_bl, constraints_br, sizeof(point) * num_tl);
   memcpy(constraints_tr, constraints_br, sizeof(point) * num_tl);

   for (i = 0; i < num_tr; i++)
     {
	/* correct displacements to be positive in the direction of expansion */
	constraints_tl[i].dx *= -1;
	constraints_tl[i].dy *= -1;

	constraints_bl[i].dx *= -1;

	constraints_tr[i].dy *= -1;
     }

   /* bust out that pareto frontier */
   filter_points(constraints_tl, &num_tl, cx - *avail_x1, cy - *avail_y1);
   filter_points(constraints_tr, &num_tr, *avail_x2 - cx, cy - *avail_y1);
   filter_points(constraints_bl, &num_bl, cx - *avail_x1, *avail_y2 - cy);
   filter_points(constraints_br, &num_br, *avail_x2 - cx, *avail_y2 - cy);

   /* now need to convert corner constraints to candidate edge constraints */
   num_t = num_tl + num_tr;
   num_b = num_bl + num_br;
   num_l = num_tl + num_bl;
   num_r = num_tr + num_br;
   tc = top_ds = EMALLOC(int, num_t);
   bc = bottom_ds = EMALLOC(int, num_b);

   lc = left_ds = EMALLOC(point, num_l);
   rc = right_ds = EMALLOC(point, num_r);

   if (!tc || !bc || !lc || !rc)
      goto freedom;

   /* convert constraint distances back to to constraint displacements
    * using cursor pointers to accumulate constraints for each edge */
   for (i = 0; i < num_tr; i++)
     {
	rc->dx = constraints_tr[i].dx;
	rc->dy = -constraints_tr[i].dy;
	*tc = -constraints_tr[i].dy;
	tc++;
	rc++;
     }
   for (i = 0; i < num_tl; i++)
     {
	lc->dx = -constraints_tl[i].dx;
	lc->dy = -constraints_tl[i].dy;
	*tc = -constraints_tl[i].dy;
	tc++;
	lc++;
     }
   for (i = 0; i < num_br; i++)
     {
	rc->dx = constraints_br[i].dx;
	rc->dy = constraints_br[i].dy;
	*bc = constraints_br[i].dy;
	bc++;
	rc++;
     }
   for (i = 0; i < num_bl; i++)
     {
	lc->dx = -constraints_bl[i].dx;
	lc->dy = constraints_bl[i].dy;
	*bc = constraints_bl[i].dy;
	bc++;
	lc++;
     }

   /* sort the lists for easy searching */
   sort_points_x(left_ds, num_l, +1);
   sort_points_x(right_ds, num_r, -1);
   sort_ints(top_ds, num_t, +1);
   sort_ints(bottom_ds, num_b, -1);
   uniq_ints(top_ds, &num_t);
   uniq_ints(bottom_ds, &num_b);

   /* brute force test the combinitorics:
    * for each pair of possible top and bottom displacements,
    * find the best possible pair of left and right displacements */

   area = 0;
   recenter = 0;

   for (i = 0; i < num_t; i++)	/* top indices */
     {
	for (j = 0; j < num_b; j++)	/* bottom indices */
	  {
	     int                 new_w, new_h, trim;

	     td = top_ds[i];	/* displacement to top */
	     bd = bottom_ds[j];	/* displacement to bottom */
	     Dprintf("starting search in td-bd %d-%d\n", td, bd);
	     ld = left_ds[0].dx;
	     rd = right_ds[0].dx;
	     Dprintf("search in y from %d to %d\n", td, bd);

	     /* find furthest left given top, bottom */
	     for (k = 0; k < num_l; k++)
	       {
		  if (left_ds[k].dy <= td)	/* constraint point is above top, skip it */
		    {
		       Dprintf("left ignoring above point %d,%d\n",
			       left_ds[k].dx, left_ds[k].dy);
		       continue;
		    }
		  if (left_ds[k].dy >= bd)	/* constraint point is below bottom, skip it */
		    {
		       Dprintf("left ignoring below point %d,%d\n",
			       left_ds[k].dx, left_ds[k].dy);
		       continue;
		    }
		  Dprintf("left using point %d,%d\n", left_ds[k].dx,
			  left_ds[k].dy);
		  if (left_ds[k].dx > ld)
		     ld = left_ds[k].dx + 1;
	       }

	     /* find furthest right given top, bottom */
	     for (k = 0; k < num_r; k++)
	       {
		  if (right_ds[k].dy <= td)	/* constraint point is above top, skip it */
		    {
		       Dprintf("right ignoring above point %d,%d\n",
			       right_ds[k].dx, right_ds[k].dy);
		       continue;
		    }
		  if (right_ds[k].dy >= bd)	/* constraint point is below bottom, skip it */
		    {
		       Dprintf("right ignoring below point %d,%d\n",
			       right_ds[k].dx, right_ds[k].dy);
		       continue;
		    }
		  Dprintf("right using point %d,%d\n", right_ds[k].dx,
			  right_ds[k].dy);
		  if (right_ds[k].dx < rd)
		     rd = right_ds[k].dx - 1;
	       }

	     /* almost there..... need to correct for aspect ratio
	      * and keep center as close as possible to old center
	      * in case of duplicates...... */

	     new_w = 1 - ld + rd;
	     new_h = 1 - td + bd;
	     aspect = (float)new_w / new_h;
	     new_area = new_w * new_h;

	     if (new_area > area)
	       {
		  if (aspect > ewin->icccm.aspect_max)
		    {
		       do
			 {
			    if (abs(ld) < abs(rd))
			      {
				 Dprintf("trimming right for aspect\n");
				 rd--;
			      }
			    else
			      {
				 Dprintf("trimming left for aspect\n");
				 ld++;
			      }
			    new_w = 1 - ld + rd;
			    trim =
			       new_w - (int)(new_h * ewin->icccm.aspect_min);
			 }
		       while (trim > 0);
		       Dprintf("ld now %d, rd now %d\n", ld, rd);
		       Dprintf("td now %d, bd now %d\n", td, bd);
		       Dprintf("\n");
		    }
		  else if (aspect < ewin->icccm.aspect_min)
		    {
		       do
			 {
			    if (abs(td) < abs(bd))
			      {
				 Dprintf("trimming bottom for aspect\n");
				 bd--;
			      }
			    else
			      {
				 Dprintf("trimming top for aspect\n");
				 td++;
			      }
			    new_h = 1 - td + bd;
			    trim =
			       new_h - (int)(new_w / ewin->icccm.aspect_min);
			 }
		       while (trim > 0);
		       Dprintf("ld now %d, rd now %d\n", ld, rd);
		       Dprintf("td now %d, bd now %d\n", td, bd);
		       Dprintf("\n");
		    }

		  new_area = new_w * new_h;
		  new_recenter = abs(td) + abs(bd) + abs(ld) + abs(rd);

		  if ((new_area > area) ||
		      ((new_area == area) && (new_recenter < recenter)))
		    {
		       Dprintf("new area %ld old area %ld\n", new_area, area);
		       Dprintf("%d-%d x %d-%d\n", x, x + w, y, y + h);
		       area = new_area;
		       recenter = new_recenter;
		       x = cx + ld;
		       y = cy + td;
		       w = (1 - ld + rd);
		       h = (1 - td + bd);
		    }
	       }
	     Dprintf("===========================\n");
	  }
     }

   *avail_x1 = x;
   *avail_x2 = x + w;
   *avail_y1 = y;
   *avail_y2 = y + h;

   /* rest a while */

 freedom:
   Efree(top_ds);
   Efree(bottom_ds);
   Efree(left_ds);
   Efree(right_ds);
   Efree(constraints_tl);
   Efree(constraints_tr);
   Efree(constraints_bl);
   Efree(constraints_br);
   Efree(filtered_lst);
   Efree(stacked_lst);
   Efree(done_stacking_flag);
   Efree(stacked_above_flag);
}
#endif /* ENABLE_SMART_MAXIMISE */

void
MaxSizeHV(EWin * ewin, const char *resize_type, int hor, int ver)
{
   int                 x, y, w, h, x1, x2, y1, y2, type, bl, br, bt, bb;
   EWin               *const *lst;
   int                 num;
   int                 old_hor = ewin->state.maximized_horz != 0;
   int                 old_ver = ewin->state.maximized_vert != 0;

   if (!ewin)
      return;

   if (ewin->state.inhibit_max_hor && hor)
      return;
   if (ewin->state.inhibit_max_ver && ver)
      return;

   if (!old_hor && !old_ver)
     {
	ewin->save_max.x = EoGetX(ewin);
	ewin->save_max.y = EoGetY(ewin);
	ewin->save_max.w = ewin->client.w;
	ewin->save_max.h = ewin->client.h;
     }

   /* Figure out target state */
   if (hor && ver)
     {
	hor = ver = ((old_hor && old_ver) ? 0 : 1);
     }
   else
     {
	hor = (hor) ? !old_hor : old_hor;
	ver = (ver) ? !old_ver : old_ver;
     }

   ewin->state.maximized_horz = hor;
   ewin->state.maximized_vert = ver;

   Dprintf("h/v old = %d/%d new=%d/%d\n", old_hor, old_ver, hor, ver);
   if (!hor && !ver)
     {
	/* Restore regular state */
	EwinSlideSizeTo(ewin, ewin->save_max.x, ewin->save_max.y,
			ewin->save_max.w, ewin->save_max.h,
			Conf.movres.maximize_speed, 0, 0);
	goto done;
     }
   if (old_ver == ver && old_hor && !hor)
     {
	/* Turn off horizontal maxsize */
	EwinSlideSizeTo(ewin, ewin->save_max.x, EoGetY(ewin),
			ewin->save_max.w, ewin->client.h,
			Conf.movres.maximize_speed, 0, 0);
	goto done;
     }
   if (old_hor == hor && old_ver && !ver)
     {
	/* Turn off vertical maxsize */
	EwinSlideSizeTo(ewin, EoGetX(ewin), ewin->save_max.y,
			ewin->client.w, ewin->save_max.h,
			Conf.movres.maximize_speed, 0, 0);
	goto done;
     }

   type = MAX_ABSOLUTE;		/* Select default */
   if (!resize_type || !resize_type[0])
      type = Conf.movres.mode_maximize_default;
   else if (!strcmp(resize_type, "absolute"))
      type = MAX_ABSOLUTE;
   else if (!strcmp(resize_type, "available"))
      type = MAX_AVAILABLE;
   else if (!strcmp(resize_type, "conservative"))
      type = MAX_CONSERVATIVE;
   else if (!strcmp(resize_type, "xinerama"))
      type = MAX_XINERAMA;

   /* Default is no change */
   x = EoGetX(ewin);
   y = EoGetY(ewin);
   h = EoGetH(ewin);
   w = EoGetW(ewin);

   switch (type)
     {
     case MAX_XINERAMA:
	if (hor)
	  {
	     x = 0;
	     w = WinGetW(VROOT);
	  }
	if (ver)
	  {
	     y = 0;
	     h = WinGetH(VROOT);
	  }
	break;

     default:
     case MAX_ABSOLUTE:
     case MAX_AVAILABLE:
     case MAX_CONSERVATIVE:
	ScreenGetAvailableArea(x + w / 2, y + h / 2, &x1, &y1, &x2, &y2,
			       Conf.place.ignore_struts_maximize);
	x2 += x1;
	y2 += y1;

	if (Conf.movres.dragbar_nocover && type != MAX_ABSOLUTE)
	  {
	     /* Leave room for the dragbar */
	     switch (Conf.desks.dragdir)
	       {
	       case 0:		/* left */
		  if (x1 < Conf.desks.dragbar_width)
		     x1 = Conf.desks.dragbar_width;
		  break;

	       case 1:		/* right */
		  if (x2 > WinGetW(VROOT) - Conf.desks.dragbar_width)
		     x2 = WinGetW(VROOT) - Conf.desks.dragbar_width;
		  break;

	       case 2:		/* top */
		  if (y1 < Conf.desks.dragbar_width)
		     y1 = Conf.desks.dragbar_width;
		  break;

	       case 3:		/* bottom */
		  if (y2 > WinGetH(VROOT) - Conf.desks.dragbar_width)
		     y2 = WinGetH(VROOT) - Conf.desks.dragbar_width;
		  break;

	       default:
		  break;
	       }
	  }

	if (type == MAX_ABSOLUTE)
	  {
	     /* Simply ignore all windows */
	     lst = NULL;
	     num = 0;
	  }
	else
	  {
	     lst = EwinListGetForDesk(&num, EoGetDesk(ewin));
	  }

#if ENABLE_SMART_MAXIMISE
	if (type == MAX_CONSERVATIVE && ver && hor &&
	    ( /*(!old_hor && !old_ver) || */ Conf.movres.enable_smart_max_hv))
	  {
	     _get_span_xy(ewin, type, lst, num, &x1, &x2, &y1, &y2);
	     x = x1;
	     w = x2 - x1;
	     y = y1;
	     h = y2 - y1;
	     break;
	  }
#endif

	if (ver)
	  {
	     _get_span_y(ewin, type, lst, num, &y1, &y2);
	     y = y1;
	     h = y2 - y1;
	  }

	if (hor)
	  {
	     _get_span_x(ewin, type, lst, num, &x1, &x2);
	     x = x1;
	     w = x2 - x1;
	  }

	break;
     }

   EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);
   w -= (bl + br);
   if (w < 10)
      w = 10;
   h -= (bt + bb);
   if (h < 10)
      h = 10;

   EwinSlideSizeTo(ewin, x, y, w, h, Conf.movres.maximize_speed, 0, 0);
 done:
   HintsSetWindowState(ewin);
}

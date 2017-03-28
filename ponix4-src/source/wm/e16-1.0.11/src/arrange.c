/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2012 Kim Woelders
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
#include "desktops.h"
#include "ewins.h"
#include "groups.h"
#include "screen.h"
#include "slide.h"

#define DEBUG_ARRANGE 0

typedef struct {
   void               *data;
   int                 x, y, w, h;
   int                 p;
} RectBox;

typedef struct {
   int                 x, y;
   int                 p, q;
} RectInfo;

static int
ArrangeAddToList(int *array, int current_size, int value)
{
   int                 i, j;

   if (current_size >= 2 &&
       (value <= array[0] || value >= array[current_size - 1]))
      return current_size;

   for (i = 0; i < current_size; i++)
     {
	if (value < array[i])
	  {
	     for (j = current_size; j > i; j--)
		array[j] = array[j - 1];
	     array[i] = value;
	     return current_size + 1;
	  }
	else if (value == array[i])
	   return current_size;
     }
   array[current_size] = value;
   return current_size + 1;
}

static void
ArrangeMakeXYArrays(int tx1, int tx2, int ty1, int ty2, int fitw, int fith,
		    const RectBox * sorted, int num_sorted,
		    int *xarray, int *nx, int *yarray, int *ny)
{
   int                 j, x1, x2, y1, y2;
   int                 xsize, ysize;

   xsize = 0;
   ysize = 0;

   /* put all the sorted rects into the xy arrays */
   xsize = ArrangeAddToList(xarray, xsize, tx1);
   xsize = ArrangeAddToList(xarray, xsize, tx2);
   xsize = ArrangeAddToList(xarray, xsize, tx2 - fitw);
   ysize = ArrangeAddToList(yarray, ysize, ty1);
   ysize = ArrangeAddToList(yarray, ysize, ty2);
   ysize = ArrangeAddToList(yarray, ysize, ty2 - fith);

   for (j = 0; j < num_sorted; j++)
     {
	x1 = sorted[j].x;
	x2 = x1 + sorted[j].w;
	xsize = ArrangeAddToList(xarray, xsize, x1);
	xsize = ArrangeAddToList(xarray, xsize, x2);
	xsize = ArrangeAddToList(xarray, xsize, x1 - fitw);
	xsize = ArrangeAddToList(xarray, xsize, x2 - fitw);
	y1 = sorted[j].y;
	y2 = y1 + sorted[j].h;
	ysize = ArrangeAddToList(yarray, ysize, y1);
	ysize = ArrangeAddToList(yarray, ysize, y2);
	ysize = ArrangeAddToList(yarray, ysize, y1 - fith);
	ysize = ArrangeAddToList(yarray, ysize, y2 - fith);
     }
#if DEBUG_ARRANGE
   for (j = 0; j < xsize; j++)
      Eprintf("xarray[%d] = %d\n", j, xarray[j]);
   for (j = 0; j < ysize; j++)
      Eprintf("yarray[%d] = %d\n", j, yarray[j]);
#endif

   *nx = xsize;
   *ny = ysize;
}

#define Filled(x,y) (filled[(y * (xsize - 1)) + x])

static void
ArrangeMakeFillLists(const RectBox * sorted, int num_sorted,
		     int *xarray, int xsize, int *yarray, int ysize,
		     unsigned char *filled)
{
   int                 j, x1, x2, y1, y2, k, y, x;

   /* fill the allocation array */
   for (j = 0; j < (xsize - 1) * (ysize - 1); filled[j++] = 0)
      ;
   for (j = 0; j < num_sorted; j++)
     {
	if (sorted[j].x + sorted[j].w <= xarray[0] ||
	    sorted[j].x >= xarray[xsize - 1])
	   continue;
	if (sorted[j].y + sorted[j].h <= yarray[0] ||
	    sorted[j].y >= yarray[ysize - 1])
	   continue;
	x1 = -1;
	x2 = -1;
	y1 = -1;
	y2 = -1;
	for (k = 0; k < xsize - 1; k++)
	   if (sorted[j].x <= xarray[k])
	     {
		x1 = x2 = k;
		break;
	     }
	for (k++; k < xsize - 1; k++)
	   if (sorted[j].x + sorted[j].w > xarray[k])
	      x2 = k;
	for (k = 0; k < ysize - 1; k++)
	   if (sorted[j].y <= yarray[k])
	     {
		y1 = y2 = k;
		break;
	     }
	for (k++; k < ysize - 1; k++)
	   if (sorted[j].y + sorted[j].h > yarray[k])
	      y2 = k;
#if DEBUG_ARRANGE
	Eprintf("Fill %4d,%4d %4dx%4d: (%2d)%4d->(%2d)%4d,(%2d)%4d->(%2d)%4d\n",
		sorted[j].x, sorted[j].y, sorted[j].w, sorted[j].h,
		x1, xarray[x1], x2, xarray[x2], y1, yarray[y1], y2, yarray[y2]);
#endif
	if ((x1 >= 0) && (x2 >= 0) && (y1 >= 0) && (y2 >= 0))
	  {
	     for (y = y1; y <= y2; y++)
	       {
		  for (x = x1; x <= x2; x++)
		    {
		       Filled(x, y) += sorted[j].p;
		    }
	       }
	  }
     }

#if DEBUG_ARRANGE
   Eprintf("Filled[%2d,%2d] =\n", xsize, ysize);
   for (k = 0; k < ysize - 1; k++)
     {
	for (j = 0; j < xsize - 1; j++)
	   printf(" %2d", Filled(j, k));
	printf("\n");
     }
#endif
}

static void
ArrangeFindSpace(const int *xarray, int xsize, const int *yarray, int ysize,
		 unsigned char *filled, RectInfo * spaces, int *ns,
		 int wx, int wy, int ww, int wh)
{
   int                 i, j, w, h, fw, fh, z1, z2;
   int                 cost, desk;
   int                 num_spaces = *ns;

   if (wx < xarray[0] || (wx != xarray[0] && wx + ww > xarray[xsize - 1]))
      return;
   if (wy < yarray[0] || (wy != yarray[0] && wy + wh > yarray[ysize - 1]))
      return;

   cost = desk = 0;
   fh = wh;
#if DEBUG_ARRANGE > 1
   Eprintf("Check-A %d,%d %dx%d\n", wx, wy, ww, wh);
#endif
   for (j = 0; j < ysize - 1; j++)
     {
	z2 = yarray[j + 1];
	if (z2 <= wy)
	   continue;

	z1 = wy > yarray[j] ? wy : yarray[j];
	z2 = wy + wh < z2 ? wy + wh : z2;
	h = z2 - z1;
	fw = ww;
	for (i = 0; i < xsize - 1; i++)
	  {
	     z2 = xarray[i + 1];
	     if (z2 <= wx)
		continue;

	     z1 = wx > xarray[i] ? wx : xarray[i];
	     z2 = wx + ww < z2 ? wx + ww : z2;
	     w = z2 - z1;
#if DEBUG_ARRANGE > 1
	     Eprintf("Add [%d,%d] %3dx%3d: %2d\n", i, j, w, h, Filled(i, j));
#endif
	     if (Filled(i, j) == 0)
		desk += w * h;
	     else
		cost += w * h * Filled(i, j);
	     fw -= w;
	     if (fw <= 0)
		break;
	  }
	fh -= h;
	if (fh <= 0)
	   break;
     }

#if DEBUG_ARRANGE
   Eprintf("Check %4d,%4d %3dx%3d cost=%d desk=%d\n", wx, wy, ww, wh,
	   cost, desk);
#endif
   spaces[num_spaces].x = wx;
   spaces[num_spaces].y = wy;
   spaces[num_spaces].p = cost;
   spaces[num_spaces].q = desk;
   num_spaces++;
   *ns = num_spaces;
}

static void
ArrangeFindSpaces(const int *xarray, int xsize, const int *yarray, int ysize,
		  unsigned char *filled, RectInfo * spaces, int max_spaces,
		  int *ns, RectBox * fit)
{
   int                 ix, iy, fx, fy, fw, fh;

   /* create list of all "spaces" */
   *ns = 0;
   fw = fit->w;
   fh = fit->h;
   for (iy = 0; iy < ysize; iy++)
     {
	fy = yarray[iy];

	for (ix = 0; ix < xsize; ix++)
	  {
	     fx = xarray[ix];

	     ArrangeFindSpace(xarray, xsize, yarray, ysize, filled, spaces, ns,
			      fx, fy, fw, fh);
	     if (*ns >= max_spaces)
		goto done;
	  }
     }

 done:
   ;
}

static void
ArrangeSwapList(RectBox * list, int a, int b)
{
   RectBox             bb;

   bb.data = list[a].data;
   bb.x = list[a].x;
   bb.y = list[a].y;
   bb.w = list[a].w;
   bb.h = list[a].h;
   list[a].data = list[b].data;
   list[a].x = list[b].x;
   list[a].y = list[b].y;
   list[a].w = list[b].w;
   list[a].h = list[b].h;
   list[b].data = bb.data;
   list[b].x = bb.x;
   list[b].y = bb.y;
   list[b].w = bb.w;
   list[b].h = bb.h;
}

static void
ArrangeRects(const RectBox * fixed, int fixed_count, RectBox * floating,
	     int floating_count, RectBox * sorted, int startx, int starty,
	     int width, int height, int policy, char initial_window)
{
   int                 num_sorted;
   int                 tx1, ty1, tx2, ty2;
   int                 xsize = 0, ysize = 0;
   int                *xarray, *yarray;
   int                 i, j, k;
   unsigned char      *filled;
   RectInfo           *spaces;
   int                 num_spaces, alloc_spaces;
   int                 sort;
   int                 a1, a2;

   tx1 = startx;
   ty1 = starty;
   tx2 = startx + width;
   ty2 = starty + height;
   if (initial_window)
     {
	int                 xx1, yy1, xx2, yy2;

	ScreenGetAvailableAreaByPointer(&xx1, &yy1, &xx2, &yy2,
					Conf.place.ignore_struts);
	xx2 += xx1;
	yy2 += yy1;
	if (tx1 < xx1)
	   tx1 = xx1;
	if (tx2 > xx2)
	   tx2 = xx2;
	if (ty1 < yy1)
	   ty1 = yy1;
	if (ty2 > yy2)
	   ty2 = yy2;
     }
#if DEBUG_ARRANGE
   Eprintf("Target area %d,%d -> %d,%d\n", tx1, ty1, tx2, ty2);
#endif

   switch (policy)
     {
     case ARRANGE_VERBATIM:
	break;
     case ARRANGE_BY_SIZE:
	sort = 0;
	while (!sort)
	  {
	     sort = 1;
	     for (i = 0; i < floating_count - 1; i++)
	       {
		  a1 = floating[i].w * floating[i].h;
		  a2 = floating[i + 1].w * floating[i + 1].h;
		  if (a2 > a1)
		    {
		       sort = 0;
		       ArrangeSwapList(floating, i, i + 1);
		    }
	       }
	  }
	break;
     case ARRANGE_BY_POSITION:
	sort = 0;
	while (!sort)
	  {
	     sort = 1;
	     for (i = 0; i < floating_count - 1; i++)
	       {
		  a1 = floating[i].x + floating[i].y;
		  a2 = (floating[i + 1].x + (floating[i + 1].w >> 1)) +
		     (floating[i + 1].y + (floating[i + 1].h >> 1));
		  if (a2 < a1)
		    {
		       sort = 0;
		       ArrangeSwapList(floating, i, i + 1);
		    }
	       }
	  }
	break;
     default:
	break;
     }

   /* for every floating rect in order, "fit" it into the sorted list */
   i = ((fixed_count + floating_count) * 4) + 2;
   xarray = EMALLOC(int, i);
   yarray = EMALLOC(int, i);

   filled = NULL;
   spaces = NULL;
   alloc_spaces = 0;

   if (!xarray || !yarray)
      goto done;

   /* copy "fixed" rects into the sorted list */
   memcpy(sorted, fixed, fixed_count * sizeof(RectBox));
   num_sorted = fixed_count;

   /* go through each floating rect in order and "fit" it in */
   for (i = 0; i < floating_count; i++)
     {
	ArrangeMakeXYArrays(tx1, tx2, ty1, ty2, floating[i].w, floating[i].h,
			    sorted, num_sorted, xarray, &xsize, yarray, &ysize);
	num_spaces = xsize * ysize;
	if (alloc_spaces < num_spaces)
	  {
	     unsigned char      *ptr_f;
	     RectInfo           *ptr_s;

	     ptr_f = EREALLOC(unsigned char, filled, num_spaces);

	     if (ptr_f)
		filled = ptr_f;
	     ptr_s = EREALLOC(RectInfo, spaces, num_spaces);
	     if (ptr_s)
		spaces = ptr_s;
	     if (!ptr_f || !ptr_s)
		goto done;
	     alloc_spaces = num_spaces;
	  }
	ArrangeMakeFillLists(sorted, num_sorted,
			     xarray, xsize, yarray, ysize, filled);

	/* create list of all "spaces" */
	ArrangeFindSpaces(xarray, xsize, yarray, ysize, filled,
			  spaces, alloc_spaces, &num_spaces, floating + i);

	/* find the first space that fits */
	k = 0;
	sort = 0x7fffffff;	/* NB! Break at 0 == free space */
	for (j = 0; j < num_spaces; j++)
	  {
	     a1 = spaces[j].p - spaces[j].q * 4;
	     if (a1 >= sort)
		continue;
	     sort = a1;
	     k = j;
	     if (spaces[j].p == 0)
		break;
	  }
	if (spaces[k].q == 0 && Conf.place.center_if_desk_full)
	  {
	     sorted[num_sorted].x = (tx1 + tx2 - floating[i].w) / 2;
	     sorted[num_sorted].y = (ty1 + ty2 - floating[i].h) / 2;
	  }
	else
	  {
	     sorted[num_sorted].x = spaces[k].x;
	     sorted[num_sorted].y = spaces[k].y;
	  }
	sorted[num_sorted].data = floating[i].data;
	sorted[num_sorted].w = floating[i].w;
	sorted[num_sorted].h = floating[i].h;
	sorted[num_sorted].p = floating[i].p;
	num_sorted++;
     }

#if DEBUG_ARRANGE
   for (i = 0; i < num_sorted; i++)
      Eprintf("Sorted: x,y=%4d,%4d wxh=%3dx%3d p=%2d: %s\n",
	      sorted[i].x, sorted[i].y, sorted[i].w, sorted[i].h, sorted[i].p,
	      (sorted[i].data) ? EobjGetName((EObj *) sorted[i].data) : "?");
#endif

 done:
   /* free up memory */
   Efree(xarray);
   Efree(yarray);
   Efree(filled);
   Efree(spaces);
}

void
SnapEwin(EWin * ewin, int dx, int dy, int *new_dx, int *new_dy)
{
   EWin               *const *lst1;
   EWin              **lst, **gwins;
   int                 gnum, num, i, j, screen_snap_dist, odx, ody;
   static char         last_res = 0;
   int                 top_bound, bottom_bound, left_bound, right_bound, w, h;

   if (!ewin)
      return;

   if (!Conf.snap.enable)
     {
	*new_dx = dx;
	*new_dy = dy;
	return;
     }

   ScreenGetGeometry(ewin->shape_x, ewin->shape_y,
		     &left_bound, &top_bound, &w, &h);
   right_bound = left_bound + w;
   bottom_bound = top_bound + h;
   screen_snap_dist = Mode.constrained ? (w + h) : Conf.snap.screen_snap_dist;

   lst1 = EwinListOrderGet(&num);
   if (!lst1)
      return;

   lst = EMALLOC(EWin *, num);
   if (!lst)
      return;
   memcpy(lst, lst1, num * sizeof(EWin *));

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, Mode.nogroup
				      || Mode.move.swap, &gnum);
   if (gwins)
     {
	for (i = 0; i < gnum; i++)
	  {
	     for (j = 0; j < num; j++)
	       {
		  if ((lst[j] == gwins[i]) || (lst[j] == ewin))
		     lst[j] = NULL;
	       }
	  }
	Efree(gwins);
     }

   odx = dx;
   ody = dy;
   if (dx < 0)
     {
	if (IN_BELOW(ewin->shape_x + dx, left_bound, screen_snap_dist)
	    && (ewin->shape_x >= left_bound))
	  {
	     dx = left_bound - ewin->shape_x;
	  }
	else
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (!lst[i])
		     continue;

		  if ((EoGetDesk(ewin) == EoGetDesk(lst[i]) ||
		       EoIsSticky(lst[i])) && !(EoIsFloating(lst[i])) &&
		      !lst[i]->state.iconified && !lst[i]->props.ignorearrange)
		    {
		       if (IN_BELOW
			   (ewin->shape_x + dx,
			    EoGetX(lst[i]) + EoGetW(lst[i]) - 1,
			    Conf.snap.edge_snap_dist)
			   && SPANS_COMMON(ewin->shape_y, EoGetH(ewin),
					   EoGetY(lst[i]), EoGetH(lst[i]))
			   && (ewin->shape_x >=
			       (EoGetX(lst[i]) + EoGetW(lst[i]))))
			 {
			    dx =
			       (EoGetX(lst[i]) + EoGetW(lst[i])) -
			       ewin->shape_x;
			    break;
			 }
		    }
	       }
	  }
	if ((ewin->req_x - ewin->shape_x) > 0)
	   dx = 0;
     }
   else if (dx > 0)
     {
	if (IN_ABOVE
	    (ewin->shape_x + EoGetW(ewin) + dx, right_bound, screen_snap_dist)
	    && ((ewin->shape_x + EoGetW(ewin)) <= right_bound))
	  {
	     dx = right_bound - (ewin->shape_x + EoGetW(ewin));
	  }
	else
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (!lst[i])
		     continue;

		  if ((EoGetDesk(ewin) == EoGetDesk(lst[i]) ||
		       EoIsSticky(lst[i])) && !(EoIsFloating(lst[i])) &&
		      !lst[i]->state.iconified && !lst[i]->props.ignorearrange)
		    {
		       if (IN_ABOVE
			   (ewin->shape_x + EoGetW(ewin) + dx - 1,
			    EoGetX(lst[i]), Conf.snap.edge_snap_dist)
			   && SPANS_COMMON(ewin->shape_y, EoGetH(ewin),
					   EoGetY(lst[i]), EoGetH(lst[i]))
			   && ((ewin->shape_x + EoGetW(ewin)) <=
			       EoGetX(lst[i])))
			 {
			    dx =
			       EoGetX(lst[i]) - (ewin->shape_x + EoGetW(ewin));
			    break;
			 }
		    }
	       }
	  }
	if ((ewin->req_x - ewin->shape_x) < 0)
	   dx = 0;
     }

   if (dy < 0)
     {
	if (IN_BELOW(ewin->shape_y + dy, top_bound, screen_snap_dist)
	    && (ewin->shape_y >= top_bound))
	  {
	     dy = top_bound - ewin->shape_y;
	  }
	else
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (!lst[i])
		     continue;

		  if ((EoGetDesk(ewin) == EoGetDesk(lst[i]) ||
		       EoIsSticky(lst[i])) && !(EoIsFloating(lst[i])) &&
		      !lst[i]->state.iconified && !lst[i]->props.ignorearrange)
		    {
		       if (IN_BELOW
			   (ewin->shape_y + dy,
			    EoGetY(lst[i]) + EoGetH(lst[i]) - 1,
			    Conf.snap.edge_snap_dist)
			   && SPANS_COMMON(ewin->shape_x, EoGetW(ewin),
					   EoGetX(lst[i]), EoGetW(lst[i]))
			   && (ewin->shape_y >=
			       (EoGetY(lst[i]) + EoGetH(lst[i]))))
			 {
			    dy =
			       (EoGetY(lst[i]) + EoGetH(lst[i])) -
			       ewin->shape_y;
			    break;
			 }
		    }
	       }
	  }
	if ((ewin->req_y - ewin->shape_y) > 0)
	   dy = 0;
     }
   else if (dy > 0)
     {
	if (IN_ABOVE
	    (ewin->shape_y + EoGetH(ewin) + dy, bottom_bound,
	     screen_snap_dist)
	    && ((ewin->shape_y + EoGetH(ewin)) <= bottom_bound))
	  {
	     dy = bottom_bound - (ewin->shape_y + EoGetH(ewin));
	  }
	else
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (!lst[i])
		     continue;

		  if ((EoGetDesk(ewin) == EoGetDesk(lst[i]) ||
		       EoIsSticky(lst[i])) && !(EoIsFloating(lst[i])) &&
		      !lst[i]->state.iconified && !lst[i]->props.ignorearrange)
		    {
		       if (IN_ABOVE
			   (ewin->shape_y + EoGetH(ewin) + dy - 1,
			    EoGetY(lst[i]), Conf.snap.edge_snap_dist)
			   && SPANS_COMMON(ewin->shape_x, EoGetW(ewin),
					   EoGetX(lst[i]), EoGetW(lst[i]))
			   && ((ewin->shape_y + EoGetH(ewin)) <=
			       EoGetY(lst[i])))
			 {
			    dy =
			       EoGetY(lst[i]) - (ewin->shape_y + EoGetH(ewin));
			    break;
			 }
		    }
	       }
	  }
	if ((ewin->req_y - ewin->shape_y) < 0)
	   dy = 0;
     }

   Efree(lst);

   if ((odx != dx) || (ody != dy))
     {
	if (!last_res)
	  {
	     /* SoundPlay(SOUND_MOVE_RESIST); */
	     last_res = 1;
	  }
     }
   else
     {
	last_res = 0;
     }
   *new_dx = dx;
   *new_dy = dy;
}

void
ArrangeEwin(EWin * ewin)
{
   int                 x, y;

   ArrangeEwinXY(ewin, &x, &y);
   EwinMove(ewin, x, y, 0);
}

void
ArrangeEwinCentered(EWin * ewin)
{
   int                 x, y;

   ArrangeEwinCenteredXY(ewin, &x, &y);
   EwinMove(ewin, x, y, 0);
}

static void
ArrangeGetRectList(RectBox ** pfixed, int *nfixed, RectBox ** pfloating,
		   int *nfloating, EWin * ewin)
{
   RectBox            *rb, *fixed, *floating;
   int                 x, y, w, h, i, nfix, nflt, num;
   EObj               *const *lst, *eo;
   Desk               *dsk;

   fixed = floating = NULL;
   nfix = nflt = 0;

   lst = EobjListStackGet(&num);
   if (!lst)
      goto done;

   fixed = EMALLOC(RectBox, num);
   if (!fixed)
      goto done;

   dsk = (ewin) ? EoGetDesk(ewin) : DesksGetCurrent();

   for (i = 0; i < num; i++)
     {
	rb = fixed + nfix;
	eo = lst[i];

	if (!eo->shown)
	   continue;

	if (eo->type == EOBJ_TYPE_EWIN)
	  {
	     EWin               *ew = (EWin *) eo;

	     if (ew == ewin)
		continue;
	     if (eo->desk != dsk)
		continue;

	     if (ew->props.ignorearrange || EoGetLayer(ew) == 0)
		continue;

	     if (pfloating)
	       {
		  int                 ax, ay;

		  DeskGetArea(EoGetDesk(ew), &ax, &ay);

		  if (!EoIsSticky(ew) && !EoIsFloating(ew) &&
		      ew->area_x == ax && ew->area_y == ay)
		    {
		       floating = EREALLOC(RectBox, floating, nflt + 1);
		       rb = floating + nflt++;
		       rb->data = ew;
		       rb->x = EoGetX(ew);
		       rb->y = EoGetY(ew);
		       rb->w = EoGetW(ew);
		       rb->h = EoGetH(ew);
		       rb->p = EoGetLayer(ew);
#if DEBUG_ARRANGE
		       Eprintf("Add float: x,y=%4d,%4d wxh=%3dx%3d p=%2d: %s\n",
			       rb->x, rb->y, rb->w, rb->h, rb->p,
			       EobjGetName(eo));
#endif
		       continue;
		    }
	       }

	     rb->data = ew;

	     if (ew->props.never_use_area)
		rb->p = 100;
	     else
		rb->p = EoGetLayer(ew);
	  }
	else if (eo->type == EOBJ_TYPE_BUTTON)
	  {
	     if (!eo->sticky && eo->desk != dsk)
		continue;

	     rb->data = NULL;
	     rb->p = (eo->sticky) ? 1 : 0;
	  }
	else
	  {
	     continue;
	  }

	x = EobjGetX(eo);
	y = EobjGetY(eo);
	w = EobjGetW(eo);
	h = EobjGetH(eo);

	if (x < 0)
	  {
	     w += x;
	     x = 0;
	  }
	if ((x + w) > WinGetW(VROOT))
	   w = WinGetW(VROOT) - x;

	if (y < 0)
	  {
	     h += y;
	     y = 0;
	  }
	if ((y + h) > WinGetH(VROOT))
	   h = WinGetH(VROOT) - y;

	if ((w <= 0) || (h <= 0))
	   continue;

	rb->x = x;
	rb->y = y;
	rb->w = w;
	rb->h = h;
#if DEBUG_ARRANGE
	Eprintf("Add fixed: x,y=%4d,%4d wxh=%3dx%3d p=%2d: %s\n", rb->x, rb->y,
		rb->w, rb->h, rb->p, EobjGetName(eo));
#endif

	nfix++;
     }

 done:
#if DEBUG_ARRANGE
   Eprintf("Fixed: %p/%d  Floating: %p/%d\n", fixed, nfix, floating, nflt);
#endif
   *pfixed = fixed;
   *nfixed = nfix;
   if (pfloating)
      *pfloating = floating;
   if (nfloating)
      *nfloating = nflt;
}

void
ArrangeEwinXY(EWin * ewin, int *px, int *py)
{
   int                 i, num;
   RectBox            *fixed, *ret, newrect;

   fixed = NULL;
   ret = NULL;

   EwinListGetAll(&num);
   if (num <= 1)
     {
	ArrangeEwinCenteredXY(ewin, px, py);
	return;
     }

   ArrangeGetRectList(&fixed, &num, NULL, NULL, ewin);

   newrect.data = ewin;
   newrect.x = 0;
   newrect.y = 0;
   newrect.w = EoGetW(ewin);
   newrect.h = EoGetH(ewin);
   newrect.p = EoGetLayer(ewin);

   ret = ECALLOC(RectBox, num + 1);
   if (!ret)
      return;
   ArrangeRects(fixed, num, &newrect, 1, ret, 0, 0,
		WinGetW(VROOT), WinGetH(VROOT), ARRANGE_BY_SIZE, 1);

   for (i = 0; i < num + 1; i++)
     {
	if (ret[i].data == ewin)
	  {
	     *px = ret[i].x;
	     *py = ret[i].y;
	     break;
	  }
     }
   Efree(ret);
   Efree(fixed);
}

void
ArrangeEwinCenteredXY(EWin * ewin, int *px, int *py)
{
   int                 x, y, w, h;

   ScreenGetAvailableAreaByPointer(&x, &y, &w, &h, Conf.place.ignore_struts);
   *px = (w - EoGetW(ewin)) / 2 + x;
   *py = (h - EoGetH(ewin)) / 2 + y;
}

void
ArrangeEwins(const char *params)
{
   const char         *type;
   int                 method;
   int                 i, nfix, nflt, num;
   RectBox            *fixed, *ret, *floating;
   EWin               *const *lst, *ewin;

   type = params;
   method = ARRANGE_BY_SIZE;

   if (params)
     {
	if (!strcmp("order", type))
	  {
	     method = ARRANGE_VERBATIM;
	  }
	else if (!strcmp("place", type))
	  {
	     method = ARRANGE_BY_POSITION;
	  }
     }

   lst = EwinListGetAll(&num);
   if (!lst)
      goto done;

   ArrangeGetRectList(&fixed, &nfix, &floating, &nflt, NULL);

   ret = ECALLOC(RectBox, nflt + nfix);
   if (!ret)
      return;
   ArrangeRects(fixed, nfix, floating, nflt, ret, 0, 0,
		WinGetW(VROOT), WinGetH(VROOT), method, 1);

   for (i = nfix; i < nflt + nfix; i++)
     {
	if (!ret[i].data)
	   continue;

	ewin = (EWin *) ret[i].data;
	if ((EoGetX(ewin) == ret[i].x) && (EoGetY(ewin) == ret[i].y))
	   continue;

	if (Conf.place.cleanupslide)
	   EwinSlideTo(ewin, EoGetX(ewin), EoGetY(ewin),
		       ret[i].x, ret[i].y, Conf.place.slidespeedcleanup,
		       Conf.place.slidemode, 0);
	else
	   EwinMove(ewin, ret[i].x, ret[i].y, 0);
     }

   Efree(fixed);
   Efree(ret);
   Efree(floating);

 done:
   return;
}

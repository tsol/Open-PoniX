/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2007-2010 Kim Woelders
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
#include "config.h"
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#ifdef USE_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#include <X11/keysym.h>
#include "dox.h"

#define EDOX_DEFAULT_W  512
#define EDOX_DEFAULT_H  400

Display            *disp;
Root                VRoot;

static Window       win_main = None, win_text = None;
static Window       win_title = None, win_exit = None;
static Window       win_next = None, win_prev = None;

static Imlib_Image  im_title = NULL;
static Imlib_Image  im_prev1 = NULL, im_prev2 = NULL;
static Imlib_Image  im_next1 = NULL, im_next2 = NULL;
static Imlib_Image  im_exit1 = NULL, im_exit2 = NULL;

static const char   doxdir[] = ENLIGHTENMENT_ROOT "/E-docs";
char               *docdir = NULL;

static Atom         ATOM_WM_DELETE_WINDOW = None;
static Atom         ATOM_WM_PROTOCOLS = None;

static char         show_top_bar = 1;

static              Window
FindRootWindow(Display * dpy)
{
   Window              win1, win2, win3, root_win;
   char               *str;
   Atom                a, at;
   int                 format_ret;
   unsigned long       bytes_after, num_ret;
   unsigned char      *retval;

   root_win = DefaultRootWindow(dpy);

   str = getenv("ENL_WM_ROOT");
   if (!str)
      goto done;
   win1 = strtoul(str, NULL, 0);

   a = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", True);

   XGetWindowProperty(dpy, win1, a, 0, 1, False, XA_WINDOW, &at,
		      &format_ret, &num_ret, &bytes_after, &retval);
   if (!retval)
      goto done;
   win2 = *((Window *) retval);
   XFree(retval);

   XGetWindowProperty(dpy, win2, a, 0, 1, False, XA_WINDOW, &at,
		      &format_ret, &num_ret, &bytes_after, &retval);
   if (!retval)
      goto done;
   win3 = *((Window *) retval);
   XFree(retval);

   if (win2 != win3)
      goto done;

   root_win = win1;

 done:
   return root_win;
}

static void
VRootInit(void)
{
   Window              root_return;
   int                 x_return, y_return;
   unsigned int        border_width_return;

   VRoot.scr = DefaultScreen(disp);
   VRoot.win = FindRootWindow(disp);
   XGetGeometry(disp, VRoot.win, &root_return, &x_return, &y_return,
		&VRoot.w, &VRoot.h, &border_width_return, &VRoot.depth);
   VRoot.vis = DefaultVisual(disp, VRoot.scr);
   VRoot.depth = DefaultDepth(disp, VRoot.scr);
   VRoot.cmap = DefaultColormap(disp, VRoot.scr);

   imlib_set_color_usage(128);

   imlib_context_set_display(disp);
   imlib_context_set_visual(VRoot.vis);
   imlib_context_set_colormap(VRoot.cmap);
   imlib_context_set_dither(1);
   imlib_context_set_dither_mask(0);
}

static              Window
CreateWindow(Window parent, int x, int y, int ww, int hh)
{
   Window              win;
   XSetWindowAttributes attr;
   XSizeHints          hnt;

   attr.backing_store = NotUseful;
   attr.override_redirect = False;
   attr.colormap = VRoot.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = False;
   win = XCreateWindow(disp, parent, x, y, ww, hh, 0, VRoot.depth,
		       InputOutput, VRoot.vis,
		       CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		       CWColormap | CWBackPixel | CWBorderPixel, &attr);
   XSetWindowBackground(disp, win, 0);
   XStoreName(disp, win, "DOX: Enlightenment Document Viewer");
   hnt.flags = USPosition | USSize | PPosition | PSize | PMinSize | PMaxSize;
   hnt.x = x;
   hnt.y = y;
   hnt.width = ww;
   hnt.height = hh;
   hnt.min_width = ww;
   hnt.max_width = ww;
   hnt.min_height = hh;
   hnt.max_height = hh;
   XSetWMNormalHints(disp, win, &hnt);

   ATOM_WM_PROTOCOLS = XInternAtom(disp, "WM_PROTOCOLS", False);
   ATOM_WM_DELETE_WINDOW = XInternAtom(disp, "WM_DELETE_WINDOW", False);
   XSetWMProtocols(disp, win, &ATOM_WM_DELETE_WINDOW, 1);

   return win;
}

static              Imlib_Image
ImageLoad(const char *dir, const char *file)
{
   char                tmp[4096];

   Esnprintf(tmp, sizeof(tmp), "%s/%s", dir, file);
   if (!exists(tmp))
      return NULL;

   return imlib_load_image(tmp);
}

static              Imlib_Image
ImageLoadDox(const char *file)
{
   return ImageLoad(doxdir, file);
}

Imlib_Image
ImageLoadDoc(const char *file)
{
   Imlib_Image         im;

   im = ImageLoad(docdir, file);
   if (im)
      return im;

   im = ImageLoadDox(file);

   return im;
}

static void
ApplyImage1(Window win, Imlib_Image im)
{
   Pixmap              pmap = 0, mask = 0;

   imlib_context_set_image(im);
   imlib_context_set_drawable(win);
   imlib_render_pixmaps_for_whole_image(&pmap, &mask);
   XSetWindowBackgroundPixmap(disp, win, pmap);
   imlib_free_pixmap_and_mask(pmap);
}

static void
ApplyImage2(Window win, Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(win);
   imlib_render_image_on_drawable(0, 0);
}

static void
LoadFile(const char *file, const char *docfile)
{
   char                s[4096];
   FILE               *f;

   f = fopen(file, "r");
   if (!f)
     {
	strcpy(s, docdir);
	strcat(s, "/");
	strcat(s, docfile);
	f = fopen(s, "r");
	if (!f)
	  {
	     printf("Edoc_dir %s does not contain a %s file\n", docdir,
		    docfile);
	     exit(1);
	  }
     }

   GetObjects(f);
   fclose(f);
}

static void
usage(const char *prog)
{
   printf("usage:\n"
	  "%s [-page page_number] [-file Edoc_fname] [-size width height] [Edoc_dir]\n",
	  prog);
   exit(1);
}

#define ApplyImage3(win, im) \
	XClearWindow(disp, win)

#define FREE_LINKS \
ll = l; \
while (ll) \
{ \
  l = ll; \
  ll = ll->next; \
  Efree(l->name); \
  Efree(l); \
}

#define UPDATE_NOW \
{ \
XSetWindowBackgroundPixmap(disp, win_text, draw); \
XClearWindow(disp, win_text); \
}

#define UPDATE \
{ \
  int up_i, up_j; \
  int up_lut[16] = { 0, 8, 4, 12, 2, 6, 10, 14, \
                     3, 11, 1, 9, 7, 13, 5, 15}; \
  XSetWindowBackgroundPixmap(disp, win_text, draw); \
  for (up_j = 0; up_j < 16; up_j++) \
    { \
      for (up_i = 0; up_i < h; up_i += 16) \
        { \
          XClearArea(disp, win_text, 0, up_i + up_lut[up_j], w, 1, False); \
        } \
      XSync(disp, False); \
    } \
}

int
main(int argc, char **argv)
{
   int                 pagenum, prev_pagenum;
   int                 i, w, h, t, x, y;
   int                 wx, wy;
   char               *s;
   const char         *docfile;
   Pixmap              draw = 0;
   Link               *l = NULL, *ll = NULL;
   Imlib_Border        ibd;
   int                *page_hist = NULL;
   int                 page_hist_len = 1;
   int                 page_hist_pos = 0;

   w = EDOX_DEFAULT_W;
   h = EDOX_DEFAULT_H;
   pagenum = 0;

   docfile = "MAIN";
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-help"))
	   usage(argv[0]);
	else if ((!strcmp(argv[i], "-page")) && (i < (argc - 1)))
	   pagenum = atoi(argv[++i]);
	else if ((!strcmp(argv[i], "-file")) && (i < (argc - 1)))
	   docfile = argv[++i];
	else if ((!strcmp(argv[i], "-size")) && (i < (argc - 2)))
	  {
	     w = atoi(argv[++i]);
	     h = atoi(argv[++i]);
	  }
	else
	   docdir = strdup(argv[i]);
     }
   if (!docdir)
      docdir = strdup(doxdir);
   s = EMALLOC(char, strlen(docdir) + strlen(docfile) + 2 + 20);

   if (strstr(docdir, "/ABOUT"))
      show_top_bar = 0;

   /* now we'll set the locale */
   setlocale(LC_ALL, "");
   if (!XSupportsLocale())
      setlocale(LC_ALL, "C");
   XSetLocaleModifiers("");
   setlocale(LC_ALL, NULL);
   /* I dont want any internationalisation of my numeric input & output */
   setlocale(LC_NUMERIC, "C");

   sprintf(s, "%s/%s", docdir, docfile);

   LoadFile(s, docfile);

   if (!(disp = XOpenDisplay(NULL)))
     {
	printf("%s: Cannot open X display.\n", argv[0]);
	exit(0);
     }

   VRootInit();

   t = (show_top_bar) ? 16 : 0;

   wx = (VRoot.w - w) / 2;
   wy = (VRoot.h - (h + t)) / 2;
#ifdef USE_XINERAMA
   if (VRoot.win == DefaultRootWindow(disp) && XineramaIsActive(disp))
     {
	Window              rt, ch;
	int                 d;
	unsigned int        ud;
	int                 pointer_x, pointer_y;
	int                 num;
	XineramaScreenInfo *screens;

	XQueryPointer(disp, VRoot.win, &rt, &ch, &pointer_x, &pointer_y,
		      &d, &d, &ud);

	screens = XineramaQueryScreens(disp, &num);
	for (i = 0; i < num; i++)
	  {
	     if (pointer_x >= screens[i].x_org &&
		 pointer_x <= (screens[i].width + screens[i].x_org) &&
		 pointer_y >= screens[i].y_org &&
		 pointer_y <= (screens[i].height + screens[i].y_org))
	       {
		  wx = ((screens[i].width - w) / 2) + screens[i].x_org;
		  wy = ((screens[i].height - (h + t)) / 2) + screens[i].y_org;
	       }
	  }

	XFree(screens);
     }
#endif
   win_main = CreateWindow(VRoot.win, wx, wy, w, h + t);
   XSelectInput(disp, win_main, KeyPressMask | KeyReleaseMask);
   win_text = XCreateSimpleWindow(disp, win_main, 0, t, w, h, 0, 0, 0);
   XSelectInput(disp, win_text, ButtonPressMask | ButtonReleaseMask |
		PointerMotionMask);

   draw = XCreatePixmap(disp, win_text, w, h, VRoot.depth);

   l = RenderPage(draw, pagenum, w, h);
   UPDATE_NOW;

   if (show_top_bar)
     {
	win_title = XCreateSimpleWindow(disp, win_main,
					0, 0, (w - 64 - 64 - t), t, 0, 0, 0);
	win_prev = XCreateSimpleWindow(disp, win_main,
				       (w - 64 - 64 - t), 0, 64, t, 0, 0, 0);
	win_next = XCreateSimpleWindow(disp, win_main,
				       (w - 64 - 64 - t) + 64, 0, 64, t,
				       0, 0, 0);
	win_exit = XCreateSimpleWindow(disp, win_main,
				       (w - 64 - 64 - t) + 64 + 64, 0, t, t,
				       0, 0, 0);
	XSelectInput(disp, win_prev, ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, win_next, ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, win_exit, ButtonPressMask | ButtonReleaseMask);

	im_title = ImageLoadDox("title.png");
	imlib_context_set_image(im_title);
	ibd.left = 50;
	ibd.right = 2;
	ibd.top = 2;
	ibd.bottom = 2;
	imlib_image_set_border(&ibd);

	im_prev1 = ImageLoadDox("prev1.png");
	im_prev2 = ImageLoadDox("prev2.png");
	im_next1 = ImageLoadDox("next1.png");
	im_next2 = ImageLoadDox("next2.png");
	im_exit1 = ImageLoadDox("exit1.png");
	im_exit2 = ImageLoadDox("exit2.png");

	ApplyImage1(win_title, im_title);
	ApplyImage1(win_prev, im_prev1);
	ApplyImage1(win_next, im_next1);
	ApplyImage1(win_exit, im_exit1);

	XMapWindow(disp, win_title);
	XMapWindow(disp, win_prev);
	XMapWindow(disp, win_next);
	XMapWindow(disp, win_exit);
     }
   XMapWindow(disp, win_text);
   XMapWindow(disp, win_main);

   XSync(disp, False);

   page_hist = EMALLOC(int, 1);

   page_hist[0] = 0;

   for (;;)
     {
	KeySym              key;
	XEvent              ev;

	prev_pagenum = pagenum;

	XNextEvent(disp, &ev);
	switch (ev.type)
	  {
	  case KeyPress:
	     key = XLookupKeysym(&ev.xkey, 0);
	     switch (key)
	       {
	       case XK_Escape:
		  exit(0);
		  break;
	       case XK_Down:
		  goto do_next;
	       case XK_Up:
		  goto do_prev;
	       case XK_Home:
		  pagenum = 0;
		  page_hist_pos = 0;
		  goto do_page;
	       case XK_End:
		  pagenum = 99999;
		  goto do_page;
	       case XK_Left:
	       case XK_Prior:
		  pagenum--;
		  page_hist_len = page_hist_pos + 1;
		  goto do_page;
	       case XK_Right:
	       case XK_Next:
		  pagenum++;
		  page_hist_len = page_hist_pos + 1;
		  goto do_page;
	       case XK_r:
		  LoadFile(s, docfile);
		  goto do_page1;
	       }
	     break;

	   do_next:
	     if (page_hist_pos >= page_hist_len - 1)
		break;
	     page_hist_pos++;
	     pagenum = page_hist[page_hist_pos];
	     goto do_page;

	   do_prev:
	     if (pagenum == page_hist[page_hist_pos])
	       {
		  page_hist_pos--;
		  if (page_hist_pos < 0)
		     page_hist_pos = 0;
	       }
	     pagenum = page_hist[page_hist_pos];
	     goto do_page;

	   do_page_save:
	     pagenum = FixPage(pagenum);
	     if (pagenum == prev_pagenum)
		break;
	     page_hist_pos++;
	     if (page_hist_pos >= page_hist_len)
	       {
		  page_hist_len++;
		  page_hist = EREALLOC(int, page_hist, page_hist_len);
	       }
	     page_hist_len = page_hist_pos + 1;
	     page_hist[page_hist_pos] = pagenum;
	     goto do_page;

	   do_page:
	     pagenum = FixPage(pagenum);
	     if (pagenum == prev_pagenum)
		break;
	   do_page1:
	     FREE_LINKS;
	     l = RenderPage(draw, pagenum, w, h);
	     UPDATE;
	     break;

	  case ButtonPress:
	     if (ev.xbutton.window == win_prev)
		ApplyImage2(win_prev, im_prev2);
	     else if (ev.xbutton.window == win_next)
		ApplyImage2(win_next, im_next2);
	     else if (ev.xbutton.window == win_exit)
		ApplyImage2(win_exit, im_exit2);
	     else
	       {
		  x = ev.xbutton.x;
		  y = ev.xbutton.y;
		  ll = l;
		  while (ll)
		    {
		       if ((x >= ll->x) && (y >= ll->y) &&
			   (x < (ll->x + ll->w)) && (y < (ll->y + ll->h)))
			 {
			    int                 pg;

			    if (!strncmp("EXEC.", ll->name, 5))
			      {
				 if (!fork())
				   {
				      char               *exe;

				      exe = &(ll->name[5]);
				      execl("/bin/sh", "/bin/sh", "-c", exe,
					    NULL);
				      exit(0);
				   }
			      }
			    else if (!strncmp("INPUT.", ll->name, 6))
			      {
				 FILE               *p;
				 char               *exe, tmp[1024];

				 exe = &(ll->name[6]);
				 if (exe[0] != '/')
				   {
				      sprintf(tmp, "%s/%s", docdir, exe);
				      exe = tmp;
				   }
				 p = popen(exe, "r");
				 if (p)
				   {
				      int                 dirlen = 0;
				      char               *sp;

				      sp = exe;
				      while ((*sp) && (*sp != ' '))
					 sp++;
				      while ((*sp != '/') && (sp != exe))
					 sp--;
				      dirlen = sp - exe;
				      if (dirlen > 1)
					{
					   Efree(docdir);
					   docdir = EMALLOC(char, dirlen + 1);

					   memcpy(docdir, exe, dirlen);
					   docdir[dirlen] = 0;
					}
				      GetObjects(p);
				      pclose(p);
				      Efree(page_hist);
				      page_hist = EMALLOC(int, 1);

				      page_hist[0] = 0;
				      page_hist_len = 1;
				      pagenum = 0;
				      page_hist_pos = 0;
				      FREE_LINKS;
				      l = RenderPage(draw, pagenum, w, h);
				      UPDATE;
				   }
			      }
			    else
			      {
				 pg = GetPage(ll->name);
				 if (pg >= 0)
				   {
				      pagenum = pg;
				      goto do_page_save;
				   }
			      }
			    break;
			 }
		       ll = ll->next;
		    }
	       }
	     break;

	  case ButtonRelease:
	     if (ev.xbutton.window == win_prev)
	       {
		  ApplyImage3(win_prev, im_prev1);
		  goto do_prev;
	       }
	     else if (ev.xbutton.window == win_next)
	       {
		  ApplyImage3(win_next, im_next1);
		  pagenum++;
		  goto do_page_save;
	       }
	     else if (ev.xbutton.window == win_exit)
	       {
		  ApplyImage3(win_exit, im_exit1);
		  exit(0);
	       }
	     break;

	  case EnterNotify:
	  case LeaveNotify:
	     break;

	  case MotionNotify:
	     while (XCheckTypedEvent(disp, ev.type, &ev))
		;
	     {
		static Link        *pl = NULL;
		char                found = 0;

		x = ev.xmotion.x;
		y = ev.xmotion.y;
		ll = l;
		while (ll)
		  {
		     if ((x >= ll->x) && (y >= ll->y) &&
			 (x < (ll->x + ll->w)) && (y < (ll->y + ll->h)))
		       {
			  GC                  gc;
			  XGCValues           gcv;
			  int                 r, g, b;
			  XColor              xclr;

			  if (pl != ll)
			    {
			       if (pl)
				 {
				    UPDATE_NOW;
				 }
			       GetLinkColors(pagenum, &r, &g, &b);
			       ESetColor(&xclr, r, g, b);
			       EAllocColor(&xclr);
			       gc = XCreateGC(disp, win_text, 0, &gcv);
			       XSetForeground(disp, gc, xclr.pixel);
			       XDrawRectangle(disp, win_text, gc, ll->x, ll->y,
					      ll->w, ll->h);
			       XFreeGC(disp, gc);
			       pl = ll;
			    }
			  found = 1;
			  ll = NULL;
		       }
		     if (ll)
			ll = ll->next;
		  }
		if (!found)
		  {
		     UPDATE_NOW;
		     pl = NULL;
		  }
	     }
	     break;

	  case ClientMessage:
	     if (ev.xclient.message_type == ATOM_WM_PROTOCOLS &&
		 (Atom) ev.xclient.data.l[0] == ATOM_WM_DELETE_WINDOW)
		goto done;
	     break;

	  default:
	     break;
	  }
     }
 done:
   return 0;
}

void
ESetColor(XColor * pxc, int r, int g, int b)
{
   pxc->red = (r << 8) | r;
   pxc->green = (g << 8) | g;
   pxc->blue = (b << 8) | b;
}

void
EGetColor(XColor * pxc, int *pr, int *pg, int *pb)
{
   *pr = pxc->red >> 8;
   *pg = pxc->green >> 8;
   *pb = pxc->blue >> 8;
}

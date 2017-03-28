/*
 * Copyright (C) 2007-2011 Kim Woelders
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
#include "cursors.h"
#include "desktops.h"
#include "eimage.h"
#include "eglx.h"
#include "emodule.h"
#include "eobj.h"
#include "events.h"
#include "ewins.h"
#include "grabs.h"
#include "timers.h"
#include "util.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/keysym.h>

#define ENABLE_DEBUG   1
#if ENABLE_DEBUG
#define Dprintf(fmt...)  do { if(EDebug(EDBUG_TYPE_GLX))Eprintf(fmt); } while(0)
#define D2printf(fmt...) do { if(EDebug(EDBUG_TYPE_GLX)>1)Eprintf(fmt); } while(0)
#else
#define Dprintf(fmt...)
#define D2printf(fmt...)
#endif /* ENABLE_DEBUG */

static struct {
   char                mode;
} Conf_glwin;

static struct {
   char                active;
} Mode_glwin;

typedef struct {
   EObj               *eo;
   char                grabbing;
   EWin               *ewin;
} GLWindow;

static void         GlwinExit(void);

static GLWindow     GLWin;
static const char  *image = "pix/about.png";

static char         light;	/* Lighting on/off */
static GLfloat      rot_x;	/* X rotation */
static GLfloat      rot_y;	/* Y rotation */
static GLfloat      speed_x;	/* X rotation speed */
static GLfloat      speed_y;	/* Y rotation speed */
static GLfloat      bg_z;	/* Background z */
static unsigned int t0, tn;

#define N_TEXTURES 5
static unsigned int sel_bg;
static unsigned int filter;
static ETexture    *texture[N_TEXTURES];
static int          sel_ewin;

static unsigned int
GetDTime(void)
{
   return GetTimeMs() - t0;
}

static void
TexturesLoad(void)
{
   EImage             *im;

   /* Texture 0 - None */
   texture[0] = NULL;

   if (!texture[1])
     {
	im = ThemeImageLoad(image);
	if (!im)
	  {
	     Eprintf("Could not load: %s\n", image);
	  }
	else
	  {
	     /* Texture 1 - Filter: None */
	     texture[1] = EGlTextureFromImage(im, 0);

	     /* Texture 2 - Filter: Linear */
	     texture[2] = EGlTextureFromImage(im, 1);

	     /* Texture 3 - Mipmap */
	     texture[3] = EGlTextureFromImage(im, 2);

	     EImageFree(im);
	  }
     }

   if (!texture[4])
     {
	/* Texture 4 - BG pixmap */
	texture[4] =
	   EGlTextureFromDrawable(DeskGetBackgroundPixmap(DesksGetCurrent()),
				  0);
     }
}

static void
SceneResize(unsigned int width, unsigned int height)
{
   Dprintf("SceneResize\n");

   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, width, 0, height, -1000.f, 1000.f);
   glMatrixMode(GL_MODELVIEW);
}

static GLfloat      light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
static GLfloat      light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat      light_position[] = { 0.0f, 0.0f, 2.0f, 1.0f };

static void
SceneInitLight(void)
{
   glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT1, GL_POSITION, light_position);
   glEnable(GL_LIGHT1);
}

#define L 0.0f
#define R 1.0f
#define T 0.0f
#define B 1.0f

static void
DrawBackground(ETexture * et, GLfloat w, GLfloat h)
{
   if (!et)
      return;

   glBindTexture(et->target, et->texture);

   glBegin(GL_QUADS);
   glNormal3f(0.0f, 0.0f, 1.0f);
   glTexCoord2f(L, B);
   glVertex3f(0, 0, bg_z);
   glTexCoord2f(L, T);
   glVertex3f(0, h, bg_z);
   glTexCoord2f(R, T);
   glVertex3f(w, h, bg_z);
   glTexCoord2f(R, B);
   glVertex3f(w, 0, bg_z);
   glEnd();
}

static void
DrawQube(ETexture * et, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h,
	 GLfloat rx, GLfloat ry)
{
   GLfloat             w2, h2, t;

   if (!et)
      return;

   glBindTexture(et->target, et->texture);

   switch (filter)
     {
     default:
     case 0:
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	break;
     case 1:
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	break;
     }

   glPushMatrix();

#if 0
   x = round(x);
   y = round(y);
#endif

   glTranslatef(x, y, z);
#if 0
   glScalef(sz, sz, sz);
#endif
   glRotatef(rx, 1.0f, 0.0f, 0.0f);	/* Rotate around X axis */
   glRotatef(ry, 0.0f, 1.0f, 0.0f);	/* Rotate around Y axis */

   t = 4.0f;
   w2 = round(w / 2.f);
   h2 = round(h / 2.f);

   glBegin(GL_QUADS);
#if 1
   /* Front */
   glNormal3f(0.0f, 0.0f, 1.0f);
   glTexCoord2f(L, B);
   glVertex3f(-w2, -h2, t);
   glTexCoord2f(R, B);
   glVertex3f(w2, -h2, t);
   glTexCoord2f(R, T);
   glVertex3f(w2, h2, t);
   glTexCoord2f(L, T);
   glVertex3f(-w2, h2, t);
#endif
#if 1
   /* Back */
   glNormal3f(0.0f, 0.0f, 1.0f);
   glTexCoord2f(L, B);
   glVertex3f(w2, -h2, -t);
   glTexCoord2f(R, B);
   glVertex3f(-w2, -h2, -t);
   glTexCoord2f(R, T);
   glVertex3f(-w2, h2, -t);
   glTexCoord2f(L, T);
   glVertex3f(w2, h2, -t);
#endif
#if 1
   /* Right */
   glNormal3f(1.0f, 0.0f, 0.0f);
   glTexCoord2f(L, B);
   glVertex3f(w2, -h2, t);
   glTexCoord2f(R, B);
   glVertex3f(w2, -h2, -t);
   glTexCoord2f(R, T);
   glVertex3f(w2, h2, -t);
   glTexCoord2f(L, T);
   glVertex3f(w2, h2, t);
#endif
#if 1
   /* Left */
   glNormal3f(-1.0f, 0.0f, 0.0f);
   glTexCoord2f(L, B);
   glVertex3f(-w2, -h2, -t);
   glTexCoord2f(R, B);
   glVertex3f(-w2, -h2, t);
   glTexCoord2f(R, T);
   glVertex3f(-w2, h2, t);
   glTexCoord2f(L, T);
   glVertex3f(-w2, h2, -t);
#endif
#if 1
   /* Top */
   glNormal3f(0.0f, 1.0f, 0.0f);
   glTexCoord2f(L, B);
   glVertex3f(w2, h2, t);
   glTexCoord2f(R, B);
   glVertex3f(w2, h2, -t);
   glTexCoord2f(R, T);
   glVertex3f(-w2, h2, -t);
   glTexCoord2f(L, T);
   glVertex3f(-w2, h2, t);
#endif
#if 1
   /* Bottom */
   glNormal3f(0.0f, -1.0f, 0.0f);
   glTexCoord2f(L, B);
   glVertex3f(w2, -h2, -t);
   glTexCoord2f(R, B);
   glVertex3f(w2, -h2, t);
   glTexCoord2f(R, T);
   glVertex3f(-w2, -h2, t);
   glTexCoord2f(L, T);
   glVertex3f(-w2, -h2, -t);
#endif
   glEnd();

   glPopMatrix();
}

static void
SceneDraw2(unsigned int tms, EWin ** ewins, int num)
{
   double              t;
   int                 i, j, k, nx, ny;
   GLfloat             x, y, w, h, dx, dy, sz;
   EObj               *eo;

   t = 1e-3 * tms;

   w = EobjGetW(GLWin.eo);
   h = EobjGetH(GLWin.eo);
   w = (3 * w) / 4;

   DrawBackground(texture[sel_bg], w, h);

   i = (int)sqrt(w * h / (1.0 * num));
   nx = (int)((w + i - 1) / i);
   if (nx <= 0)
      nx = 1;
   ny = (num + nx - 1) / nx;
   if (ny <= 0)
      ny = 1;
#if 0
   Eprintf("wxh=%fx%f num=%d nx,ny=%d,%d\n", w, h, num, nx, ny);
#endif
   w = EobjGetW(GLWin.eo) / nx;
   h = EobjGetH(GLWin.eo) / ny;

   k = 0;
   for (j = 0; j < ny; j++)
     {
	for (i = 0; i < nx; i++)
	  {
	     if (k >= num)
		break;
	     x = i * w;
	     y = j * h;
	     eo = EoObj(ewins[k]);
	     dx = 100.0f * exp(-t);
	     dx = (fabs(dx) < 1.0) ? 0. : dx * sin(5. * t);
	     dy = 100.0f * exp(-t);
	     dy = (fabs(dy) < 1.0) ? 0. : dy * cos(5. * t);
	     sz = (k == sel_ewin) ? 0.6f : 0.5f;
	     DrawQube(EobjGetTexture(eo),
		      dx + (0.5f + i) * w, dy + (0.5f + j) * h, 500.0f,
		      sz * EobjGetW(eo), sz * EobjGetH(eo), rot_x, rot_y);
#if 1
	     if (k == sel_ewin)
	       {
		  glColor3f(1., 0., 0.);
		  glRectf(x, y, x + w, y + h);
#define X0 x
#define Y0 y
#define W w
#define H h
		  glLineWidth(2.);
		  glColor3f(0., 1., 0.);
		  glBegin(GL_LINE_LOOP);
		  glVertex3f(X0, Y0, 0);
		  glVertex3f(X0 + W, Y0, 0);
		  glVertex3f(X0 + W, Y0 + H, 0);
		  glVertex3f(X0, Y0 + H, 0);
		  glEnd();
		  glColor4f(1., 1., 1., 1.);
	       }
#endif
	     k++;
	  }
     }
}

static void
SceneDraw1(unsigned int tms, EWin ** ewins, int num)
{
   double              t1, arg;
   int                 i;
   GLfloat             w, h, dx, dy, sz;
   EObj               *eo;

   w = EobjGetW(GLWin.eo);
   h = EobjGetH(GLWin.eo);

   t1 = 2 * M_PI * (-exp(-(20e-3 * (tms - tn))) / num);

   DrawBackground(texture[sel_bg], w, h);

   for (i = 0; i < num; i++)
     {
	arg = t1 + M_PI / 2. - (i - sel_ewin) * 2. * M_PI / num;
	dx = (.5 + .3 * cos(arg)) * w;
	dy = (.5 - .3 * sin(arg)) * h;

	eo = EoObj(ewins[i]);
	if (i == sel_ewin)
	   sz = 0.5 + .01 * cos(10e-3 * (tms - tn));
	else
	   sz = 0.3;
	DrawQube(EobjGetTexture(eo), dx, dy, 500.0,
		 sz * EobjGetW(eo), sz * EobjGetH(eo), rot_x, rot_y);
     }
}

static EWin       **
GlwinEwins(int *pnum)
{
   int                 i, j, num;
   EWin               *const *ewins;
   EWin              **lst, *ewin;

   ewins = EwinListGetAll(&num);
   lst = EMALLOC(EWin *, num);

   for (i = j = 0; i < num; i++)
     {
	ewin = ewins[i];
	if (!EoIsShown(ewin))
	   continue;
	if (ewin->props.skip_focuslist || ewin->props.skip_ext_task)
	   continue;
	lst[j++] = ewin;
     }
   *pnum = j;

   return lst;
}

static void
SceneDraw(void)
{
   unsigned int        t;
   EWin              **ewins;
   int                 num;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   t = GetDTime();

   ewins = GlwinEwins(&num);
   if (sel_ewin < 0)
      sel_ewin = num - 1;
   else if (sel_ewin >= num)
      sel_ewin = 0;
   GLWin.ewin = ewins[sel_ewin];

   switch (Conf_glwin.mode)
     {
     default:
	SceneDraw1(t, ewins, num);
	break;
     case 1:
	SceneDraw2(t, ewins, num);
	break;
     }

   glXSwapBuffers(disp, EobjGetXwin(GLWin.eo));

   Efree(ewins);

   rot_x += speed_x;
   rot_y += speed_y;
}

static int
GlwinRun(void *data __UNUSED__)
{
   if (!GLWin.eo)
      return 0;
   SceneDraw();
   return 1;
}

static int
GlwinKeyPress(GLWindow * gw, KeySym key)
{
   switch (key)
     {
     case XK_q:
     case XK_Escape:
	return 1;

     case XK_b:
	sel_bg += 1;
	if (sel_bg >= N_TEXTURES)
	   sel_bg = 0;
	break;
     case XK_f:
	filter += 1;
	if (filter >= 2)
	   filter = 0;
	break;

     case XK_g:		/* Toggle grabs */
	if (gw->grabbing)
	  {
	     GrabPointerRelease();
	     GrabKeyboardRelease();
	     gw->grabbing = 0;
	  }
	else
	  {
	     GrabPointerSet(EobjGetWin(gw->eo), ECSR_GRAB, 0);
	     GrabKeyboardSet(EobjGetWin(gw->eo));
	     gw->grabbing = 1;
	  }
	break;

     case XK_l:
	light = !light;
	if (!light)
	   glDisable(GL_LIGHTING);
	else
	   glEnable(GL_LIGHTING);
	break;

     case XK_R:
	bg_z = -2.0f;
	speed_x = 0.0f;
	speed_y = 0.0f;
	break;

     case XK_Page_Up:
	bg_z -= 0.10f;
	break;
     case XK_Page_Down:
	bg_z += 0.10f;
	break;

     case XK_Up:
	speed_x -= 0.10f;
	break;
     case XK_Down:
	speed_x += 0.10f;
	break;
     case XK_Right:
	speed_y += 0.10f;
	break;
     case XK_Left:
	speed_y -= 0.10f;
	break;

     case XK_m:
	Conf_glwin.mode++;
	Conf_glwin.mode %= 2;
	break;

     case XK_n:
     case XK_Tab:
	sel_ewin++;
	tn = GetDTime();
	break;
     case XK_p:
	sel_ewin--;
	break;

     case XK_Return:
	if (!gw->ewin)
	  {
	     Eprintf("No win\n");
	     break;
	  }
	EwinOpActivate(gw->ewin, OPSRC_USER, Conf.warplist.raise_on_select);
	return 1;
     }
#define TI(no) ((texture[no]) ? (int)texture[no]->texture : -1)
   Dprintf("bg=%d(%d) filter=%d l=%d  z=%.2f  spx/y=%.2f/%.2f\n",
	   sel_bg, TI(sel_bg), filter, light, bg_z, speed_x, speed_y);

   return 0;
}

static void
GlwinEvent(Win win __UNUSED__, XEvent * ev, void *prm)
{
   GLWindow           *gw = (GLWindow *) prm;
   KeySym              key;
   int                 done = 0;

   switch (ev->type)
     {
     default:
	break;
     case EX_EVENT_DAMAGE_NOTIFY:
	return;
     }

   Dprintf("GlwinEvent ev %d\n", ev->type);

   switch (ev->type)
     {
     default:
	break;
     case KeyPress:
	key = XLookupKeysym(&ev->xkey, ev->xkey.state);
	done = GlwinKeyPress(gw, key);
	break;
#if 0
     case EnterNotify:
	GrabKeyboardSet(EobjGetWin(GLWin.eo));
	break;
     case LeaveNotify:
	GrabKeyboardRelease();
	break;
#endif
     case MapNotify:
	GlwinKeyPress(gw, XK_g);
	AnimatorAdd(GlwinRun, NULL);
	break;
#if 0
     case ConfigureNotify:
	if (ev->xconfigure.width == EobjGetW(GLWin.eo) &&
	    ev->xconfigure.height == EobjGetH(GLWin.eo))
	   break;
	SceneResize(ev->xconfigure.width, ev->xconfigure.height);
	break;
#endif
     }

   if (done)
      GlwinExit();
}

static int
GlwinCreate(const char *title __UNUSED__, int width, int height)
{
   Win                 win;
   int                 x, y;

#if 0
   win = RROOT;
#else
   win = VROOT;
#endif
   x = ((win->w - width) / 2);
   y = ((win->h - height) / 2);

   GLWin.eo = EobjWindowCreate(EOBJ_TYPE_GLX, x, y, width, height, 0, "GLwin");
   if (!GLWin.eo)
      return -1;
   win = EobjGetWin(GLWin.eo);
   GLWin.eo->fade = GLWin.eo->shadow = 1;

   EventCallbackRegister(win, GlwinEvent, &GLWin);

   ESelectInput(win, ExposureMask | KeyPressMask | ButtonPressMask |
		StructureNotifyMask);

   EGlWindowConnect(WinGetXwin(win));

   GLWin.grabbing = 0;
   GLWin.ewin = NULL;

   EobjMap(GLWin.eo, 1);

   t0 = GetTimeMs();
   tn = t0;

   return 0;
}

static void
GlwinInit(void)
{
   bg_z = -2.0f;
   sel_bg = 0;
   filter = 0;
   sel_ewin = 0;
   light = 0;

   if (GlwinCreate("GLwin", 640, 480))
     {
	Eprintf("Failed to create window\n");
	return;
     }

   Mode_glwin.active = 1;

   TexturesLoad();

   SceneResize(EobjGetW(GLWin.eo), EobjGetW(GLWin.eo));

   SceneInitLight();

   glFlush();
}

static void
GlwinExit(void)
{
   if (!Mode_glwin.active)
      return;

   Dprintf("GlTestExit\n");

   if (GLWin.eo)
     {
	EventCallbackUnregister(EobjGetWin(GLWin.eo), GlwinEvent, &GLWin);
	EobjWindowDestroy(GLWin.eo);
	GLWin.eo = NULL;
     }

#if 0
   unsigned int        i;

   for (i = 0; i < N_TEXTURES; i++)
     {
	EGlTextureDestroy(texture[i]);
	texture[i] = NULL;
     }
#endif

   Mode_glwin.active = 0;
}

/*
 * GLwin Module
 */

static void
GlwinSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_START:
	break;

     case ESIGNAL_EXIT:
	GlwinExit();
	break;
     }
}

static void
GlwinIpc(const char *params)
{
   const char         *cmd;

   cmd = params;

   if (!cmd)
     {
	GlwinInit();
     }
}

static const IpcItem GlwinIpcArray[] = {
   {
    GlwinIpc,
    "glwin", NULL,
    "Glwin functions",
    "  glwin\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(GlwinIpcArray)/sizeof(IpcItem))

static const CfgItem GlwinCfgItems[] = {
   CFG_ITEM_INT(Conf_glwin, mode, 0),
};
#define N_CFG_ITEMS (sizeof(GlwinCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
const EModule       ModGlwin = {
   "glwin", NULL,
   GlwinSighan,
   {N_IPC_FUNCS, GlwinIpcArray},
   {N_CFG_ITEMS, GlwinCfgItems}
};

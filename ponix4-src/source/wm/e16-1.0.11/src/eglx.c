/*
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
#define GLX_GLXEXT_PROTOTYPES 1
#include "E.h"
#include "eglx.h"
#include "eimage.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>

#define ENABLE_DEBUG   1
#if ENABLE_DEBUG
#define Dprintf(fmt...)  do { if(EDebug(EDBUG_TYPE_GLX))Eprintf(fmt); } while(0)
#define D2printf(fmt...) do { if(EDebug(EDBUG_TYPE_GLX)>1)Eprintf(fmt); } while(0)
#else
#define Dprintf(fmt...)
#define D2printf(fmt...)
#endif /* ENABLE_DEBUG */

#define ETEX_TYPE_IMAGE      1
#define ETEX_TYPE_PIXMAP     2

#if 1
#define TEXTURE_TARGET GL_TEXTURE_2D
#else
#define TEXTURE_TARGET GLX_TEXTURE_RECTANGLE_EXT
#endif

#ifdef HAVE_GLX_glXBindTexImageEXT

#define _glXBindTexImageEXT    glXBindTexImageEXT
#define _glXReleaseTexImageEXT glXReleaseTexImageEXT

#else

#include <dlfcn.h>

/* GL functions and helper */
typedef void        (*glXBindTexImageEXT_func) (Display * dpy,
						GLXDrawable drawable,
						int buffer,
						const int *attrib_list);
typedef void        (*glXReleaseTexImageEXT_func) (Display * dpy,
						   GLXDrawable drawable,
						   int buffer);
typedef void        (*glXFuncPtr) (void);
typedef             glXFuncPtr(*glXGetProcAddress_func) (const GLubyte *);

static glXBindTexImageEXT_func _glXBindTexImageEXT;
static glXReleaseTexImageEXT_func _glXReleaseTexImageEXT;
static glXGetProcAddress_func glx_get_proc_address;

static              glXFuncPtr
get_func_addr(const char *name)
{
   glXFuncPtr          ret = NULL;

   if (glx_get_proc_address)
      ret = glx_get_proc_address((const GLubyte *)name);
   if (!ret)
      ret = (glXFuncPtr) dlsym(RTLD_DEFAULT, name);

   return ret;
}

static int
glx_funcs_init(void)
{
   glx_get_proc_address = (glXGetProcAddress_func)
      get_func_addr("glXGetProcAddress");
   if (!glx_get_proc_address)
      glx_get_proc_address = (glXGetProcAddress_func)
	 get_func_addr("glXGetProcAddressARB");

   _glXBindTexImageEXT = (glXBindTexImageEXT_func)
      get_func_addr("glXBindTexImageEXT");

   _glXReleaseTexImageEXT = (glXReleaseTexImageEXT_func)
      get_func_addr("glXReleaseTexImageEXT");

   return !_glXBindTexImageEXT || !_glXReleaseTexImageEXT;
}

#endif /* HAVE_GLX_glXBindTexImageEXT */

static void         EobjTexturesFree(void);

typedef struct {
   XVisualInfo        *vi;
   GLXContext          ctx;
   GLXFBConfig         fbc;
   unsigned            ctx_initialised:1;
} EGlContext;

static EGlContext   egl;

#define FBCATTR(fbc, attr, want) _EGlFbcAttrib(fbc, #attr, attr, want)
static int
_EGlFbcAttrib(GLXFBConfig fbc, const char *name, int attr, int want)
{
   int                 err, value;

   value = 0xabbabeef;
   err = glXGetFBConfigAttrib(disp, fbc, attr, &value);
   if (err)
      Eprintf("  %s *** Error %d ***\n", name, err);
   else if (want > 0)
      D2printf("  %s=%#x (want %#x)\n", name, value, want);
   else
      D2printf("  %s=%#x\n", name, value);

   return value;
}

int
EGlInit(void)
{
/* From NV's README.txt (AddARGBGLXVisuals) */
   static const int    attrs[] = {
      GLX_VISUAL_CAVEAT_EXT, GLX_NONE_EXT,
      GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT | GLX_PIXMAP_BIT,
      GLX_RENDER_TYPE, GLX_RGBA_BIT,
      GLX_RED_SIZE, 1,
      GLX_GREEN_SIZE, 1,
      GLX_BLUE_SIZE, 1,
      GLX_ALPHA_SIZE, 1,
      GLX_DOUBLEBUFFER, True,
      GLX_DEPTH_SIZE, 1,
      0
   };
   XVisualInfo        *vi;
   GLXFBConfig        *fbc;
   int                 i, ix, num;
   int                 value;
   char               *s;
   XID                 vid = None;
   XRenderPictFormat  *pictFormat;

   Dprintf("EGlInit\n");

   memset(&egl, 0, sizeof(EGlContext));

   s = getenv("EVISUAL");
   if (s)
     {
	vid = strtoul(s, NULL, 0);
	Eprintf("Want Visual Id=%#lx\n", vid);
     }

   /* Create a GLX context */
   fbc = glXChooseFBConfig(disp, DefaultScreen(disp), attrs, &num);
   if (!fbc)
     {
	Eprintf("No FB configs\n");
	return -1;
     }

   D2printf("Visuals found: %d\n", num);
   ix = -1;
   for (i = 0; i < num; i++)
     {
	vi = glXGetVisualFromFBConfig(disp, fbc[i]);
	if (!vi)
	   continue;

	D2printf("Checking Visual ID=%#lx depth=%d\n", vi->visualid, vi->depth);
	if (vid && vi->visualid != vid)
	   continue;

#if 1
	value = FBCATTR(fbc[i], GLX_FBCONFIG_ID, -1);
	value = FBCATTR(fbc[i], GLX_CONFIG_CAVEAT, GLX_NONE);
	value = FBCATTR(fbc[i], GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT);
	value = FBCATTR(fbc[i], GLX_RENDER_TYPE, -1);
	value = FBCATTR(fbc[i], GLX_X_VISUAL_TYPE, -1);
	value = FBCATTR(fbc[i], GLX_X_RENDERABLE, -1);
	value = FBCATTR(fbc[i], GLX_BUFFER_SIZE, -1);
	value = FBCATTR(fbc[i], GLX_LEVEL, -1);
	value = FBCATTR(fbc[i], GLX_TRANSPARENT_TYPE, -1);
#endif

#if 1
	value = FBCATTR(fbc[i], GLX_BIND_TO_TEXTURE_RGBA_EXT, 1);
	value = FBCATTR(fbc[i], GLX_BIND_TO_TEXTURE_RGB_EXT, 1);
	if (!value)
	   continue;
	value = FBCATTR(fbc[i], GLX_BIND_TO_MIPMAP_TEXTURE_EXT, -1);
	value = FBCATTR(fbc[i], GLX_BIND_TO_TEXTURE_TARGETS_EXT,
			GLX_TEXTURE_2D_BIT_EXT);
#if 0
	if (!(value & GLX_TEXTURE_2D_BIT_EXT))
	   continue;
#endif
	value = FBCATTR(fbc[i], GLX_Y_INVERTED_EXT, -1);
#endif

#if 1
	/* We want an ARGB visual */
	pictFormat = XRenderFindVisualFormat(disp, vi->visual);
	if (!pictFormat)
	   continue;
	if (pictFormat->direct.alphaMask == 0)
	   continue;
#endif

	D2printf(" - passed\n");
	if (ix < 0)
	   ix = i;

	XFree(vi);
     }

   if (ix >= 0)
      egl.fbc = fbc[ix];
   XFree(fbc);

   if (ix < 0)
     {
	Eprintf("No FB config match\n");
	return -1;
     }

#ifndef HAVE_GLX_glXBindTexImageEXT
   if (glx_funcs_init())
     {
	Eprintf("glXBindTexImageEXT or glXReleaseTexImageEXT not available\n");
	return -1;
     }
#endif

   egl.vi = glXGetVisualFromFBConfig(disp, egl.fbc);

   egl.ctx = glXCreateNewContext(disp, egl.fbc, GLX_RGBA_TYPE, NULL, True);

   Dprintf("Direct Rendering %s\n",
	   glXIsDirect(disp, egl.ctx) ? "enabled" : "not available");
   Dprintf("Visual ID=%#lx  depth %d\n", egl.vi->visualid, egl.vi->depth);

   return 0;
}

void
EGlExit(void)
{
   Dprintf("EGlExit\n");

   EobjTexturesFree();

   if (egl.vi)
     {
	XFree(egl.vi);
	egl.vi = NULL;
     }

   if (egl.ctx)
     {
	EGlWindowDisconnect();
	glXDestroyContext(disp, egl.ctx);
	egl.ctx = NULL;
     }
}

Visual             *
EGlGetVisual(void)
{
   if (!egl.vi)
      EGlInit();
   return (egl.vi) ? egl.vi->visual : NULL;
}

unsigned int
EGlGetDepth(void)
{
   if (!egl.vi)
      EGlInit();
   return (egl.vi) ? egl.vi->depth : 0;
}

void
EGlWindowConnect(Window xwin)
{
   glXMakeContextCurrent(disp, xwin, xwin, egl.ctx);

   if (egl.ctx_initialised)
      return;

   /* First time */
   glEnable(TEXTURE_TARGET);

   glShadeModel(GL_SMOOTH);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   egl.ctx_initialised = 1;
}

void
EGlWindowDisconnect(void)
{
   if (!glXMakeContextCurrent(disp, None, None, NULL))
     {
	Eprintf("Failed to release GL context.\n");
     }
}

ETexture           *
EGlTextureFromImage(EImage * im, int mode)
{
   ETexture           *et;
   int                 w, h;
   unsigned char      *data;

   if (!im)
      return NULL;

   et = Ecalloc(1, sizeof(ETexture));
   if (!et)
      return NULL;

   et->type = ETEX_TYPE_IMAGE;
   et->target = TEXTURE_TARGET;
   glGenTextures(1, &et->texture);
   glBindTexture(et->target, et->texture);

   EImageGetSize(im, &w, &h);
   data = EImageGetData(im);

   switch (mode)
     {
     case 0:			/* No filtering */
	glTexImage2D(et->target, 0, GL_RGB8, w, h, 0, GL_BGRA,
		     GL_UNSIGNED_BYTE, data);
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	break;
     case 1:			/* Linear filtering */
	glTexImage2D(et->target, 0, GL_RGB8, w, h, 0, GL_BGRA,
		     GL_UNSIGNED_BYTE, data);
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	break;
     case 2:			/* Mipmapping */
	gluBuild2DMipmaps(et->target, GL_RGB8, w, h, GL_BGRA,
			  GL_UNSIGNED_BYTE, data);
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	break;
     }

   return et;
}

static              GLXPixmap
GetGlPixmap(Window xwin, Drawable draw)
{
   static const int    attrs[] = {
      GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT,
      GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGB_EXT,
      0
   };
   Pixmap              pixmap;
   GLXPixmap           glxpixmap;

   if (xwin == None && draw == None)
      return 0;

   pixmap = (draw) ? draw : XCompositeNameWindowPixmap(disp, xwin);
   glxpixmap = glXCreatePixmap(disp, egl.fbc, pixmap, attrs);
   Dprintf("GetGlPixmap: Window=%#lx Drawable=%#lx glxpixmap=%#lx\n",
	   xwin, draw, glxpixmap);

   return glxpixmap;
}

static void
_EGlTextureFromDrawable(ETexture * et, Drawable draw, int mode)
{
   if (!et || draw == None)
      return;

   glBindTexture(et->target, et->texture);
   et->glxpmap = GetGlPixmap(draw, (mode & 0x100) ? None : draw);
   if (et->glxpmap == None)
      return;

   _glXBindTexImageEXT(disp, et->glxpmap, GLX_FRONT_LEFT_EXT, NULL);
#if 0				/* No! */
   glXDestroyPixmap(disp, et->glxpmap);
#endif
}

ETexture           *
EGlTextureFromDrawable(Drawable draw, int mode)
{
   ETexture           *et;

   if (draw == None)
      return NULL;

   et = Ecalloc(1, sizeof(ETexture));
   if (!et)
      return NULL;

   et->type = ETEX_TYPE_PIXMAP;
   et->target = TEXTURE_TARGET;
   glGenTextures(1, &et->texture);
   glBindTexture(et->target, et->texture);

   _EGlTextureFromDrawable(et, draw, mode);

   return et;
}

void
EGlTextureDestroy(ETexture * et)
{
   if (!et)
      return;

   Dprintf("EGlTextureDestroy %d type=%u pmap=%#x\n", et->texture, et->type,
	   et->glxpmap);

   EGlTextureInvalidate(et);
   glDeleteTextures(1, &et->texture);
   Efree(et);
}

void
EGlTextureInvalidate(ETexture * et)
{
   if (!et)
      return;

   Dprintf("EGlTextureInvalidate %d type=%u pmap=%#x\n", et->texture, et->type,
	   et->glxpmap);

   switch (et->type)
     {
     case ETEX_TYPE_IMAGE:
	break;
     case ETEX_TYPE_PIXMAP:
	if (!et->glxpmap)
	   break;
	_glXReleaseTexImageEXT(disp, et->glxpmap, GLX_FRONT_LEFT_EXT);
	glXDestroyPixmap(disp, et->glxpmap);
	et->glxpmap = None;
	break;
     }
}

#include "eobj.h"

static void
EobjTexturesFree(void)
{
   int                 i, num;
   EObj               *const *eol;

   eol = EobjListStackGet(&num);
   for (i = 0; i < num; i++)
      EobjTextureDestroy(eol[i]);
}

ETexture           *
EobjGetTexture(EObj * eo)
{
   if (eo->glhook)
     {
	if (eo->glhook->glxpmap == None)
	   _EGlTextureFromDrawable(eo->glhook, EobjGetPixmap(eo), 0);
     }
   else
     {
	EobjTextureCreate(eo);
     }

   return eo->glhook;
}

void
EobjTextureCreate(EObj * eo)
{
   Pixmap              pmap;

   pmap = EobjGetPixmap(eo);
   if (pmap == None)
      return;

   eo->glhook = EGlTextureFromDrawable(pmap, 0);
}

void
EobjTextureDestroy(EObj * eo)
{
   EGlTextureDestroy(eo->glhook);
   eo->glhook = NULL;
}

void
EobjTextureInvalidate(EObj * eo)
{
   EGlTextureInvalidate(eo->glhook);
}

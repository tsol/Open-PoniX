/***************************************************************************
 * Copyright (C) 2003-2006 by XGI Technology, Taiwan.			   *
 *									   *
 * All Rights Reserved.							   *
 *									   *
 * Permission is hereby granted, free of charge, to any person obtaining   *
 * a copy of this software and associated documentation files (the	   *
 * "Software"), to deal in the Software without restriction, including	   *
 * without limitation on the rights to use, copy, modify, merge,	   *
 * publish, distribute, sublicense, and/or sell copies of the Software,	   *
 * and to permit persons to whom the Software is furnished to do so,	   *
 * subject to the following conditions:					   *
 *									   *
 * The above copyright notice and this permission notice (including the	   *
 * next paragraph) shall be included in all copies or substantial	   *
 * portions of the Software.						   *
 *									   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	   *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF	   *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND		   *
 * NON-INFRINGEMENT.  IN NO EVENT SHALL XGI AND/OR			   *
 * ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,	   *
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER	   *
 * DEALINGS IN THE SOFTWARE.						   *
 ***************************************************************************/

#ifndef _XGI_VERSION_H_
#define _XGI_VERSION_H_

#undef  XGI_NAME
#undef  XGI_DRIVER_NAME
#undef  XGI_VERSION_MAJOR
#undef  XGI_VERSION_MINOR
#undef  XGI_VERSION_PATCH
#undef  XGI_VERSION_CURRENT
#undef  XGI_VERSION_EVALUATE
#undef  XGI_VERSION_STRINGIFY
#undef  XGI_VERSION_NAME

#define XGI_NAME          "XGI"
#define XGI_DRIVER_NAME   "xgixp"

/* Jong 07/19/2006; need automation to modify revision no. */
#define XGI_VERSION_MAJOR 1
#define XGI_VERSION_MINOR 7
#define XGI_VERSION_PATCH 2

#ifndef XGI_VERSION_EXTRA
#define XGI_VERSION_EXTRA ""
#endif

#define XGI_VERSION_CURRENT \
    ((XGI_VERSION_MAJOR << 24) | \
     (XGI_VERSION_MINOR << 16) | \
     (XGI_VERSION_PATCH))

#define XGI_VERSION_EVALUATE(__x) #__x
#define XGI_VERSION_STRINGIFY(_x) XGI_VERSION_EVALUATE(_x)
#define XGI_VERSION_NAME                                         \
    XGI_VERSION_STRINGIFY(XGI_VERSION_MAJOR) "."                 \
    XGI_VERSION_STRINGIFY(XGI_VERSION_MINOR) "."                 \
    XGI_VERSION_STRINGIFY(XGI_VERSION_MINOR) XGI_VERSION_EXTRA

#endif /* _XGI_VERSION_H_ */


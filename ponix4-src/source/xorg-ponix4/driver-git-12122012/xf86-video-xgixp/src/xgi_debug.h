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

#ifndef _XGI_DEBUG_H_
#define _XGI_DEBUG_H_

#define DBG0                0x00000000
#define DBG1                0x01000000
#define DBG2                0x02000000
#define DBG3                0x04000000
#define DBG4                0x08000000
#define DBG5                0x10000000
#define DBG6                0x20000000
#define DBG7                0x40000000
#define DBG8                0x80000000

#define DBG9                0x00010000
#define DBG10               0x00020000
#define DBG11               0x00040000
#define DBG12               0x00080000
#define DBG13               0x00100000
#define DBG14               0x00200000
#define DBG15               0x00400000
#define DBG16               0x00800000


#define DBG_ERROR           DBG1
#define DBG_SHOW            DBG1
#define DBG_INFO            DBG3
#define DBG_FUNCTION        DBG9
#define DBG_WAIT_FOR_IDLE   DBG10
#define DBG_CMDLIST         DBG11
#define DBG_CMD_BUFFER      DBG12
#define DBG_SPECIAL			DBG13

#define ERRORF_DEBUG 
#ifdef ERRORF_DEBUG
#define PDEBUG(p) p
#else
#define PDEBUG(p) 
#endif

/* #define DEBUG_PRINT */

/* #define DUMP_MEMORY */

#ifdef DEBUG_PRINT
void XGIDebug(int level, const char *format, ...);
#else
#define XGIDebug(arg,...)
#endif /*DEBUG_PRINT*/

#endif /*_XGI_DEBUG_H_*/

/*
 * Copyright (C) 2006-2008 Kim Woelders
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
#ifndef _EDEBUG_H_
#define _EDEBUG_H_

#include "util.h"

#define ENABLE_DEBUG_EVENTS 1

#if ENABLE_DEBUG_EVENTS

#define EDBUG_TYPE_EWINS        128
#define EDBUG_TYPE_FOCUS        129
#define EDBUG_TYPE_COMPRESSION  130
#define EDBUG_TYPE_STACKING     131
#define EDBUG_TYPE_RAISELOWER   132
#define EDBUG_TYPE_MOVERESIZE   133
#define EDBUG_TYPE_SESSION      134
#define EDBUG_TYPE_SNAPS        135
#define EDBUG_TYPE_DESKS        136
#define EDBUG_TYPE_GRABS        137
#define EDBUG_TYPE_DISPATCH     138
#define EDBUG_TYPE_MODULES      139
#define EDBUG_TYPE_CONFIG       140
#define EDBUG_TYPE_IPC          141
#define EDBUG_TYPE_EVENTS       142
#define EDBUG_TYPE_ICONBOX      143
#define EDBUG_TYPE_VERBOSE      144
#define EDBUG_TYPE_SYNC         145
#define EDBUG_TYPE_PAGER        146
#define EDBUG_TYPE_SELECTION    147
#define EDBUG_TYPE_FONTS        148
#define EDBUG_TYPE_DBUS         149
#define EDBUG_TYPE_TIMERS       150
#define EDBUG_TYPE_IDLERS       151
#define EDBUG_TYPE_SOUND        152
#define EDBUG_TYPE_XI2          153
#define EDBUG_TYPE_ZOOM         154

#define EDBUG_TYPE_COMPMGR      161
#define EDBUG_TYPE_COMPMGR2     162
#define EDBUG_TYPE_COMPMGR3     163

#define EDBUG_TYPE_GLX          170

void                EDebugInit(const char *s);
void                EDebugSet(unsigned int type, int value);
__EXPORT__ int      EDebug(unsigned int type);

#else

#define             EDebugInit(str) do{}while(0)
#define             EDebugSet(type, value)
#define             EDebug(type) 0

#endif

#endif /* _EDEBUG_H_ */

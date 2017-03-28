/*
 * Copyright (C) 2005-2012 Kim Woelders
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
#ifndef _SNAPS_H_
#define _SNAPS_H_

#include "etypes.h"

#define SNAP_MATCH_TITLE        (1 << 0)
#define SNAP_MATCH_NAME         (1 << 1)
#define SNAP_MATCH_CLASS        (1 << 2)
#define SNAP_MATCH_ROLE         (1 << 3)
#define SNAP_MATCH_ROLE         (1 << 3)
#define SNAP_MATCH_MULTIPLE     (1 << 8)

#define SNAP_MATCH_DEFAULT      (SNAP_MATCH_NAME | SNAP_MATCH_CLASS | SNAP_MATCH_ROLE)

#define SNAP_AUTO               (1 << 0)
#define SNAP_USE_BORDER         (1 << 1)
#define SNAP_USE_COMMAND        (1 << 2)
#define SNAP_USE_DESK           (1 << 3)
#define SNAP_USE_POS            (1 << 4)
#define SNAP_USE_SIZE           (1 << 5)
#define SNAP_USE_LAYER          (1 << 6)
#define SNAP_USE_STICKY         (1 << 7)
#define SNAP_USE_SHADED         (1 << 8)
#define SNAP_USE_FLAGS          (1 << 9)
#define SNAP_USE_SKIP_LISTS     (1 << 10)
#define SNAP_USE_OPACITY        (1 << 11)
#define SNAP_USE_SHADOW         (1 << 12)
#define SNAP_USE_GROUPS         (1 << 13)

#define SNAP_USE_ALL            (~1)

void                SnapshotsSaveReal(void);
void                SnapshotsLoad(void);
void                SnapshotsSave(void);
void                SnapshotsSpawn(void);

void                SnapshotEwinApply(EWin * ewin);
void                SnapshotEwinUnmatch(EWin * ewin);
void                SnapshotEwinUpdate(const EWin * ewin, unsigned int flags);
void                SnapshotEwinParse(EWin * ewin, const char *params);

extern const char   SnapshotsIpcText[];
void                SnapshotsIpcFunc(const char *params);

#endif /* _SNAPS_H_ */

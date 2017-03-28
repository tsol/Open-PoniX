/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2007 Kim Woelders
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
#ifndef _FILE_H_
#define _FILE_H_

#include <time.h>

void                E_md(const char *s);
void                E_mv(const char *s, const char *ss);
void                E_rm(const char *s);
char              **E_ls(const char *dir, int *num);

#define EFILE_ANY   0x01
#define EFILE_REG   0x02
#define EFILE_DIR   0x04

#define EPERM_R     0x10
#define EPERM_W     0x20
#define EPERM_X     0x40
#define EPERM_RWX   (EPERM_R | EPERM_W | EPERM_X)

int                 file_test(const char *s, unsigned int test);

#define exists(s)   file_test(s, EFILE_ANY)
#define isdir(s)    file_test(s, EFILE_DIR)
#define isfile(s)   file_test(s, EFILE_REG)
#define canread(s)  file_test(s, EFILE_REG | EPERM_R)
#define canexec(s)  file_test(s, EFILE_REG | EPERM_X)

time_t              moddate(const char *s);
int                 fileinode(const char *s);
int                 filedev_map(int dev);
int                 filedev(const char *s);

int                 isabspath(const char *s);
const char         *fileext(const char *s);
char               *fileof(const char *s);
const char         *fullfileof(const char *s);

char               *path_test(const char *file, unsigned int test);
int                 path_canexec(const char *file);

#endif /* _FILE_H_ */

/*
 * Copyright © 2012 Rob Clark <robclark@freedesktop.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MSM_ACCEL_H_
#define MSM_ACCEL_H_

#include "msm.h"
#include "msm-accel-z1xx.h"

#define ENABLE_EXA_TRACE                  0
#define ENABLE_SW_FALLBACK_REPORTS        0

#define TRACE_EXA(fmt, ...) do {                                    \
        if (ENABLE_EXA_TRACE)                                       \
            ErrorF("EXA: " fmt"\n", ##__VA_ARGS__);                 \
    } while (0)

#define EXA_FAIL_IF(cond) do {                                      \
        if (cond) {                                                 \
            if (ENABLE_SW_FALLBACK_REPORTS) {                       \
                ErrorF("FALLBACK: " #cond"\n");                     \
            }                                                       \
            return FALSE;                                           \
        }                                                           \
    } while (0)

#endif /* MSM_ACCEL_H_ */

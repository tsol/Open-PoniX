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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xgi.h"
#include "xgi_debug.h"

#ifdef DEBUG_PRINT
int Log2Screen = 0;
int Log2File = 1;
static const char dbglog[] = "/var/log/xgi.log";

unsigned DebugLevel = DBG_ERROR|DBG_CMDLIST;/*|DBG_CMD_BUFFER;*/

void XGIDebug(int level, const char *format, ...)
{
    va_list args;

    if ((level & DebugLevel) != 0) {
        va_start(args, format);

        /* RATS: We assume the format string is trusted, since it is always
         * from a log message in our code.
         */
        if (Log2Screen) {
            vfprintf(stderr, format, args);
        }

        if (Log2File) {
            FILE *fp = fopen(dbglog, "at");

            if (fp != NULL) {
                vfprintf(fp, format, args);
                fflush(fp);
                fclose(fp);
            }
        }

        va_end(args);
    }
}
#endif /*DEBUG_PRINT*/

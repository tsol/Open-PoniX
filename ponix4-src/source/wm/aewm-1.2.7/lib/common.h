/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#ifndef AEWM_COMMON_H
#define AEWM_COMMON_H

#include <stdio.h>
#include <string.h>

#define BUF_SIZE 2048
#define ARG(longname, shortname, nargs) \
    ((strcmp(argv[i], "--" longname) == 0 \
    || strcmp(argv[i], "-" shortname) == 0) \
    && i + (nargs) < argc)

#endif /* AEWM_COMMON_H */

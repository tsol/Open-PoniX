/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include "parser.h"
#include <string.h>
#include <ctype.h>

/* Our crappy parser. A token is either a whitespace-delimited word,
 * or a bunch of words in double quotes (backslashes are permitted in
 * either case). src points to somewhere in a buffer -- the caller
 * must save the location of this buffer, because we update src to
 * point past all the tokens found so far. If we find a token, we
 * write it into dest (caller is responsible for allocating storage)
 * and return 1. Otherwise return 0. */

int get_token(char **src, char *dest)
{
    int quoted, nchars = 0;

    while (**src && isspace(**src)) (*src)++;

    if (**src == '"') {
        quoted = 1;
        (*src)++;
    } else {
        quoted = 0;
    }

    while (**src) {
        if (quoted) {
            if (**src == '"') {
                (*src)++;
                break;
            }
        } else {
            if (isspace(**src)) break;
        }
        if (**src == '\\') (*src)++;
        *dest++ = *(*src)++;
        nchars++;
    }

    *dest = '\0';
    return nchars || quoted;
}

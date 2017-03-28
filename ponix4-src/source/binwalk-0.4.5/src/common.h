/* 
 * Copyright (c) 2010 Craig Heffner
 *
 * This software is provided under the MIT license. For the full text of this license, please see
 * the COPYING file included with this code, or visit http://www.opensource.org/licenses/MIT.
 */

#ifndef COMMON_H
#define COMMON_H

#define INT_MAX_STR_SIZE        	1024
#define MAX_TIMESTAMP_SIZE      	1024
#define TIMESTAMP_FORMAT        	"%b %d, %Y @ %T"
#define STRING_ENTRY_FORMAT		"0\tstring\t%s\t\"%s\""
#define TMP_MAGIC_FILE			"/tmp/binwalk.magic.raw.search"

/* These globals are set once, read many */
struct bin_globals
{
        FILE *fsout;
        int quiet;
} globals;

int str2int(char *str);
const void *file_read(char *file, size_t *fsize);
void print(const char* format, ...);
char *timestamp();
char *create_magic_file(char *search_string);
void cleanup_magic_file(void);
void uppercase(char *string);
int string_contains(char *haystack, char *needle);

#endif

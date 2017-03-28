/* 
 * Copyright (c) 2010 Craig Heffner
 *
 * This software is provided under the MIT license. For the full text of this license, please see
 * the COPYING file included with this code, or visit http://www.opensource.org/licenses/MIT.
 */

#ifndef FILTER_H
#define FILTER_H

#define MAX_FILTERS 	8192
#define MAX_SIGNATURES	8192

enum filter_type_t
{
	FILTER_EXCLUDE,
	FILTER_INCLUDE,
	FILTER_ADD,
	FILTER_GREP
};

enum filter_result_t
{
	RESULT_NONE,
	RESULT_NOT_FOUND,
	RESULT_EXCLUDE,
	RESULT_INCLUDE
};

struct magic_filter
{
        enum filter_type_t type;
        char *filter;
};

struct magic_signature
{
        int offset;
        int size;
        int wildcard;
	int matched;
        char *signature;
};

void free_filters(struct magic_filter *filters[], int filter_count);
enum filter_result_t grep_check(struct magic_filter *filters[], int filter_count, char *description);
enum filter_result_t filter_check(struct magic_filter *filters[], int filter_count, char *description);
void add_filter(struct magic_filter *filters[], int *filter_count, enum filter_type_t type, char* filter);

void free_signatures(struct magic_signature **signatures, int num_sigs);
void add_signature(struct magic_signature **signatures, int *num_sigs, int magic_size, int magic_offset, int wildcard, char *magic);

#endif

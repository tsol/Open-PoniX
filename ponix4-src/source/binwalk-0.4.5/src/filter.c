/* 
 * Copyright (c) 2010 Craig Heffner
 *
 * This software is provided under the MIT license. For the full text of this license, please see
 * the COPYING file included with this code, or visit http://www.opensource.org/licenses/MIT.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "filter.h"

static int grep_filter_count = 0;
static enum filter_result_t default_filter_check_response = RESULT_NOT_FOUND;

/* Frees allocated magic_filter entries */
void free_filters(struct magic_filter *filters[], int filter_count)
{
	int i = 0;

	if(filters)
	{
		for(i=0; i<filter_count; i++)
		{
			if(filters[i])
			{
				if(filters[i]->filter)
				{
					free(filters[i]->filter);
				}
				
				free(filters[i]);
			}
		}
	}
}

/* Appends a new filter to the filters array */
void add_filter(struct magic_filter *filters[], int *filter_count, enum filter_type_t type, char *filter)
{
	/* If an explicit include filter was specified, then default to RESULT_EXCLUDE in filter_check unless a match is found */
	if(type == FILTER_INCLUDE)
	{
		default_filter_check_response = RESULT_EXCLUDE;
	}
	else if(type == FILTER_GREP)
	{
		grep_filter_count++;
	}

        if(*filter_count < MAX_FILTERS)
        {
                filters[*filter_count] = malloc(sizeof(struct magic_filter));
                if(filters[*filter_count] != NULL)
                {
                        memset(filters[*filter_count], 0, sizeof(struct magic_filter));

                        filters[*filter_count]->type = type;
                        filters[*filter_count]->filter = strdup(filter);
			uppercase(filters[*filter_count]->filter);
                        *filter_count = *filter_count+1;
                }
                else
                {
                        perror("Malloc failure");
                }
        }

        return;
}

/* Do a case-insensitive search of the specified description for all FILTER_GREP filters */
enum filter_result_t grep_check(struct magic_filter *filters[], int filter_count, char *description)
{
	int i = 0;
	enum filter_result_t found = RESULT_INCLUDE;

	if(grep_filter_count > 0)
	{
		/* If grep filters were specified, default to excluding this entry unless it matches a filter */
		found = RESULT_EXCLUDE;
		
		for(i=0; i<filter_count; i++)
		{
			if(filters[i]->type == FILTER_GREP)
			{
				if(string_contains(description, filters[i]->filter))
				{
					found = RESULT_INCLUDE;
					break;
				}
			}
		}
	}

	return found;
}

/* Check to see if the description matches any of the include / exclude filters */
enum filter_result_t filter_check(struct magic_filter *filters[], int filter_count, char *description)
{
	int i = 0; 
	enum filter_result_t found = default_filter_check_response;

	/* If include filters have been specified, then ONLY those descriptions that match the include filters
	 * should be used. Loop through to see if there are any include filters; if so, make sure this desciption
	 * matches one of the include filters.
	 */
	for(i=0; i<filter_count; i++)
	{
		if(filters[i]->type == FILTER_INCLUDE || filters[i]->type == FILTER_ADD)
		{
			if(string_contains(description, filters[i]->filter))
			{
				found = RESULT_INCLUDE;
				break;
			}
		}
	}
	
	/* Check to see if description matches any exclude filters */
	for(i=0; i<filter_count; i++)
	{
		if(filters[i]->type == FILTER_EXCLUDE)
		{
			if(string_contains(description, filters[i]->filter))
			{
				found = RESULT_EXCLUDE;
				break;
			}
		}
	}

	return found;
}

/* Free an allocated struct magic_signature array */
void free_signatures(struct magic_signature **signatures, int num_sigs)
{
	int i = 0;

	if(signatures)
	{
		for(i=0; i<num_sigs; i++)
		{
			if(signatures[i])
			{
				if(signatures[i]->signature)
				{
					free(signatures[i]->signature);
				}

				free(signatures[i]);
			}
		}
	}
}

/* Add a signature to the passed magic_signature array */
void add_signature(struct magic_signature **signatures, int *num_sigs, int magic_size, int magic_offset, int wildcard, char *magic)
{
	if(*num_sigs < MAX_SIGNATURES)
	{
		signatures[*num_sigs] = malloc(sizeof(struct magic_signature));
		signatures[*num_sigs]->size = magic_size;
		signatures[*num_sigs]->offset = magic_offset;
		signatures[*num_sigs]->wildcard = wildcard;
		signatures[*num_sigs]->matched = 0;
		signatures[*num_sigs]->signature = malloc(magic_size);
		memcpy(signatures[*num_sigs]->signature, magic, magic_size);
		*num_sigs = *num_sigs + 1;
	}
}

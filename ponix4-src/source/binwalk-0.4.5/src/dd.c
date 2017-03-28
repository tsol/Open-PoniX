#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "dd.h"

int dd_rule_count = 0;
struct dd_rule **dd_rules = NULL;

/* Add a DD rule to extract matching entries from the target file */
void add_dd_rule(char *match, char *extension, int max)
{
	void *tmp = NULL;

	tmp = dd_rules;
	dd_rules = realloc(dd_rules, ((dd_rule_count+1) * sizeof(struct dd_rule *)));
	if(!dd_rules)
	{
		perror("realloc");
		if(tmp) free(tmp);
	}
	else
	{
		dd_rules[dd_rule_count] = malloc(sizeof(struct dd_rule));
		if(dd_rules[dd_rule_count])
		{
			memset(dd_rules[dd_rule_count], 0, sizeof(struct dd_rule));
			
			dd_rules[dd_rule_count]->match = strdup(match);
			dd_rules[dd_rule_count]->ext = strdup(extension);
			dd_rules[dd_rule_count]->max = max;
			dd_rules[dd_rule_count]->count = 0;

			dd_rule_count++;
		}
	}
}

/* Frees all DD rule entries */
void free_dd_rules(void)
{
	int i = 0;

	if(dd_rules)
	{
		for(i=0; i<dd_rule_count; i++)
		{
			if(dd_rules[i])
			{
				if(dd_rules[i]->match) free(dd_rules[i]->match);
				if(dd_rules[i]->ext) free(dd_rules[i]->ext);
				
				free(dd_rules[i]);
			}
		}

		free(dd_rules);
	}

	dd_rule_count = 0;
}

/* Checks to see if a description matches any DD rules */
int matches_dd_rule(char *description)
{
	int i = 0, retval = -1;

	for(i=0; i<dd_rule_count; i++)
	{
		if((strcmp(dd_rules[i]->match, DD_WILDCARD) == 0) || 
		   (string_contains(description, dd_rules[i]->match)))
		{
			if((dd_rules[i]->max == 0) ||
			   (dd_rules[i]->max > dd_rules[i]->count))
			{
				retval = i;
				dd_rules[i]->count++;
				break;
			}
		}
	}

	return retval;
}

/* Save data to a file using the DD rule extension and offset value as the file name */
void dd(int index, uint32_t offset, char *data, size_t size)
{
	FILE *fp = NULL;
	char file[FILENAME_MAX] = { 0 };

	if(index < dd_rule_count)
	{
		snprintf((char *) &file, FILENAME_MAX, "%X.%s", offset, dd_rules[index]->ext);

		fp = fopen((char *) &file, "wb");
		if(fp)
		{
			if(fwrite(data, 1, size, fp) != size)
			{
				perror("fwrite");
			}
	
			fclose(fp);
		}
		else
		{
			perror("fopen");
		}
	}
}

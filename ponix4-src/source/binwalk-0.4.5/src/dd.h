#ifndef __DD_H__
#define __DD_H__

#include <stdint.h>

#define DD_WILDCARD "all"

struct dd_rule
{
	char *match;
	char *ext;
	int count;
	int max;
};

void add_dd_rule(char *match, char *extension, int max);
void free_dd_rules(void);
int matches_dd_rule(char *description);
void dd(int index, uint32_t offset, char *data, size_t size);

#endif

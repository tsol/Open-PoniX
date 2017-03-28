#ifndef REG_LIB_H
#define REG_LIB_H

#include <stdlib.h>
#include <linux/types.h>

#include "regdb.h"

/* Common regulatory structures, functions and helpers */

/* This matches the kernel's data structures */
struct ieee80211_freq_range {
	__u32 start_freq_khz;
	__u32 end_freq_khz;
	__u32 max_bandwidth_khz;
};

struct ieee80211_power_rule {
	__u32 max_antenna_gain;
	__u32 max_eirp;
};

struct ieee80211_reg_rule {
	struct ieee80211_freq_range freq_range;
	struct ieee80211_power_rule power_rule;
	__u32 flags;
};

struct ieee80211_regdomain {
	__u32 n_reg_rules;
	char alpha2[2];
	struct ieee80211_reg_rule reg_rules[];
};

static inline int is_world_regdom(const char *alpha2)
{
	if (alpha2[0] == '0' && alpha2[1] == '0')
		return 1;
	return 0;
}

static inline int isalpha_upper(char letter)
{
	if (letter >= 'A' && letter <= 'Z')
		return 1;
	return 0;
}

static inline int is_alpha2(const char *alpha2)
{
	if (isalpha_upper(alpha2[0]) && isalpha_upper(alpha2[1]))
		return 1;
	return 0;
}

/* Avoid stdlib */
static inline int is_len_2(const char *alpha2)
{
        if (alpha2[0] == '\0' || (alpha2[1] == '\0'))
                return 0;
        if (alpha2[2] == '\0')
                return 1;
        return 0;
}

static inline int is_valid_regdom(const char *alpha2)
{
	if (!is_len_2(alpha2))
		return 0;

	if (!is_alpha2(alpha2) && !is_world_regdom(alpha2))
		return 0;

	return 1;
}

static inline __u32 max(__u32 a, __u32 b)
{
	return (a > b) ? a : b;
}

static inline __u32 min(__u32 a, __u32 b)
{
	return (a > b) ? b : a;
}

void *crda_get_file_ptr(__u8 *db, int dblen, int structlen, __be32 ptr);
int crda_verify_db_signature(__u8 *db, int dblen, int siglen);

/* File reg db entry -> rd converstion utilities */
struct ieee80211_regdomain *country2rd(__u8 *db, int dblen,
	struct regdb_file_reg_country *country);

/* reg helpers */
void print_regdom(struct ieee80211_regdomain *rd);

#endif

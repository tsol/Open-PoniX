#ifndef REG_DB_H
#define REG_DB_H

#include <linux/types.h>

/*
 * WARNING: This file needs to be kept in sync with
 *  - the parser (dbparse.py)
 *  - the generator code (db2bin.py)
 */

/* spells "RGDB" */
#define REGDB_MAGIC	0x52474442

/*
 * Only supported version now, start at arbitrary number
 * to have some more magic. We still consider this to be
 * "Version 1" of the file.
 */
#define REGDB_VERSION	19

/*
 * The signature at the end of the file is an RSA-signed
 * SHA-1 hash of the file.
 */

/* db file starts with a struct regdb_file_header */

struct regdb_file_header {
	/* must be REGDB_MAGIC */
	__be32	magic;
	/* must be REGDB_VERSION */
	__be32	version;
	/*
	 * Pointer (offset) into file where country list starts
	 * and number of countries. The country list is sorted
	 * alphabetically to allow binary searching (should it
	 * become really huge). Each country is described by a
	 * struct regdb_file_reg_country.
	 */
	__be32	reg_country_ptr;
	__be32	reg_country_num;
	/* length (in bytes) of the signature at the end of the file */
	__be32	signature_length;
};

struct regdb_file_freq_range {
	__be32	start_freq,	/* in kHz */
		end_freq,	/* in kHz */
		max_bandwidth;	/* in kHz */
};

/*
 * Values of zero mean "not applicable", i.e. the regulatory
 * does not limit a certain value.
 */
struct regdb_file_power_rule {
	/* antenna gain is in mBi (100 * dBi) */
	__be32	max_antenna_gain;
	/* this is in mBm (100 * dBm) */
	__be32	max_eirp;
};

/* must match <linux/nl80211.h> enum nl80211_reg_rule_flags */

enum reg_rule_flags {
	RRF_NO_OFDM		= 1<<0, /* OFDM modulation not allowed */
	RRF_NO_CCK		= 1<<1, /* CCK modulation not allowed */
	RRF_NO_INDOOR		= 1<<2, /* indoor operation not allowed */
	RRF_NO_OUTDOOR		= 1<<3, /* outdoor operation not allowed */
	RRF_DFS			= 1<<4, /* DFS support is required to be
					 * used */
	RRF_PTP_ONLY		= 1<<5, /* this is only for Point To Point
					 * links */
	RRF_PTMP_ONLY		= 1<<6, /* this is only for Point To Multi
					 * Point links */
	RRF_PASSIVE_SCAN	= 1<<7, /* passive scan is required */
	RRF_NO_IBSS		= 1<<8, /* IBSS is not allowed */
};

struct regdb_file_reg_rule {
	/* pointers (offsets) into the file */
	__be32	freq_range_ptr; /* pointer to a struct regdb_file_freq_range */
	__be32	power_rule_ptr; /* pointer to a struct regdb_file_power_rule */
	/* rule flags using enum reg_rule_flags */
	__be32 flags;
};

struct regdb_file_reg_rules_collection {
	__be32	reg_rule_num;
	/* pointers (offsets) into the file. There are reg_rule_num elements
	 * in the reg_rule_ptrs array pointing to struct
	 * regdb_file_reg_rule */
	__be32	reg_rule_ptrs[];
};

struct regdb_file_reg_country {
	__u8	alpha2[2];
	__u8	PAD[2];
	/* pointer (offset) into the file to a struct
	 * regdb_file_reg_rules_collection */
	__be32	reg_collection_ptr;
};


/*
 * Verify that no unexpected padding is added to structures
 * for some reason.
 */

#define ERROR_ON(cond) \
	((void)sizeof(char[1 - 2*!!(cond)]))

#define CHECK_STRUCT(name, size) \
	ERROR_ON(sizeof(struct name) != size)

static inline void check_db_binary_structs(void)
{
	CHECK_STRUCT(regdb_file_header, 20);
	CHECK_STRUCT(regdb_file_freq_range, 12);
	CHECK_STRUCT(regdb_file_power_rule, 8);
	CHECK_STRUCT(regdb_file_reg_rule, 12);
	CHECK_STRUCT(regdb_file_reg_rules_collection, 4);
	CHECK_STRUCT(regdb_file_reg_country, 8);
}

#endif

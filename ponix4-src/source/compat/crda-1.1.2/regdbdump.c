#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h> /* ntohl */

#include "regdb.h"
#include "reglib.h"

int main(int argc, char **argv)
{
	int fd;
	struct stat stat;
	__u8 *db;
	struct regdb_file_header *header;
	struct regdb_file_reg_country *countries;
	int dblen, siglen, num_countries, i, r = 0;
	struct ieee80211_regdomain *rd = NULL;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 2;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("failed to open db file");
		return 2;
	}

	if (fstat(fd, &stat)) {
		perror("failed to fstat db file");
		return 2;
	}

	dblen = stat.st_size;

	db = mmap(NULL, dblen, PROT_READ, MAP_PRIVATE, fd, 0);
	if (db == MAP_FAILED) {
		perror("failed to mmap db file");
		return 2;
	}

	header = crda_get_file_ptr(db, dblen, sizeof(*header), 0);

	if (ntohl(header->magic) != REGDB_MAGIC) {
		fprintf(stderr, "Invalid database magic\n");
		return 2;
	}

	if (ntohl(header->version) != REGDB_VERSION) {
		fprintf(stderr, "Invalid database version\n");
		return 2;
	}

	siglen = ntohl(header->signature_length);
	/* adjust dblen so later sanity checks don't run into the signature */
	dblen -= siglen;

	if (dblen <= (int)sizeof(*header)) {
		fprintf(stderr, "Invalid signature length %d\n", siglen);
		return 2;
	}

	/* verify signature */
	if (!crda_verify_db_signature(db, dblen, siglen))
		return -EINVAL;

	num_countries = ntohl(header->reg_country_num);
	countries = crda_get_file_ptr(db, dblen,
			sizeof(struct regdb_file_reg_country) * num_countries,
			header->reg_country_ptr);

	for (i = 0; i < num_countries; i++) {
		struct regdb_file_reg_country *country = countries + i;

		rd = country2rd(db, dblen, country);
		if (!rd) {
			r = -ENOMEM;
			fprintf(stderr, "Could not covert country "
			"(%.2s) to rd\n", country->alpha2);
			goto out;
		}

		print_regdom(rd);
		free(rd);
		rd = NULL;

	}
out:
	return r;
}

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * Dumps firmware trace.
 *
 * Uses binary representation of 'strings' file,
 * it should be named fw_strings.bin and be in the current directory
 * Periodically reads peripheral memory like in blob_fw_peri on the debugfs
 * Name of peripheral memory file passed as parameter
 */

typedef uint32_t u32;
typedef unsigned int uint;

struct module_level_enable {	/* Little Endian */
      uint	error_level_enable:1;
      uint	warn_level_enable:1;
      uint	info_level_enable:1;
      uint	verbose_level_enable:1;
      uint	reserved0:4;
} __attribute__((packed));

struct log_trace_header {	/* Little Endian */
	uint	strring_offset:20;          /* the offset of the trace string in the strings sections */
	uint	module:4;                   /* module that outputs the trace */
	uint	level:2;                    /*	0 - Error
						1- WARN
						2 - INFO
						3 - VERBOSE */
	uint	parameters_num:2;           /* [0..3] */
	uint	is_string:1;				/* this bit was timestamp_present:1; and changed to indicate if the printf uses %s */
	uint	signature:3;                /* should be 5 (2'101) in valid header */
} __attribute__((packed));

union log_event {
	struct log_trace_header hdr;
	u32 param;
} __attribute__((packed));

struct log_table_header {
	u32 write_ptr;                      /* incremented by trace producer every write */
	struct module_level_enable module_level_enable[16];
	union log_event evt[0];
} __attribute__((packed));

static size_t read_all(int f, void *buf, size_t n)
{
	size_t actual = 0, r;
	do {
		r = read(f, buf + actual, n - actual);
		actual += r;
	} while ((r > 0) && (actual < n));
	return actual;
}

static void *read_file(const char *name, size_t *size)
{
	int f = open(name, O_RDONLY);
	size_t sz = *size;
	size_t r;
	void *buf;
	if (f < 0)
		return NULL;

	if (!sz) {
		sz = lseek(f, 0, SEEK_END);
		lseek(f, 0, SEEK_SET);
	}
	buf = malloc(sz);
	if (!buf) {
		close(f);
		return NULL;
	}
	r = read_all(f, buf, sz);
	close(f);
	if (r != sz) {
		printf("Error: from %s read %zd bytes out of %zd\n",
				name, r, sz);
		free(buf);
		return NULL;
	}
	*size = sz;
	return buf;
}

static char *strings_bin = "fw_strings.bin";
static char *peri = "peri.dump";
enum {
	log_buf_sizedw = 0x1000/4,
	str_mask = 0xFFFF,
};

static void *peri_buf;
static void *str_buf;
static size_t str_sz;
static u32 rptr = 0;
static const char* levels[] = {
	"E",
	"W",
	"I",
	"V",
};

static const char *modules[16];

static void do_parse(void)
{
	struct log_table_header *h = peri_buf;
	u32 wptr = h->write_ptr;
	if ((wptr - rptr) >= log_buf_sizedw) {
		/* overflow; try to parse last wrap */
		rptr = wptr - log_buf_sizedw;
	}
	for (;(long)(wptr - rptr) > 0; rptr++) {
		int i;
		u32 p[3] = {0};
		union log_event *evt = &h->evt[rptr % log_buf_sizedw];
		const char *fmt;
		if (evt->hdr.signature != 5)
			continue;
		if (evt->hdr.strring_offset > str_sz)
			continue;
		if (evt->hdr.parameters_num > 3)
			continue;
		fmt = str_buf + evt->hdr.strring_offset;
		for (i = 0; i < evt->hdr.parameters_num; i++) {
			p[i] = h->evt[(rptr + i + 1) % log_buf_sizedw].param;
		}
		printf("[%6d] %9s %s :", rptr, modules[evt->hdr.module],
				levels[evt->hdr.level]);
		if (evt->hdr.is_string) {
			printf(fmt, str_buf + (p[0] & str_mask),
					str_buf + (p[1] & str_mask),
					str_buf + (p[2] & str_mask));
		} else {
			printf(fmt, p[0], p[1], p[2]);
		}
		printf("\n");
		rptr += evt->hdr.parameters_num;
	}
	fflush(stdout);
}

int main(int argc, char* argv[])
{
	if (argc > 1)
		peri = argv[1];
	const char *mod;
	size_t peri_sz = 8*1024;
	int i;
	str_buf = read_file(strings_bin, &str_sz);
	mod = str_buf;
	peri_buf = read_file(peri, &peri_sz);
	if (!str_buf || !peri_buf)
		return -1;
	struct log_table_header *h = peri_buf;
	u32 wptr = h->write_ptr;
	if ((wptr - rptr) >= log_buf_sizedw) {
		/* overflow; try to parse last wrap */
		rptr = wptr - log_buf_sizedw;
	}
	printf("  wptr = %d rptr = %d\n", wptr, rptr);
	for (i = 0; i < 16; i++) {
		modules[i] = mod;
		struct module_level_enable *m = &h->module_level_enable[i];
		printf("  %s[%2d] : %s%s%s%s\n", modules[i], i,
				m->error_level_enable   ? "E" : " ",
				m->warn_level_enable    ? "W" : " ",
				m->info_level_enable    ? "I" : " ",
				m->verbose_level_enable ? "V" : " ");
		mod = strchr(mod, '\0') + 1;
	}
	for(;;) {
		do_parse();
		sleep(1);
		int f = open(peri, O_RDONLY);
		size_t r = read_all(f, peri_buf, peri_sz);
		close(f);
		if (r != peri_sz)
			break;
	}
	return 0;
}

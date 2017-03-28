/* 
 * Copyright (c) 2010 Craig Heffner
 *
 * This software is provided under the MIT license. For the full text of this license, please see
 * the COPYING file included with this code, or visit http://www.opensource.org/licenses/MIT.
 */

#ifndef BINWALK_H
#define BINWALK_H

#include <magic.h>
#include "md5.h"
#include "mparse.h"
#include "filter.h"
#include "common.h"
#include "dd.h"
#include "config.h"

/* These should be defined in the Makefile. If not, default to /etc/binwalk/magic.bin*. */
#ifndef MAGIC
#define MAGIC			"/etc/binwalk/magic.binwalk"
#endif
#ifndef MAGIC_CAST
#define MAGIC_CAST		"/etc/binwalk/magic.bincast"
#endif
#ifndef MAGIC_ARCH
#define MAGIC_ARCH		"/etc/binwalk/magic.binarch"
#endif

#define DATA			"data"
#define DATA_SIZE		4
#define TEXT			"text"
#define DEFAULT_BYTE_ALIGN	1
#define PROGRESS_INTERVAL	1000
#define MULTIPLE_MATCH_DELIM	"\\012- "
#define MULTIPLE_MATCH_NEWLINE	"\r\n\t\t\t\t"
#define MULTIPLE_MATCH_SIZE	6
#define GZIP_FILTER		"gzip"
#define LZMA_FILTER		"lzma"
#define JFFS_FILTER		"jffs2"
#define INVALID_FILTER		"invalid"

#define USAGE_OPTIONS		"\
\t-o, --offset=<int>            File offset to start searching at\n\
\t-l, --length=<int>            Number of bytes to search\n\
\t-b, --align=<int>             Set byte alignment\n\
\t-f, --file=<file>             Log results to file\n\
\t-m, --magic=<file>            Magic file to use [%s]\n\
\t-g, --grep=<string>           Only display results that contain the text <string>\n\
\t-r, --raw-bytes=<string>      Search for a sequence of raw bytes inside the target file (implies -a, -d, -I)\n\
\t-y, --search=<filter>         Only search for matches that have <filter> in their description (implies -t, -d, -k)\n\
\t-x, --exclude=<filter>        Exclude matches that have <filter> in their description\n\
\t-i, --include=<filter>        Include matches that are normally excluded and that have <filter> in their description *\n\
\t-D, --dd=<type:ext:count>     Extract <count> number of entries whose descriptions match <type> and save them with the file extension <ext> ****\n\
\t-a, --all                     Search for all matches, including those that are normally excluded *\n\
\t-d, --defaults                Speed up scan by disabling default filters **\n\
\t-I, --show-invalid            Show results marked as invalid ***\n\
\t-t, --fast                    Speed up scan by only loading signatures specified by -i or -y\n\
\t-u, --update                  Update magic signature files\n\
\t-v, --verbose                 Enable verbose mode\n\
\t-s, --smart                   Disable smart matching (implies -a)\n\
\t-k, --keep-going              Don't stop at the first match (implies -I)\n\
\t-c, --validate                Validate magic file\n\
\t-q, --quiet                   Supress output to stdout\n\
\t-A, --opcodes                 Scan for executable code (implies -a)\n\
\t-C, --cast                    Cast file contents as various data types (implies -k)\n\
\n\n\
*    Signatures of two bytes or less are excluded by default. Use -i or -a to include them in the search.\n\
\n\
**   Default filters include '%s', '%s' and '%s' results.\n\
     Disabling the default filters (-d) will speed up scan time, but may miss these file types.\n\
\n\
***  By default, all results that contain the text '%s' will not be shown. Use -I to display them.\n\
\n\
**** If count is not specified or is 0, all matching entries will be extracted. The type '%s' will match everything.\n\
"

struct binconf
{
	int smart;
	int verbose;
	int flags;
	int offset;
	int align;
	int length;
	char *magic;
	magic_t cookie;
};

struct progress_t
{
	size_t length;
	size_t offset;
} progress;

void usage(char *progname);
void *display_progress(void *arg);
void parse_dd_option(char *optarg);
int process_file(char *bin_file, struct binconf *config, struct magic_signature **signatures, int num_sigs, struct magic_filter **filters, int filter_count);

#endif

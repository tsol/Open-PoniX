/*
 * Command line parsing and main routines for Xdialog.
 */

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <stdio.h>
#ifdef STDC_HEADERS
#	include <stdlib.h>
#	include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#ifdef ENABLE_NLS
#	include <locale.h>
#endif

#include <gtk/gtk.h>

#ifdef HAVE_GETOPT_LONG_ONLY
#	include <getopt.h>
#else
#	include "getopt.h"
#endif

#include "interface.h"
#include "support.h"
#include "time.h"

/* A structure used to pass Xdialog parameters. */
Xdialog_data Xdialog;

/* (c)dialog compatibility flag */
gboolean dialog_compat = FALSE;

/* Usage displaying */

#define HELP_TEXT1 \
"Xdialog v"VERSION" by Thierry Godefroy <xdialog@free.fr> (v1.0 was\n\
written by Alfred at Cyberone Internet <alfred@cyberone.com.au>).\n\
Xdialog home page available at: http://xdialog.dyns.net/\n\
\n\
Usage: "

#define HELP_TEXT2 \
" [<common options>] [<transient options>] <box option> ...\n\
\n\
Common options:\n\
  --wmclass <name>\n\
  --rc-file <gtkrc filename>\n\
  --backtitle <backtitle>\n\
  --title <title>\n\
  --allow-close | --no-close\n\
  --screen-center | --under-mouse | --auto-placement\n\
  --center | --right | --left | --fill\n\
  --no-wrap | --wrap\n\
  --cr-wrap | --no-cr-wrap\n\
  --stderr | --stdout\n\
  --separator <character> | --separate-output\n\
  --buttons-style default|icon|text\n\
\n\
Transient options:\n\
  --fixed-font\n\
  --password (may be repeated 2 or 3 times before --2inputsbox or --3inputsbox)\n\
  --password=1|2 (for --2inputsbox or --3inputsbox)\n\
  --editable\n\
  --time-stamp | --date-stamp\n\
  --reverse\n\
  --keep-colors\n\
  --interval <timeout>\n\
  --timeout <timeout> (in seconds)\n\
  --no-tags\n\
  --item-help (if used, the {...} parameters are needed in menus/lists widgets)\n\
  --default-item <tag>\n\
  --icon <xpm filename>\n\
  --no-ok\n\
  --no-cancel\n\
  --no-buttons\n\
  --default-no\n\
  --wizard\n\
  --help <help>\n\
  --print <printer> (1)\n\
  --check <label> [<status>]\n\
  --ok-label <label>\n\
  --cancel-label <label>\n\
  --beep\n\
  --beep-after\n\
  --begin <Yorg> <Xorg>\n\
  --ignore-eof\n\
  --smooth\n\
\n\
Box options:\n\
  --yesno	<text> <height> <width>\n\
  --msgbox	<text> <height> <width>\n\
  --infobox	<text> <height> <width> [<timeout>]\n\
  --gauge	<text> <height> <width> [<percent>]\n\
  --progress	<text> <height> <width> [<maxdots> [[-]<msglen>]]\n\
  --inputbox	<text> <height> <width> [<init>]\n\
  --2inputsbox	<text> <height> <width> <label1> <init1> <label2> <init2>\n\
  --3inputsbox	<text> <height> <width> <label1> <init1> <label2> <init2> <label3> <init3>\n\
  --combobox	<text> <height> <width> <item1> ... <itemN>\n\
  --rangebox	<text> <height> <width> <min value> <max value> [<default value>]\n\
  --2rangesbox	<text> <height> <width> <label1> <min1> <max1> <def1> <label2> <min2> <max2> <def2>\n\
  --3rangesbox	<text> <height> <width> <label1> <min1> <max1> <def1> ... <label3> <min3> <max3> <def3>\n\
  --spinbox	<text> <height> <width> <min value> <max value> <default value> <label>\n\
  --2spinsbox	<text> <height> <width> <min1> <max1> <def1> <label1> <min2> <max2> <def2> <label2>\n\
  --3spinsbox	<text> <height> <width> <min1> <max1> <def1> <label1> ... <min3> <max3> <def3> <label3>\n\
  --textbox	<file> <height> <width>\n\
  --editbox	<file> <height> <width>\n\
  --tailbox	<file> <height> <width>\n\
  --logbox	<file> <height> <width>\n\
  --menubox	<text> <height> <width> <menu height> <tag1> <item1> {<help1>}...\n\
  --checklist	<text> <height> <width> <list height> <tag1> <item1> <status1> {<help1>}...\n\
  --radiolist	<text> <height> <width> <list height> <tag1> <item1> <status1> {<help1>}...\n\
  --buildlist	<text> <height> <width> <list height> <tag1> <item1> <status1> {<help1>}...\n\
  --treeview	<text> <height> <width> <list height> <tag1> <item1> <status1> <item_depth1> {<help1>}...\n\
  --fselect	<file> <height> <width>\n\
  --dselect	<directory> <height> <width>\n\
  --colorsel	<text> <height> <width> [<red> <green> <blue>]\n\
  --fontsel	<font name> <height> <width>\n\
  --calendar	<text> <height> <width> [<day> <month> <year>]\n\
  --timebox	<text> <height> <width> [<hours> <minutes> <seconds>]\n\
\n\
Special options:\n\
  --version		(prints version number to stderr and exits).\n\
  --print-version	(same as above in a cdialog-compatible way).\n\
  --print-maxsize	(prints maximum menu size in characters and exits).\n\
\n\
Note that <height> and <width> are in characters and may be replaced by a single\n\
XSIZExYSIZE[+/-XORG+/-YORG] parameter (like the one passed in the -geometry option\n\
of X) which will represent the size of the Xdialog window in pixels. Specifying\n\
a size of 0 0 (or 0x0) will auto-size Xdialog, while a size of -1 -1 (or -1x-1)\n\
will maximize it.\n\
\n\
(1) This Xdialog binary compiled with: "PRINTER_CMD" "PRINTER_CMD_OPTION"<printer>\n\
    as the print command. If <printer> is \"\" (an empty string), the "PRINTER_CMD_OPTION"\n\
    option is not used.\n\n"

#ifdef USE_SCANF

#define HELP_TEXT3 \
"WARNING: This binary of Xdialog was compiled with the --with-scanf-calls configure\n\
option. The --infobox and --gauge widgets may therefore not work completely as\n\
expected (lack of refresh of the GTK+ menu while scanning the input stream).\n\
You may try to re-compile Xdialog without specifying this configure option (but\n\
then, your system C library may lack the necessary functions).\n\n"

#define HELP_MSG_SIZE 4600

#else

#define HELP_MSG_SIZE 4200

#endif

/* List of all recognized Xdialog options */
enum {
	/* Box options */
	B_YESNO,
	B_MSGBOX,
	B_INFOBOX,
	B_GAUGE,
	B_PROGRESS,
	B_TAILBOX,
	B_LOGBOX,
	B_TEXTBOX,
	B_EDITBOX,
	B_INPUTBOX,		/* Don't change the order of the 3 next options ! */
	B_PASSWORDBOX,
	B_2INPUTSBOX,
	B_3INPUTSBOX,
	B_COMBOBOX,
	B_RANGEBOX,		/* Don't change the order of the 2 next options ! */
	B_2RANGESBOX,
	B_3RANGESBOX,
	B_SPINBOX,		/* Don't change the order of the 2 next options ! */
	B_2SPINSBOX,
	B_3SPINSBOX,
	B_MENUBOX,
	B_CHECKLIST,
	B_RADIOLIST,
	B_BUILDLIST,
	B_TREEVIEW,
	B_FSELECT,
	B_DSELECT,
	B_COLORSEL,
	B_FONTSEL,
	B_CALENDAR,
	B_TIMEBOX,
	/* Common options */
	C_TITLE,
	C_BACKTITLE,
	C_WMCLASS,
	C_SCREENCENTER,
	C_UNDERMOUSE,
	C_AUTOPLACEMENT,
	C_LEFT,
	C_RIGHT,
	C_CENTER,
	C_FILL,
	C_WRAP,
	C_NOWRAP,
	C_CRWRAP,
	C_NOCRWRAP,
	C_STDOUT,
	C_STDERR,
	C_NOCLOSE,
	C_ALLOWCLOSE,
	C_BUTTONSSTYLE,
	C_RCFILE,
	C_SEPARATOR,
	C_SEPARATEOUTPUT,
	/* Transient options */
	T_FIXEDFONT,
	T_PASSWORD,
	T_EDITABLE,
	T_TIMESTAMP,
	T_DATESTAMP,
	T_REVERSE,
	T_KEEPCOLORS,
	T_NOOK,
	T_NOCANCEL,
	T_NOBUTTONS,
	T_NOTAGS,
	T_ITEMHELP,
	T_DEFAULTITEM,
	T_HELP,
	T_PRINT,
	T_WIZARD,
	T_DEFAULTNO,
	T_OKLABEL,
	T_CANCELLABEL,
	T_ICON,
	T_INTERVAL,
	T_TIMEOUT,
	T_CHECK,
	T_BEEP,
	T_BEEPAFTER,
	T_BEGIN,
	T_IGNOREEOF,
	T_SMOOTH,
	/* Special options */
	S_PRINTMAXSIZE,
	S_VERSION,
	S_PRINTVERSION,
	S_CLEAR
};

static void print_help_info(char *name, char *errmsg)
{
	gchar msg[HELP_MSG_SIZE];
	gchar cmd[32];
#ifdef USE_GTK2
	GtkTextBuffer *text_buffer;
#endif

	strcpy(cmd, strlen(name) < 32 ? name : XDIALOG);

	strcpysafe(msg, HELP_TEXT1, HELP_MSG_SIZE);
	strcatsafe(msg, cmd, HELP_MSG_SIZE);
	strcatsafe(msg, HELP_TEXT2, HELP_MSG_SIZE);
#ifdef USE_SCANF
	strcatsafe(msg, HELP_TEXT3, HELP_MSG_SIZE);
#endif

	fprintf(stderr, "%s: %s !\n", cmd, errmsg);
	fprintf(stderr, msg);

	if (strlen(msg) == HELP_MSG_SIZE-1)
		fprintf(stderr, "\n\nHelp message truncated, please re-compile "\
				"after increasing HELP_MSG_SIZE in main.c !\n");

	strcpysafe(Xdialog.title, "Usage for ", MAX_TITLE_LENGTH);
	strcatsafe(Xdialog.title, cmd, MAX_TITLE_LENGTH);
	Xdialog.cancel_button = Xdialog.help = Xdialog.icon = Xdialog.check = FALSE;
	if (!Xdialog.print) {
		Xdialog.print = TRUE;
		Xdialog.printer[0] = 0;
	}
	Xdialog.fixed_font = Xdialog.buttons = TRUE;
	Xdialog.backtitle[0] = 0;
	Xdialog.set_origin = TRUE;
	Xdialog.xorg = Xdialog.yorg = 0;
	Xdialog.size_in_pixels = FALSE;
	get_maxsize(&Xdialog.xsize, &Xdialog.ysize);
	create_textbox("", FALSE);
#ifdef USE_GTK2
	text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Xdialog.widget1));
	gtk_text_buffer_insert_at_cursor(text_buffer, msg, strlen(msg));
#else
	gtk_text_insert(GTK_TEXT(Xdialog.widget1), NULL, NULL, NULL, msg,
			strlen(msg));
#endif
	gtk_widget_show(Xdialog.window);
	gtk_main();

	exit(255);
}

/* Returns TRUE when var_name environment variable exists and is set to "true"
 * (case insensitive).
 */
static gboolean is_true(const char *var_name)
{
	char *env_var;

	if ((env_var = getenv(var_name)) == NULL)
		return FALSE;

#ifdef HAVE_STRCASECMP
	return (!strcasecmp(env_var, "true") || !strcmp(env_var, "1"));
#else
	return (!strcmp(env_var, "true") || !strcmp(env_var, "True") ||
		!strcmp(env_var, "TRUE") || !strcmp(env_var, "1"));
#endif
}

/* An equivalent to the g_log_default_handler() function of glib, with
 * exclusive use of stderr (nothing is ever printed on stdout), and with a
 * possibility to disable completely any message output, via the
 * XDIALOG_NO_GMSGS environment variable.
 */
static void g_log_xdialog_handler(const gchar *log_domain,
				  GLogLevelFlags log_level,
				  const gchar *message, gpointer unused_data)
{
	gchar *recurse, *fatal, type[10];

	if (is_true("XDIALOG_NO_GMSGS"))
		return;

	recurse = (log_level & G_LOG_FLAG_RECURSION) != 0 ? "" : "(recursed) ";
	fatal = (log_level & G_LOG_FLAG_FATAL) != 0 ? "\naborting..." : "";
	log_level &= G_LOG_LEVEL_MASK;

	if (!message)
		message = "g_log_default_handler(): (NULL) message";

	switch (log_level) {
		case G_LOG_LEVEL_ERROR:
			strcpy(type, "ERROR");
			break;
		case G_LOG_LEVEL_CRITICAL:
			strcpy(type, "CRITICAL");
			break;
		case G_LOG_LEVEL_WARNING:
			strcpy(type, "WARNING");
			break;
		case G_LOG_LEVEL_MESSAGE:
			strcpy(type, "Message");
			break;
		case G_LOG_LEVEL_INFO:
			strcpy(type, "INFO");
			break;
		case G_LOG_LEVEL_DEBUG:
			strcpy(type, "DEBUG");
			break;
		default:
			sprintf(type, "LOG 0x%2x", log_level);
	}

	if (log_domain) {
		fprintf(stderr, "\n%s-", log_domain);
	} else {
		fprintf(stderr, "\n** ");
	}
	fprintf(stderr, "%s %s**: %s%s\n", type, recurse, message, fatal);
}

/* Xsize, Ysize, Xorg and Yorg parameters */

#ifdef HAVE_STRSTR
static void get_box_size(int argc, char *argv[], int *optind)
{
	char *index, *index2 = NULL;

	if (*optind > argc-1)
		print_help_info(argv[0], "box size missing");

	if ((index = strstr(argv[*optind], "x")) == NULL &&
	    argv[*optind + 1] == NULL)
		print_help_info(argv[0], "bad size parameter");

	/* Force auto-sizing when in dialog high compatibility mode
	 * and when XDIALOG_AUTO_SIZE is "true"
	 */
	if (dialog_compat) {
		if (is_true(FORCE_AUTOSIZE)) {
			Xdialog.ysize = Xdialog.xsize = 0;
			*optind += (index == NULL ? 2 : 1);
			return;
		}
	}

	if (index == NULL) {
		Xdialog.size_in_pixels = FALSE;
		Xdialog.ysize = atoi(argv[(*optind)++]);
		Xdialog.xsize = atoi(argv[(*optind)++]);
	} else {
		Xdialog.size_in_pixels = TRUE;
		Xdialog.set_origin = FALSE;
		Xdialog.xsize = atoi(argv[*optind]);
		Xdialog.ysize = atoi(++index);
		index  = strstr(argv[*optind], "+");
		index2 = strstr(argv[(*optind)++], "-");
		if (index != NULL && index2 != NULL) {
			if (index < index2) {
				Xdialog.xorg = atoi(++index);
				Xdialog.yorg = atoi(index2);
				if (Xdialog.yorg == 0)
					Xdialog.yorg = -1;
			} else {
				Xdialog.xorg = atoi(index2);
				if (Xdialog.xorg == 0)
					Xdialog.xorg = -1;
				Xdialog.yorg = atoi(++index);
			}
			Xdialog.set_origin = TRUE;
		} else if (index == NULL && index2 != NULL) {
				Xdialog.xorg = atoi(index2++);
				if (Xdialog.xorg == 0)
					Xdialog.xorg = -1;
				index2 = strstr(index2, "-");
				if (index2 != NULL) {
					Xdialog.yorg = atoi(index2);
					if (Xdialog.yorg == 0)
						Xdialog.yorg = -1;
					Xdialog.set_origin = TRUE;
				}
		} else if (index != NULL && index2 == NULL) {
				Xdialog.xorg = atoi(++index);
				index = strstr(index, "+");
				if (index != NULL) {
					Xdialog.yorg = atoi(++index);
					Xdialog.set_origin = TRUE;
				}
		}
	}

	/* If either of the two size parameters is negative, then
	 * use the maximum size for the box.
	 */
	if (Xdialog.xsize < 0 || Xdialog.ysize < 0) {
		get_maxsize(&Xdialog.xsize, &Xdialog.ysize);
		Xdialog.size_in_pixels = FALSE;
		Xdialog.set_origin = TRUE;
		Xdialog.xorg = Xdialog.yorg = 0;
	}
}
#else
#error strstr() function is needed by Xdialog !
#endif

static int get_list_size(int argc, char *argv[], int *optind, int parameters,
			 gboolean with_list_height)
{
	int list_size, next_opt, i;

	i = *optind;
	if (with_list_height)
		i++;

	if (i >= argc)
		print_help_info(argv[0], "missing list parameters");

	if (with_list_height) {
		Xdialog.list_height = atoi(argv[(*optind)++]);
		if (Xdialog.list_height < 0)
			print_help_info(argv[0], "negative list height");
	}

	next_opt = argc;

	for (i = *optind; i < argc-1; i++)
		if (strstr(argv[i], "--") == argv[i]) {
			next_opt = i;
			break;
		}

	list_size = (next_opt - *optind) / parameters;

	if (list_size * parameters + *optind != next_opt)
		print_help_info(argv[0],
				"incorrect number of parameters in list");

	if (list_size < 1)
		print_help_info(argv[0], "missing list parameters");

	return list_size;
}

static int param_number(int argc, char *argv[], int *optind)
{
	int next_opt, i;

	next_opt = argc;

	for (i = *optind; i < argc-1; i++)
		if (strstr(argv[i], "--") == argv[i]) {
			next_opt = i;
			break;
		}

	return (next_opt - *optind);

}

/* There is a problem with getopt_long_only that returns an optcode of 63 when
 * it reaches an unknown option !
 */
#define GETOPT_BUG 1

/* Main routine */

int main(int argc, char *argv[])
{
	gchar title_tmp[MAX_TITLE_LENGTH];
	gchar backt_tmp[MAX_BACKTITLE_LENGTH];
	gchar help_text[MAX_LABEL_LENGTH];
	int option_index = 0, old_optind = 0;
#if GETOPT_BUG
	int old_option_index = 0;
#endif
	char *env_var;
	int optcode, i;
	gboolean win = FALSE;
	gint timeout, percent, x, y;
	gint list_size = 0;
	gint min = 0, max = 0, deflt =0;
	gint day = 0, month = 0, year = 0;
	gint hours = 0, minutes = 0, seconds = 0;
	gdouble colors[4];
	gint beep_tmp, timeout_tmp;
	gboolean icon_tmp, check_tmp;
	time_t curr_time;
	struct tm *localdate;
	static struct option long_options[] = {
		/* Box options */
		{"yesno",		1, 0, B_YESNO},
		{"msgbox",		1, 0, B_MSGBOX},
		{"infobox",		1, 0, B_INFOBOX},
                {"gauge",		1, 0, B_GAUGE},
                {"guage",		1, 0, B_GAUGE},
                {"progress",		1, 0, B_PROGRESS},
                {"tailboxbg",		1, 0, B_TAILBOX},
                {"logbox",		1, 0, B_LOGBOX},
                {"textbox",		1, 0, B_TEXTBOX},
                {"editbox",		1, 0, B_EDITBOX},
                {"inputbox",		1, 0, B_INPUTBOX},
                {"passwordbox",		1, 0, B_PASSWORDBOX},
                {"2inputsbox",		1, 0, B_2INPUTSBOX},
                {"3inputsbox",		1, 0, B_3INPUTSBOX},
                {"combobox",		1, 0, B_COMBOBOX},
                {"rangebox",		1, 0, B_RANGEBOX},
                {"2rangesbox",		1, 0, B_2RANGESBOX},
                {"3rangesbox",		1, 0, B_3RANGESBOX},
                {"spinbox",		1, 0, B_SPINBOX},
                {"2spinsbox",		1, 0, B_2SPINSBOX},
                {"3spinsbox",		1, 0, B_3SPINSBOX},
                {"menubox",		1, 0, B_MENUBOX},
                {"checklist",		1, 0, B_CHECKLIST},
                {"radiolist",		1, 0, B_RADIOLIST},
                {"buildlist",		1, 0, B_BUILDLIST},
                {"treeview",		1, 0, B_TREEVIEW},
                {"fselect",		1, 0, B_FSELECT},
                {"dselect",		1, 0, B_DSELECT},
                {"colorsel",		1, 0, B_COLORSEL},
                {"fontsel",		1, 0, B_FONTSEL},
                {"calendar",		1, 0, B_CALENDAR},
                {"timebox",		1, 0, B_TIMEBOX},
		/* Common options */
                {"title",		1, 0, C_TITLE},
                {"backtitle",		1, 0, C_BACKTITLE},
                {"wmclass",		1, 0, C_WMCLASS},
                {"screen-center",	0, 0, C_SCREENCENTER},
                {"under-mouse",		0, 0, C_UNDERMOUSE},
                {"auto-placement",	0, 0, C_AUTOPLACEMENT},
                {"left",		0, 0, C_LEFT},
                {"right",		0, 0, C_RIGHT},
                {"center",		0, 0, C_CENTER},
                {"fill",		0, 0, C_FILL},
                {"wrap",		0, 0, C_WRAP},
                {"no-wrap",		0, 0, C_NOWRAP},
                {"cr-wrap",		0, 0, C_CRWRAP},
                {"no-cr-wrap",		0, 0, C_NOCRWRAP},
                {"stdout",		0, 0, C_STDOUT},
                {"stderr",		0, 0, C_STDERR},
                {"no-close",		0, 0, C_NOCLOSE},
                {"allow-close",		0, 0, C_ALLOWCLOSE},
                {"buttons-style",	1, 0, C_BUTTONSSTYLE},
                {"rc-file",		1, 0, C_RCFILE},
                {"separator",		1, 0, C_SEPARATOR},
                {"separate-output",	0, 0, C_SEPARATEOUTPUT},
		/* Transient options */
                {"fixed-font",		0, 0, T_FIXEDFONT},
                {"password",		2, 0, T_PASSWORD},
                {"editable",		0, 0, T_EDITABLE},
                {"time-stamp",		0, 0, T_TIMESTAMP},
                {"date-stamp",		0, 0, T_DATESTAMP},
                {"reverse",		0, 0, T_REVERSE},
                {"keep-colors",		0, 0, T_KEEPCOLORS},
                {"no-ok",		0, 0, T_NOOK},
                {"no-cancel",		0, 0, T_NOCANCEL},
                {"no-buttons",		0, 0, T_NOBUTTONS},
                {"no-tags",		0, 0, T_NOTAGS},
                {"item-help",		0, 0, T_ITEMHELP},
                {"default-item",	1, 0, T_DEFAULTITEM},
                {"help",		1, 0, T_HELP},
                {"print",		1, 0, T_PRINT},
                {"wizard",		0, 0, T_WIZARD},
                {"default-no",		0, 0, T_DEFAULTNO},
                {"defaultno",		0, 0, T_DEFAULTNO},
                {"ok-label",		1, 0, T_OKLABEL},
                {"cancel-label",	1, 0, T_CANCELLABEL},
                {"icon",		1, 0, T_ICON},
                {"interval",		1, 0, T_INTERVAL},
                {"timeout",		1, 0, T_TIMEOUT},
                {"check",		1, 0, T_CHECK},
                {"beep",		0, 0, T_BEEP},
                {"beep-after",		0, 0, T_BEEPAFTER},
                {"begin",		1, 0, T_BEGIN},
                {"ignore-eof",		0, 0, T_IGNOREEOF},
                {"smooth",		0, 0, T_SMOOTH},
		/* Special options */
                {"print-maxsize",	0, 0, S_PRINTMAXSIZE},
                {"version",		0, 0, S_VERSION},
                {"print-version",	0, 0, S_PRINTVERSION},
                {"clear",		0, 0, S_CLEAR},
		/* End of options marker */
		{0, 0, 0, 0}
	};

#ifdef ENABLE_NLS
#ifndef HAVE_SETLOCALE
#error setlocale() function unavailable, try: ./configure --disable-nls
#endif
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
#endif

	/* Check if the dialog high compatibility mode is requested */
	dialog_compat = is_true(HIGH_DIALOG_COMPAT);

	memset(&Xdialog, 0, sizeof(Xdialog_data));	/* Set all parameters to zero/NULL */

#if FALSE != 0	/* Just in case your compiler uses a different value for FALSE... */
	Xdialog.editable	= FALSE;		/* Don't allow editable combobox */
	Xdialog.time_stamp	= FALSE;		/* Don't time-stamp logbox messages */
	Xdialog.date_stamp	= FALSE;		/* Don't date-stamp logbox messages */
	Xdialog.reverse		= FALSE;		/* Don't reverse order in logbox */
	Xdialog.keep_colors	= FALSE;		/* Don't keep colors in logbox */
	Xdialog.fixed_font	= FALSE;		/* Don't use a fixed font as default */
	Xdialog.icon		= FALSE;		/* No icon as default */
	Xdialog.no_close	= FALSE;		/* Allow to close the box as default */
	Xdialog.help		= FALSE;		/* No help button as default */
	Xdialog.wizard		= FALSE;		/* No wizard buttons as default */
	Xdialog.check		= FALSE;		/* No check button as default */
	Xdialog.checked		= FALSE;		/* Check button unchecked as default */
	Xdialog.default_no	= FALSE;		/* Default selected button is Yes/OK */
	Xdialog.print		= FALSE;		/* No print button as default */
	Xdialog.wrap		= FALSE;		/* Don't wrap back-title & text as default */
	Xdialog.set_origin	= FALSE;		/* Don't set window origin */
	Xdialog.cr_wrap		= FALSE;		/* Don't wrap at linefeeds by default */
	Xdialog.ignore_eof	= FALSE;		/* Don't ignore EOF in infobox/gauge */
	Xdialog.smooth		= FALSE;		/* Don't use smooth (slow) scrolling  */
#endif
	if (dialog_compat) {
		Xdialog.justify	= GTK_JUSTIFY_LEFT;	/* Left justify messages as default */
	} else {
		Xdialog.justify	= GTK_JUSTIFY_CENTER;	/* Center messages as default */
		Xdialog.cr_wrap	= TRUE;			/* Wrap at linefeeds by default */
	}
	Xdialog.output		= stderr;		/* Default output for Xdialog results */
	Xdialog.placement	= GTK_WIN_POS_CENTER;	/* Center window on screen as default */
	Xdialog.buttons_style	= ICON_AND_TEXT;	/* Default buttons style (icon+text) */
	Xdialog.buttons		= TRUE;			/* Display buttons as default */
	Xdialog.ok_button	= TRUE;			/* Display "OK" button as default */
	Xdialog.cancel_button	= TRUE;			/* Display "Cancel" button as default */
	Xdialog.tags		= TRUE;			/* Display tags before items in lists */
#if 0	/* Not needed because of the memset: listed here as a reminder only... */
	Xdialog.passwd		= 0;			/* Don't use passwd input as default */
	Xdialog.interval	= 0;			/* Don't report periodically as default */
	Xdialog.timeout		= 0;			/* Don't use a timeout */
	Xdialog.tips		= 0;			/* Don't use tips for items in lists */
	Xdialog.beep		= 0;			/* Don't beep */
	Xdialog.backtitle[0]	= 0;			/* Defaults to no backtitle */
	Xdialog.rc_file[0]	= 0;			/* Defaults to no rc-file */
	Xdialog.default_item[0]	= 0;			/* No default item */
	Xdialog.ok_label[0]	= 0;			/* No alternative for OK button label */
	Xdialog.cancel_label[0]	= 0;			/* No alternative for CANCEL button label */
#endif
	strcpy(Xdialog.title, XDIALOG);			/* Default widget title */
	strcpy(Xdialog.separator, "/");			/* Default results separator */

	if (gtk_major_version < 1 ||
	    (gtk_major_version == 1 && gtk_minor_version < 2))
		fprintf(stderr,
			"%s: GTK+ version too old, please upgrade !\n", argv[0]);
	gtk_set_locale();

	/* Set custom log handler routines, so that GTK, GDK and GLIB never
         * print anything on stdout, but always use stderr instead.
	 */
	g_log_set_handler("Gdk", G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
			  | G_LOG_FLAG_RECURSION, g_log_xdialog_handler, NULL);
	g_log_set_handler("GLib", G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
			  | G_LOG_FLAG_RECURSION, g_log_xdialog_handler, NULL);
	g_log_set_handler("Gtk", G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
			  | G_LOG_FLAG_RECURSION, g_log_xdialog_handler, NULL);
	g_log_set_handler(NULL, G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
			  | G_LOG_FLAG_RECURSION, g_log_xdialog_handler, NULL);

	if (!gtk_init_check(&argc, &argv)) {
		fprintf(stderr,
			"%s: Error initializing the GUI...\n"
			"Do you run under X11 with GTK+ v1.2.0+ installed ?\n", argv[0]);
		return 255;
	}

	opterr = 0;

	while (TRUE) {
		optcode = getopt_long_only(argc, argv, "", long_options, &option_index);

		if (optcode == -1)
			break;

#if GETOPT_BUG
		/* Work around for a bug into getopt that returns optcode=63 for
		 * unrecognized options (the problem being that 63 is used for
		 * a valid option ==> crash !) !
		 */
		if (option_index == old_option_index && optcode == 63)
			continue;
		old_option_index = option_index;
#endif

		old_optind = optind;
show_again:
		win = FALSE;

		switch (optcode) {
		/* Box options */
			case B_YESNO:		/* a yesno box */
				get_box_size(argc, argv, &optind);
				create_msgbox(optarg, TRUE);
				win = TRUE;
				break;
			case B_MSGBOX:		/* a msgbox */
				get_box_size(argc, argv, &optind);
				create_msgbox(optarg, FALSE);
				win = TRUE;
				break;
			case B_INFOBOX:		/* an infobox */
				get_box_size(argc, argv, &optind);
				if (dialog_compat) {
					if ((env_var = getenv(INFOBOX_TIMEOUT)) != NULL)
						timeout = atoi(env_var);
					else
						timeout = 0;
					if (timeout > 0)
						create_infobox(optarg, timeout);
					else
						create_msgbox(optarg, FALSE);
				} else {
					if (argv[optind] != NULL)
						timeout = atoi(argv[optind++]);
					else
						timeout	= INFO_TIME;
					create_infobox(optarg, timeout);
				}
				win = TRUE;
				break;
			case B_GAUGE:		/* a gauge */
				get_box_size(argc, argv, &optind);
				if (argv[optind] == NULL)
					percent = 0;
				else
					percent = atoi(argv[optind++]);
				create_gauge(optarg, percent);
				win = TRUE;
				break;
			case B_PROGRESS:	/* a progress report box */
				get_box_size(argc, argv, &optind);
				if (argv[optind] == NULL) {
					max = 0;
					min = 0;
				} else {
					max = atoi(argv[optind++]);
					if (argv[optind] == NULL)
						min = 0;
					else
						min = atoi(argv[optind++]);
				}
				create_progress(optarg, min, max);
				win = TRUE;
				break;
			case B_TAILBOX:		/* a tail box */
				get_box_size(argc, argv, &optind);
				create_tailbox(optarg);
				win = TRUE;
				break;
			case B_LOGBOX:		/* a log box */
				get_box_size(argc, argv, &optind);
				create_logbox(optarg);
				win = TRUE;
				break;
			case B_TEXTBOX:		/* a text box */
			case B_EDITBOX:		/* an edit box */
				get_box_size(argc, argv, &optind);
				create_textbox(optarg, optcode == B_EDITBOX);
				win = TRUE;
				break;
			case B_INPUTBOX:	/* an inputbox */
			case B_PASSWORDBOX:	/* a passwordbox (cdialog compatible) */
			case B_2INPUTSBOX:	/* a 2inputsbox */
			case B_3INPUTSBOX:	/* a 3inputsbox */
				if (optcode == B_PASSWORDBOX)
					Xdialog.passwd = 1;
				get_box_size(argc, argv, &optind);
				if (optcode > B_PASSWORDBOX) {
					list_size = get_list_size(argc, argv, &optind, 2, FALSE);
					if (list_size != optcode - B_INPUTBOX)
						print_help_info(argv[0],
								"incorrect number of parameters");
				}
				create_inputbox(optarg, argv+optind,
						optcode >= B_2INPUTSBOX ? optcode - B_2INPUTSBOX + 2 : 1);
				if (optcode <= B_PASSWORDBOX)
					optind += ((argv[optind] != NULL) ? 1 : 0);
				else
					optind += 2*list_size;
				win = TRUE;
				break;
			case B_COMBOBOX:	/* a combo box */
				get_box_size(argc, argv, &optind);
				list_size = get_list_size(argc, argv, &optind, 1, FALSE);
				create_combobox(optarg, argv+optind, list_size);
				optind += list_size;
				win = TRUE;
				break;
			case B_RANGEBOX:	/* a range box */
			case B_2RANGESBOX:	/* a 2ranges box */
			case B_3RANGESBOX:	/* a 3ranges box */
				get_box_size(argc, argv, &optind);
				if (optcode != B_RANGEBOX) {
					list_size = get_list_size(argc, argv, &optind, 4, FALSE);
					if (list_size != optcode - B_RANGEBOX + 1)
						print_help_info(argv[0],
								"incorrect number of parameters");
					for (i = 0 ; i < list_size; i++) {
						min   = atoi(argv[optind+4*i+1]);
						max   = atoi(argv[optind+4*i+2]);
						deflt = atoi(argv[optind+4*i+3]);
						if (min >= max || deflt < min || deflt > max)
							print_help_info(argv[0],
									"bad range parameters");
					}
				} else {
					if (argv[optind] == NULL || argv[optind+1] == NULL)
						print_help_info(argv[0], "missing range parameter");
					else {
						min = atoi(argv[optind]);
						max = atoi(argv[optind+1]);
					}
					if (argv[optind+2] == NULL)
						deflt = min;
					else
						deflt = atoi(argv[optind+2]);
					if (min >= max || deflt < min || deflt > max)
						print_help_info(argv[0], "bad range parameters");
				}
				create_rangebox(optarg, argv+optind, optcode - B_RANGEBOX + 1);
				if (optcode != B_RANGEBOX)
					optind += 4*list_size;
				else
					optind += 2 + (argv[optind+2] != NULL ? 1 : 0);
				win = TRUE;
				break;
			case B_SPINBOX:		/* a spin box */
			case B_2SPINSBOX:	/* a 2spins box */
			case B_3SPINSBOX:	/* a 3spins box */
				get_box_size(argc, argv, &optind);
				list_size = get_list_size(argc, argv, &optind, 4, FALSE);
				if (list_size != optcode - B_SPINBOX + 1)
					print_help_info(argv[0], "incorrect number of parameters");
				for (i = 0 ; i < list_size; i++) {
					min   = atoi(argv[optind+4*i]);
					max   = atoi(argv[optind+4*i+1]);
					deflt = atoi(argv[optind+4*i+2]);
					if (min >= max || deflt < min || deflt > max)
						print_help_info(argv[0], "bad range parameters");
				}
				create_spinbox(optarg, argv+optind, optcode - B_SPINBOX + 1);
				optind += 4*list_size;
				win = TRUE;
				break;
			case B_MENUBOX:		/* a menu box */
				get_box_size(argc, argv, &optind);
				list_size = get_list_size(argc, argv, &optind,
							  2 + Xdialog.tips, TRUE);
				create_menubox(optarg, argv+optind, list_size);
				optind += (2+Xdialog.tips)*list_size;
				win = TRUE;
				break;
			case B_CHECKLIST:	/* a check list */
			case B_RADIOLIST:	/* a radio list */
				get_box_size(argc, argv, &optind);
				list_size = get_list_size(argc, argv, &optind, 3+Xdialog.tips, TRUE);
				create_itemlist(optarg,
						optcode == B_CHECKLIST ? CHECKLIST : RADIOLIST,
						argv+optind, list_size);
				optind += (3+Xdialog.tips)*list_size;
				win = TRUE;
				break;
			case B_BUILDLIST:	/* a build list */
				get_box_size(argc, argv, &optind);
				list_size = get_list_size(argc, argv, &optind,
							  3 + Xdialog.tips, TRUE);
				create_buildlist(optarg, argv+optind, list_size);
				optind += (3+Xdialog.tips)*list_size;
				win = TRUE;
				break;
			case B_TREEVIEW:	/* a tree view */
				get_box_size(argc, argv, &optind);
				list_size = get_list_size(argc, argv, &optind,
							  4 + Xdialog.tips, TRUE);
				create_treeview(optarg, argv+optind, list_size);
				optind += (4+Xdialog.tips)*list_size;
				win = TRUE;
				break;
			case B_FSELECT:		/* file selector */
				get_box_size(argc, argv, &optind);
				create_filesel(optarg, FALSE);
				win = TRUE;
				break;
			case B_DSELECT:		/* directory selector */
				get_box_size(argc, argv, &optind);
				create_filesel(optarg, TRUE);
				win = TRUE;
				break;
			case B_COLORSEL:	/* colour selector */
				get_box_size(argc, argv, &optind);
				list_size = param_number(argc, argv, &optind);
				if (list_size != 0) {
					if (list_size != 3)
						print_help_info(argv[0], "incorrect number of parameters");
					colors[0] = atoi(argv[optind++]) / 255.0;
                                        colors[1] = atoi(argv[optind++]) / 255.0;
                                        colors[2] = atoi(argv[optind++]) / 255.0;
                                }  else {
					colors[0] = colors[1] = colors[2] = 1.0;
				}
				create_colorsel(optarg, colors);
				win = TRUE;
				break;
			case B_FONTSEL:		/* font selector */
				get_box_size(argc, argv, &optind);
				create_fontsel(optarg);
				win = TRUE;
				break;
			case B_CALENDAR:	/* a calendar */
				get_box_size(argc, argv, &optind);
				list_size = param_number(argc, argv, &optind);
				if (list_size != 0) {
					if (list_size != 3)
						print_help_info(argv[0], "incorrect number of parameters");
					day   = atoi(argv[optind++]);
					month = atoi(argv[optind++]);
					year  = atoi(argv[optind++]);
				} else {
					day   = 0;
					month = 0;
					year  = 0;
				}
				if (day < 0 || day > 31 ||
				    month < 0 || month > 12 ||
				    (year != 0 && year < 1970))
					print_help_info(argv[0], "bad calendar date");
				if ( day == 0 || month == 0 || year == 0 ) {
					time(&curr_time);
					localdate = localtime(&curr_time);
					day = localdate->tm_mday;
					month = localdate->tm_mon + 1;
					year = localdate->tm_year + 1900;
				}
				create_calendar(optarg, day, month, year);
				win = TRUE;
				break;
			case B_TIMEBOX:		/* a time box */
				get_box_size(argc, argv, &optind);
				list_size = param_number(argc, argv, &optind);
				if (list_size != 0) {
					if (list_size != 3)
						print_help_info(argv[0], "incorrect number of parameters");
					hours   = atoi(argv[optind++]);
					minutes = atoi(argv[optind++]);
					seconds = atoi(argv[optind++]);
                                }  else {
					time(&curr_time);
					localdate = localtime(&curr_time);
					hours     = localdate->tm_hour;
					minutes   = localdate->tm_min;
					seconds   = localdate->tm_sec;
				}
				if (hours < 0 || hours > 23 ||
				    minutes < 0 || minutes > 59 ||
				    seconds < 0 || seconds > 59)
					print_help_info(argv[0], "bad time argument");
				create_timebox(optarg, hours, minutes, seconds);
				win = TRUE;
				break;
		/* Common options */
			case C_TITLE:		/* --title option */
				strcpysafe(Xdialog.title, optarg, MAX_TITLE_LENGTH);
	  			break;
			case C_BACKTITLE:	/* --backtitle option */
				strcpysafe(Xdialog.backtitle, optarg, MAX_BACKTITLE_LENGTH);
				break;
			case C_WMCLASS:		/* --wmclass option */
				strcpysafe(Xdialog.wmclass, optarg, MAX_WMCLASS_LENGTH);
				break;
			case C_SCREENCENTER:	/* --screen-center option */
				Xdialog.placement = GTK_WIN_POS_CENTER;
				break;
			case C_UNDERMOUSE:	/* --under-mouse option */
				Xdialog.placement = GTK_WIN_POS_MOUSE;
				break;
			case C_AUTOPLACEMENT:	/* --auto-placement option */
				Xdialog.placement = GTK_WIN_POS_NONE;
				break;
			case C_LEFT:		/* --left option */
				Xdialog.justify = GTK_JUSTIFY_LEFT;
				break;
			case C_RIGHT:		/* --right option */
				Xdialog.justify = GTK_JUSTIFY_RIGHT;
				break;
			case C_CENTER:		/* --center option */
				Xdialog.justify = GTK_JUSTIFY_CENTER; 
				break;
			case C_FILL:		/* --fill option */
				Xdialog.justify = GTK_JUSTIFY_FILL; 
				break;
			case C_WRAP:		/* --wrap option */
				Xdialog.wrap = TRUE;
				break;
			case C_NOWRAP:		/* --no-wrap option */
				Xdialog.wrap = FALSE;
				break;
			case C_CRWRAP:		/* --cr-wrap option */
				Xdialog.cr_wrap = TRUE;
				break;
			case C_NOCRWRAP:	/* --no-cr-wrap option */
				Xdialog.cr_wrap = FALSE;
				break;
			case C_STDOUT:		/* --stdout option */
				Xdialog.output = stdout;
				break;
			case C_STDERR:		/* --stderr option */
				Xdialog.output = stderr;
				break;
			case C_NOCLOSE:		/* --no-close option */
				Xdialog.no_close = TRUE;
				break;
			case C_ALLOWCLOSE:	/* --allow-close option */
				Xdialog.no_close = FALSE;
				break;
			case C_BUTTONSSTYLE:	/* --buttons-style option */
				if (!strcmp(optarg, "default"))
					Xdialog.buttons_style = ICON_AND_TEXT;
				else if (!strcmp(optarg, "icon"))
					Xdialog.buttons_style = ICON_ONLY;
				else if (!strcmp(optarg, "text"))
					Xdialog.buttons_style = TEXT_ONLY;
				else
					print_help_info(argv[0], "bad button style name");
				break;
			case C_RCFILE:		/* --rc-file option */
				strcpysafe(Xdialog.rc_file, optarg,
					   MAX_FILENAME_LENGTH);
				break;
			case C_SEPARATOR:	/* --separator option */
				if (!strcmp(optarg, "\\n"))
					Xdialog.separator[0] = '\n';
				else
					Xdialog.separator[0] = optarg[0];
				break;
			case C_SEPARATEOUTPUT:	/* --separate-output option */
				Xdialog.separator[0] = '\n';
				break;
		/* Transient options */
			case T_FIXEDFONT:	/* --fixed-font option */
				Xdialog.fixed_font = TRUE;
				break;
			case T_PASSWORD:	/* --passwd option */
				if (optarg == NULL) {
					if (Xdialog.passwd < 3)
						Xdialog.passwd++;
				} else {
					if (atoi(optarg) > 0 && atoi(optarg) < 4)
						Xdialog.passwd = atoi(optarg) + 10;
				}
				break;
			case T_EDITABLE:	/* --editable option */
				Xdialog.editable = TRUE;
				break;
			case T_TIMESTAMP:	/* --time-stamp option */
				Xdialog.time_stamp = TRUE;
				break;
			case T_DATESTAMP:	/* --date-stamp option */
				Xdialog.time_stamp = Xdialog.date_stamp = TRUE;
				break;
			case T_REVERSE:		/* --reverse option */
				Xdialog.reverse = TRUE;
				break;
			case T_KEEPCOLORS:	/* --keep-colors option */
				Xdialog.keep_colors = TRUE;
				break;
			case T_NOOK:	/* --no-ok option */
				Xdialog.ok_button = FALSE;
				break;
			case T_NOCANCEL:	/* --no-cancel option */
				Xdialog.cancel_button = FALSE;
				break;
			case T_NOBUTTONS:	/* --no-button option */
				Xdialog.buttons = FALSE;
				break;
			case T_NOTAGS:		/* --no-tags option */
				Xdialog.tags = FALSE;
				break;
			case T_ITEMHELP:	/* --item-help option */
				Xdialog.tips = 1;
				break;
			case T_DEFAULTITEM:	/* --default-item option */
				strcpysafe(Xdialog.default_item, optarg, MAX_ITEM_LENGTH);
				break;
			case T_HELP:		/* --help option */
				strcpysafe(help_text, optarg, MAX_LABEL_LENGTH);
				Xdialog.help = TRUE;
				break;
			case T_PRINT:		/* --print option */
				strcpysafe(Xdialog.printer, optarg, MAX_PRTNAME_LENGTH);
				Xdialog.print = TRUE;
				break;
			case T_WIZARD:		/* --wizard option */
				Xdialog.wizard = TRUE;
				break;
			case T_DEFAULTNO:	/* --default-no option */
				Xdialog.default_no = TRUE;
				break;
			case T_OKLABEL:		/* --ok-label option */
				strcpysafe(Xdialog.ok_label, optarg, MAX_BUTTON_LABEL_LENGTH);
				break;
			case T_CANCELLABEL:		/* --ok-label option */
				strcpysafe(Xdialog.cancel_label, optarg, MAX_BUTTON_LABEL_LENGTH);
				break;
			case T_ICON:		/* --icon option */
				strcpysafe(Xdialog.icon_file, optarg, MAX_FILENAME_LENGTH);
				Xdialog.icon = TRUE;
				break;
			case T_INTERVAL:	/* --interval option */
				Xdialog.interval = -1;
				if (optarg != NULL)
					Xdialog.interval = atoi(optarg);
				if (Xdialog.interval < 0)
					print_help_info(argv[0], "bad interval value");
				break;
			case T_TIMEOUT:		/* --timeout option */
				Xdialog.timeout = -1;
				if (optarg != NULL)
					Xdialog.timeout = atoi(optarg);
				if (Xdialog.timeout < 0)
					print_help_info(argv[0], "bad timeout value");
				break;
			case T_CHECK:		/* --check option */
				strcpysafe(Xdialog.check_label, optarg, MAX_LABEL_LENGTH);
				Xdialog.check = TRUE;
				list_size = param_number(argc, argv, &optind);
				if (list_size > 1)
					print_help_info(argv[0], "incorrect number of parameters");
				if (list_size)
#ifdef HAVE_STRCASECMP
					Xdialog.checked = !strcasecmp(argv[optind++], "on");
#else
					Xdialog.checked = !strcmp(argv[optind], "on") ||
				                          !strcmp(argv[optind], "On") ||
				                          !strcmp(argv[optind++], "ON");
#endif
				break;
			case T_BEEP:		/* --beep option */
				Xdialog.beep |= BEEP_BEFORE;
				break;
			case T_BEEPAFTER:	/* --beep-after option */
				Xdialog.beep |= BEEP_AFTER;
				break;
			case T_BEGIN:		/* --begin option */
				Xdialog.set_origin = TRUE;
				Xdialog.xorg = -1;
				if (optarg != NULL) {
					Xdialog.yorg = atoi(optarg);
					if (argv[optind] != NULL)
						Xdialog.xorg = atoi(argv[optind++]);
				}
				if (Xdialog.xorg < 0 || Xdialog.yorg < 0)
					print_help_info(argv[0], "bad window origin");
				break;
			case T_IGNOREEOF:	/* --ignore-eof option */
				Xdialog.ignore_eof = TRUE;
				break;
			case T_SMOOTH:		/* --smooth option */
				Xdialog.smooth = TRUE;
				break;
		/* Special options */
			case S_PRINTMAXSIZE:	/* --print-maxsize option */
				get_maxsize(&x, &y);
				fprintf(Xdialog.output, "MaxSize: %d, %d\n", y, x);
				return 0;
			case S_VERSION:		/* --version option */
			case S_PRINTVERSION:	/* --print-version (cdialog compatible) option */
				if (optcode == S_PRINTVERSION || dialog_compat)
					fprintf(Xdialog.output, "Version: "VERSION"\n");
				else
					fprintf(Xdialog.output, VERSION"\n");
				return 0;
			case S_CLEAR:		/* --clear option (cdialog compatibility trick) */
				win = TRUE;	/* don't complain about a missing box option */
				continue;	/* don't try to open a window, skip to next option */
		}
		if (win) {
			gtk_widget_show(Xdialog.window);
			gtk_main();

			switch (Xdialog.exit_code) {
				case 0: /* OK/Yes/Next pressed or normal termination */
					break;
				case 2:	/* Help button pressed */
					if (strlen(help_text) == 0)
						return Xdialog.exit_code;
					Xdialog.help = FALSE;
					strcpy(title_tmp, Xdialog.title);
					strcpy(Xdialog.title, HELP);
					strcpy(backt_tmp, Xdialog.backtitle);
					Xdialog.backtitle[0] = 0;
					icon_tmp = Xdialog.icon;
					Xdialog.icon = FALSE;
					check_tmp = Xdialog.check;
					Xdialog.check = FALSE;
					beep_tmp = Xdialog.beep;
					Xdialog.beep = 0;
					timeout_tmp = Xdialog.timeout;
					Xdialog.timeout = 0;
					create_msgbox(help_text, FALSE);
					gtk_widget_show(Xdialog.window);
					gtk_main();
					if (Xdialog.exit_code != 0)
						return Xdialog.exit_code;
					Xdialog.timeout = timeout_tmp;
					Xdialog.beep = beep_tmp;
					Xdialog.icon = icon_tmp;
					Xdialog.check = check_tmp;
					strcpy(Xdialog.title, title_tmp);
					strcpy(Xdialog.backtitle, backt_tmp);
					Xdialog.help = TRUE;
					optind = old_optind;
					Xdialog.exit_code = 2;	/* avoid beeping */
					goto show_again;
				default: /* No/Cancel/Previous/close pressed or error */
					return Xdialog.exit_code;
			}

			/* Reset transient options flags to their default values */
			Xdialog.passwd		= 0;
			Xdialog.editable	= FALSE;
			Xdialog.time_stamp	= FALSE;
			Xdialog.date_stamp	= FALSE;
			Xdialog.reverse		= FALSE;
			Xdialog.keep_colors	= FALSE;
			Xdialog.interval	= 0;
			Xdialog.timeout		= 0;
			Xdialog.icon		= FALSE;
			Xdialog.buttons		= TRUE;
			Xdialog.ok_button	= TRUE;
			Xdialog.cancel_button	= TRUE;
			Xdialog.help		= FALSE;
			Xdialog.print		= FALSE;
			Xdialog.default_no	= FALSE;
			Xdialog.wizard		= FALSE;
			Xdialog.check		= FALSE;
			Xdialog.checked		= FALSE;
			Xdialog.tags		= TRUE;
			Xdialog.tips		= 0;
			Xdialog.fixed_font	= FALSE;
			Xdialog.beep		= 0;
			Xdialog.set_origin	= FALSE;
			Xdialog.default_item[0]	= 0;
			Xdialog.ok_label[0]	= 0;
			Xdialog.cancel_label[0]	= 0;
			Xdialog.ignore_eof	= FALSE;
			Xdialog.smooth		= FALSE;
		}
	}

	if (!win)
		print_help_info(argv[0], "missing box option");
	return 0;
}

/*
 * defines for interface.h
 */

/* You may change the following defines IOT customize Xdialog... */

#define XDIALOG "Xdialog"	/* Default Xdialog window title */

#define INFO_TIME 1000		/* the number of ms an infobox should stay up*/

				/* Names for environment variables */
#define HIGH_DIALOG_COMPAT	"XDIALOG_HIGH_DIALOG_COMPAT"
#define FORCE_AUTOSIZE		"XDIALOG_FORCE_AUTOSIZE"
#define INFOBOX_TIMEOUT		"XDIALOG_INFOBOX_TIMEOUT"
                                /* Temporary pipe filename used for printing */
#define TEMP_FILE               "/tmp/Xdialog.tmp"

/* You may change these as well IOT change the Xdialog limitations... */

#define MAX_TREE_DEPTH 24
#define MAX_LABEL_LENGTH 2048
#define MAX_INPUT_DEFAULT_LENGTH 1024
#define MAX_BUTTON_LABEL_LENGTH 32
#define MAX_TITLE_LENGTH 64
#define MAX_BACKTITLE_LENGTH 256
#define MAX_CHECK_LABEL_LENGTH 256
#define MAX_WMCLASS_LENGTH 32
#define MAX_PRTNAME_LENGTH 64
#define MAX_ITEM_LENGTH 128
#define MAX_FILENAME_LENGTH 256
#define MAX_PRTNAME_LENGTH 64
#define MAX_PRTCMD_LENGTH MAX_PRTNAME_LENGTH+32

/* The following defines should be changed via the "configure" options, type:
 *    ./configure --help
 * to learn more about these options.
 */

#ifndef FIXED_FONT
#define FIXED_FONT "-*-*-medium-r-normal-*-*-*-*-*-m-70-*-*"
#endif

#ifndef PRINTER_CMD
#define PRINTER_CMD "lpr" 		/* Command to be used IOT print text */
#endif
#ifndef PRINTER_CMD_OPTION
#define PRINTER_CMD_OPTION "-P"		/* The option to specify the prt queue */
#endif
#ifdef HAVE_CATGETS			/* We don't support catgets */
#error Xdialog does not get catgets support, try: ./configure --enable-nls
#endif

#ifdef ENABLE_NLS			/* NLS is supported by default */
#include <libintl.h>
#define _(s) gettext(s)
#else
#undef _
#define _(s) s
#endif

#ifdef FRENCH				/* french translations without NLS */
#	define OK "OK"
#	define CANCEL "Annuler"
#	define YES "Oui"
#	define NO "Non"
#	define HELP "Aide"
#	define PREVIOUS "Précédent"
#	define NEXT "Suivant"
#	define PRINT "Imprimer"
#	define ADD "Ajouter"
#	define REMOVE "Retirer"
#	define TIME_FRAME_LABEL "Heures : Minutes : Secondes"
#	define TIME_STAMP "Heure"
#	define DATE_STAMP "Date - Heure"
#	define LOG_MESSAGE "Message"
#	define HIDE_TYPING "Masquer la saisie"
#else
#	define OK _("OK")
#	define CANCEL _("Cancel")
#	define YES _("Yes")
#	define NO _("No")
#	define HELP _("Help")
#	define PREVIOUS _("Previous")
#	define NEXT _("Next")
#	define PRINT _("Print")
#	define ADD _("Add")
#	define REMOVE _("Remove")
#	define TIME_FRAME_LABEL _("Hours : Minutes : Seconds")
#	define TIME_STAMP _("Time stamp")
#	define DATE_STAMP _("Date - Time")
#	define LOG_MESSAGE _("Log message")
#	define HIDE_TYPING _("Hide typing")
#endif

/* The following defines should not be changed. */

#define XSIZE_MULT 8
#define YSIZE_MULT 12

#define ALPHANUM_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"

#define FIXED_FONT_RC_STRING "style 'fixed_font' { font = \"" FIXED_FONT "\" }\nwidget '*' style 'fixed_font'"

#define ICON_AND_TEXT -1
#define ICON_ONLY 0
#define TEXT_ONLY 1

#define RADIOLIST 1
#define CHECKLIST 2

#define BEEP_BEFORE 1
#define BEEP_AFTER  2

/* Structure definitions. */

typedef struct {
	gint state;
	GtkWidget *widget;
	gchar tag[MAX_ITEM_LENGTH];
	gchar name[MAX_ITEM_LENGTH];
	gchar tips[MAX_ITEM_LENGTH];
} listname;

typedef struct	{
	gchar		title[MAX_TITLE_LENGTH];		/* Xdialog window title */
	gchar		backtitle[MAX_BACKTITLE_LENGTH];	/* Backtitle label */
	gchar		wmclass[MAX_WMCLASS_LENGTH];		/* Xdialog wmclass name */
	gchar		label_text[MAX_LABEL_LENGTH];		/* New label text (for infobox and gauge) */
	gchar		check_label[MAX_CHECK_LABEL_LENGTH];	/* Check button label text */
	gchar		ok_label[MAX_BUTTON_LABEL_LENGTH];	/* OK button label text */
	gchar		cancel_label[MAX_BUTTON_LABEL_LENGTH];	/* CANCEL button label text */
	gchar		default_item[MAX_ITEM_LENGTH];		/* Tag of the default item */
	gchar		separator[2];				/* Xdialog output result separator */
	gint		xsize;					/* Xdialog window X size */
	gint		ysize;					/* Xdialog window Y size */
	gint		xorg;					/* Xdialog window X origin */
	gint		yorg;					/* Xdialog window Y origin */
	gint		list_height;				/* Xdialog menu/list height (in lines) */
	gint		placement;				/* Xdialog window placement method */
	gint		justify;				/* Xdialog labels justification method */
	gint		buttons_style;				/* Xdialog buttons style */
	gint		interval;				/* Xdialog output result time interval */
	gint		timeout;				/* Xdialog user input timeout (in seconds) */
	gint		timer;					/* Xdialog timer routine */
	gint		timer2;					/* Xdialog timer routine #2 (for user timeout) */
	gint		passwd;					/* Password flags for text entries */
	gint		tips;					/* Tips flag (0 or 1) for tips in menu/list */
	gint		beep;					/* Beep flag */
	gboolean	size_in_pixels;				/* TRUE if xsize/ysize are in pixels */
	gboolean	set_origin;				/* TRUE if window origin to be set */
	gboolean	new_label;				/* TRUE if a start label delimiter received */
	gboolean	wrap;					/* TRUE if label text is to be auto-wrapped */
	gboolean	cr_wrap;				/* TRUE to wrap at linefeeds */
	gboolean	fixed_font;				/* TRUE if fixed font to be used in text */
	gboolean	tags;					/* TRUE if tags to be displayed in menu/list */
	gboolean	buttons;				/* FALSE to prevent setting up buttons */
	gboolean	ok_button;				/* FALSE to prevent setting up OK button in tailbox/logbox */
	gboolean	cancel_button;				/* FALSE to prevent setting up Cancel button */
	gboolean	help;					/* TRUE to setup the Help button */
	gboolean	default_no;				/* When TRUE No/Cancel is the default button */
	gboolean	wizard;					/* TRUE to setup Wizard buttons */
	gboolean	print;					/* TRUE to setup the Print button */
	gboolean	check;					/* TRUE to setup the check button */
	gboolean	checked;				/* TRUE when check button is checked */
	gboolean	icon;					/* TRUE to setup an icon */
	gboolean	no_close;				/* TRUE to forbid Xdialog window closing */
	gboolean	editable;				/* TRUE for an editable combobox */
	gboolean	time_stamp;				/* TRUE for time stamps in logbox */
	gboolean	date_stamp;				/* TRUE for date stamps in logbox */
	gboolean	reverse;				/* TRUE for reverse order in logbox */
	gboolean	keep_colors;				/* TRUE to remember colors in logbox */
	gboolean	ignore_eof;				/* TRUE to ignore EOF in infobox/gauge */
	gboolean	smooth;					/* TRUE for smooth scrolling in tail/log */
	GtkWidget *	window;
	GtkBox *	vbox;
	GtkWidget *	widget1;
	GtkWidget *	widget2;
	GtkWidget *	widget3;
	GtkWidget *	widget4;
	guint		status_id;
	FILE *		output;
	FILE *		file;
	long		file_init_size;				/* for logbox and tailbox */
	char		icon_file[MAX_FILENAME_LENGTH];
	char		rc_file[MAX_FILENAME_LENGTH];
	char		printer[MAX_PRTNAME_LENGTH];
	int		exit_code;
	listname *	array;
} Xdialog_data;


/* interface.c functions prototypes */

void get_maxsize(int *x, int *y);

void create_msgbox(gchar *optarg, gboolean yesno);

void create_infobox(gchar *optarg, gint timeout);

void create_gauge(gchar *optarg, gint percent);

void create_progress(gchar *optarg, gint leading, gint maxdots);

void create_tailbox(gchar *optarg);

void create_logbox(gchar *optarg);

void create_textbox(gchar *optarg, gboolean editable);

void create_inputbox(gchar *optarg, gchar *options[], gint entries);

void create_combobox(gchar *optarg, gchar *options[], gint list_size);

void create_rangebox(gchar *optarg, gchar *options[], gint ranges);

void create_spinbox(gchar *optarg, gchar *options[], gint spins);

void create_itemlist(gchar *optarg, gint type, gchar *options[], gint list_size);

void create_buildlist(gchar *optarg, gchar *options[], gint list_size);

void create_treeview(gchar *optarg, gchar *options[], gint list_size);

void create_menubox(gchar *optarg, gchar *options[], gint list_size);

void create_filesel(gchar *optarg, gboolean dsel_flag);

void create_colorsel(gchar *optarg, gdouble *colors);

void create_fontsel(gchar *optarg);

void create_calendar(gchar *optarg, gint day, gint month, gint year);

void create_timebox(gchar *optarg, gint hours, gint minutes, gint seconds);

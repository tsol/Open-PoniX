/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <signal.h>
#include <X11/cursorfont.h>
#include "aewm.h"
#include "parser.h"

Display *dpy;
Window root;
int screen;
XFontStruct *font;
#ifdef XFT
XftFont *xftfont;
XftColor xft_fg;
#endif
GC invert_gc;
GC string_gc;
GC border_gc;
XColor fg;
XColor bg;
XColor bd;
Cursor move_curs;
Cursor resize_curs;
Atom wm_state;
Atom wm_change_state;
Atom wm_protos;
Atom wm_delete;
#ifdef GNOME_PDA
Atom gnome_pda;
#endif
#ifdef MWM_HINTS
Atom mwm_hints;
#endif
client_t *head_client;
static char *opt_font = DEF_FONT;
static char *opt_fg = DEF_FG;
static char *opt_bg = DEF_BG;
static char *opt_bd = DEF_BD;
char *opt_new1 = DEF_NEW1;
char *opt_new2 = DEF_NEW2;
char *opt_new3 = DEF_NEW3;
int opt_bw = DEF_BW;
int opt_pad = DEF_PAD;
#ifdef SHAPE
Bool shape;
int shape_event;
#endif

static void scan_wins(void);
static void setup_display(void);
static void read_config_helper(FILE *);
static void read_config(char *);

#define USAGE \
    "usage: aewm [--config|-rc <file>]\n" \
        "            [--font|-fn <font>]\n" \
        "            [--fgcolor|-fg <color>]\n" \
        "            [--bgcolor|-bg <color>]\n" \
        "            [--bdcolor|-bd <color>]\n" \
        "            [--bdwidth|-bw <integer>]\n" \
        "            [--padding|-p <integer>]\n" \
        "            [--new1|-1 <command>]\n" \
        "            [--new2|-2 <command>]\n" \
        "            [--new3|-3 <command>]\n" \
        "            [--help|-h]\n" \
        "            [--version|-v]\n"

int main(int argc, char **argv)
{
    int i;
    struct sigaction act;

    read_config(NULL);

    for (i = 1; i < argc; i++) {
        if ARG("config", "rc", 1) {
            read_config(argv[++i]);
            continue;
        }
        if ARG("font", "fn", 1) {
            opt_font = argv[++i];
            continue;
        }
        if ARG("fgcolor", "fg", 1) {
            opt_fg   = argv[++i];
            continue;
        }
        if ARG("bgcolor", "bg", 1) {
            opt_bg   = argv[++i];
            continue;
        }
        if ARG("bdcolor", "bd", 1) {
            opt_bd   = argv[++i];
            continue;
        }
        if ARG("bdwidth", "bw", 1) {
            opt_bw   = atoi(argv[++i]);
            continue;
        }
        if ARG("padding", "p", 1) {
            opt_pad  = atoi(argv[++i]);
            continue;
        }
        if ARG("new1", "1", 1) {
            opt_new1 = argv[++i];
            continue;
        }
        if ARG("new2", "2", 1) {
            opt_new2 = argv[++i];
            continue;
        }
        if ARG("new3", "3", 1) {
            opt_new3 = argv[++i];
            continue;
        }
        if ARG("version", "v",0) {
            printf("aewm: version " VERSION "\n");
            exit(0);
        }
        if ARG("help", "h",0) {
            printf(USAGE);
            exit(0);
        }
        /* if we get here, it must be a bad option */
        err("unknown option: '%s'\n" USAGE, argv[i]);
        exit(2);
    }

    act.sa_handler = sig_handler;
    act.sa_flags = 0;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGHUP, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);

    setup_display();
    scan_wins();
    do_event_loop();
    return 1; /* just another brick in the -Wall */
}

static void scan_wins(void)
{
    unsigned int nwins, i;
    Window dummyw1, dummyw2, *wins;
    XWindowAttributes attr;

    XQueryTree(dpy, root, &dummyw1, &dummyw2, &wins, &nwins);
    for (i = 0; i < nwins; i++) {
        XGetWindowAttributes(dpy, wins[i], &attr);
        if (!attr.override_redirect && attr.map_state == IsViewable)
            make_new_client(wins[i]);
    }
    XFree(wins);
}

static void setup_display(void)
{
    XColor dummyc;
    XGCValues gv;
    XSetWindowAttributes sattr;
#ifdef SHAPE
    int dummy;
#endif

    dpy = XOpenDisplay(NULL);

    if (!dpy) {
        err("can't open display '%s' (is $DISPLAY set properly?)",
            getenv("DISPLAY"));
        exit(1);
    }

    XSetErrorHandler(handle_xerror);
    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);

    wm_protos = XInternAtom(dpy, "WM_PROTOCOLS", False);
    wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    wm_state = XInternAtom(dpy, "WM_STATE", False);
    wm_change_state = XInternAtom(dpy, "WM_CHANGE_STATE", False);
#ifdef GNOME_PDA
    gnome_pda = XInternAtom(dpy, "GNOME_PANEL_DESKTOP_AREA", False);
#endif
#ifdef MWM_HINTS
    mwm_hints = XInternAtom(dpy, _XA_MWM_HINTS, False);
#endif

    XAllocNamedColor(dpy, DefaultColormap(dpy, screen), opt_fg, &fg, &dummyc);
    XAllocNamedColor(dpy, DefaultColormap(dpy, screen), opt_bg, &bg, &dummyc);
    XAllocNamedColor(dpy, DefaultColormap(dpy, screen), opt_bd, &bd, &dummyc);

    font = XLoadQueryFont(dpy, opt_font);
    if (!font) { err("font '%s' not found", opt_font); exit(1); }

#ifdef XFT
    xft_fg.color.red = fg.red;
    xft_fg.color.green = fg.green;
    xft_fg.color.blue = fg.blue;
    xft_fg.color.alpha = 0xffff;
    xft_fg.pixel = fg.pixel;

    xftfont = XftFontOpenXlfd(dpy, DefaultScreen(dpy), opt_font);
    if (!xftfont) { err("font '%s' not found", opt_font); exit(1); }
#endif

#ifdef SHAPE
    shape = XShapeQueryExtension(dpy, &shape_event, &dummy);
#endif

    move_curs = XCreateFontCursor(dpy, XC_fleur);
    resize_curs = XCreateFontCursor(dpy, XC_plus);

    gv.function = GXcopy;
    gv.foreground = fg.pixel;
    gv.font = font->fid;
    string_gc = XCreateGC(dpy, root, GCFunction|GCForeground|GCFont, &gv);

    gv.foreground = bd.pixel;
    gv.line_width = opt_bw;
    border_gc = XCreateGC(dpy, root, GCFunction|GCForeground|GCLineWidth, &gv);

    gv.function = GXinvert;
    gv.subwindow_mode = IncludeInferiors;
    invert_gc = XCreateGC(dpy, root, GCFunction|GCSubwindowMode|GCLineWidth|GCFont, &gv);

    sattr.event_mask = ChildMask|ColormapChangeMask|ButtonMask;
    XChangeWindowAttributes(dpy, root, CWEventMask, &sattr);
}

static void read_config_helper(FILE *rc)
{
    char buf[BUF_SIZE], token[BUF_SIZE], *p;

    while (fgets(buf, sizeof buf, rc)) {
        /* comments and blank lines are skipped */
        if (buf[0] == '#' || buf[0] == '\n') continue;

        p = buf;
        while (get_token(&p, token)) {
            if (strcmp(token, "font") == 0) {
                if (get_token(&p, token))
                    opt_font = strdup(token);
            } else if (strcmp(token, "fgcolor") == 0) {
                if (get_token(&p, token))
                    opt_fg = strdup(token);
            } else if (strcmp(token, "bgcolor") == 0) {
                if (get_token(&p, token))
                    opt_bg = strdup(token);
            } else if (strcmp(token, "bdcolor") == 0) {
                if (get_token(&p, token))
                    opt_bd = strdup(token);
            } else if (strcmp(token, "bdwidth") == 0) {
                if (get_token(&p, token))
                    opt_bw = atoi(token);
            } else if (strcmp(token, "padding") == 0) {
                if (get_token(&p, token))
                    opt_pad = atoi(token);
            } else if (strcmp(token, "button1") == 0) {
                if (get_token(&p, token))
                    opt_new1 = strdup(token);
            } else if (strcmp(token, "button2") == 0) {
                if (get_token(&p, token))
                    opt_new2 = strdup(token);
            } else if (strcmp(token, "button3") == 0) {
                if (get_token(&p, token))
                    opt_new3 = strdup(token);
            }
        }
    }
}

/* If the user specifies an rc file, complain if it's not found, but
 * otherwise, don't. If we can't open any files, just proceed with the
 * defaults. */

static void read_config(char *rcfile)
{
    char defrc_buf[BUF_SIZE];
    FILE *rc;

    if (rcfile) {
        rc = fopen(rcfile, "r");
        if (!rc) err("rc '%s' not found", rcfile);
    } else {
        snprintf(defrc_buf, sizeof defrc_buf,
            "%s/.aewm/aewmrc", getenv("HOME"));
        rc = fopen(defrc_buf, "r");
        if (!rc) rc = fopen(DEF_RC, "r");
    }

    if (rc) {
        read_config_helper(rc);
        fclose(rc);
    }
}

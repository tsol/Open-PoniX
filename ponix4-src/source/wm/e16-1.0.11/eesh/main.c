/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2007 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

/* Global vars */
Display            *disp;

static char         buf[10240];
static int          stdin_state;
static char        *display_name;
static Client      *e;
static Window       my_win, comms_win;

static void
process_line(char *line)
{
   if (!line)
      exit(0);
   if (*line == '\0')
      return;

   CommsSend(e, line);
   XSync(disp, False);
}

static void
stdin_state_setup(void)
{
   stdin_state = fcntl(0, F_GETFL, 0);
   fcntl(0, F_SETFL, O_NONBLOCK);
}

static void
stdin_state_restore(void)
{
   fcntl(0, F_SETFL, stdin_state);
}

static void
stdin_read(void)
{
   static int          j = 0;
   int                 k, ret;

   k = 0;
   while ((ret = read(0, &(buf[j]), 1) > 0))
     {
	k = 1;
	if (buf[j] == '\n')
	  {
	     buf[j] = 0;
	     if (strlen(buf) > 0)
		process_line(buf);
	     j = -1;
	  }
	j++;
     }
   if ((ret < 0) || ((k == 0) && (ret == 0)))
      exit(0);
}

int
main(int argc, char **argv)
{
   XEvent              ev;
   Client             *me;
   int                 i;
   fd_set              fd;
   char               *command, *s;
   char                mode;
   int                 len, l;

   mode = 0;
   display_name = NULL;
   command = NULL;

   for (i = 1; i < argc; i++)
     {
	s = argv[i];
	if (*s != '-')
	   break;

	if (!strcmp(argv[i], "-e"))
	  {
	     mode = -1;
	     if (i != (argc - 1))
	       {
		  command = argv[++i];
	       }
	  }
	else if (!strcmp(argv[i], "-ewait"))
	  {
	     mode = 1;
	     if (i != (argc - 1))
		command = argv[++i];
	  }
	else if (!strcmp(argv[i], "-display"))
	  {
	     if (i != (argc - 1))
	       {
		  display_name = argv[++i];
		  display_name = Estrdup(display_name);
	       }
	  }
	else if ((!strcmp(argv[i], "-h")) ||
		 (!strcmp(argv[i], "-help")) || (!strcmp(argv[i], "--help")))
	  {
	     printf
		("eesh sends commands to E\n\n"
		 "Examples:\n"
		 "  eesh Command to Send to E then wait for a reply then exit\n"
		 "  eesh -ewait \"Command to Send to E then wait for a reply then exit\"\n"
		 "  eesh -e \"Command to Send to Enlightenment then exit\"\n\n");
	     printf("Use eesh by itself to enter the \"interactive mode\"\n"
		    "  Ctrl-D will exit interactive mode\n"
		    "  Use \"help\" from inside interactive mode for further assistance\n");
	     exit(0);
	  }
     }

   /* Open a connection to the diplay nominated by the DISPLAY variable */
   /* Or set with the -display option */
   disp = XOpenDisplay(display_name);
   if (!disp)
     {
	fprintf(stderr, "Failed to connect to X server\n");
	exit(1);
     }

   CommsInit();
   comms_win = CommsFindCommsWindow();
   my_win = CommsSetup(comms_win);

   e = ClientCreate(comms_win);
   me = ClientCreate(my_win);

   CommsSend(e, "set clientname eesh");
   CommsSend(e, "set version 0.2");
#if 0				/* Speed it up */
   CommsSend(e, "set author The Rasterman");
   CommsSend(e, "set email raster@rasterman.com");
   CommsSend(e, "set web http://www.enlightenment.org");
/* CommsSend(e, "set address NONE"); */
   CommsSend(e, "set info Enlightenment IPC Shell - talk to E direct");
/* CommsSend(e, "set pixmap 0"); */
#endif

   if (!command && i < argc)
     {
	mode = 1;
	len = 0;
	for (; i < argc; i++)
	  {
	     l = strlen(argv[i]);
	     command = EREALLOC(char, command, len + l + 2);

	     if (len)
		command[len++] = ' ';
	     strcpy(command + len, argv[i]);
	     len += l;
	  }
     }

   if (command)
     {
	/* Non-interactive */
	CommsSend(e, command);
	XSync(disp, False);
#if 0				/* No - Wait for ack */
	if (mode <= 0)
	   goto done;
#endif
     }
   else
     {
	/* Interactive */
	stdin_state_setup();
	atexit(stdin_state_restore);
     }

   for (;;)
     {
	XSync(disp, False);
	while (XPending(disp))
	  {
	     XNextEvent(disp, &ev);
	     switch (ev.type)
	       {
	       case ClientMessage:
		  s = CommsGet(me, &ev);
		  if (!s)
		     break;
		  if (*s)
		     printf("%s", s);
		  fflush(stdout);
		  Efree(s);
		  if (mode)
		     goto done;
		  break;
	       case DestroyNotify:
		  goto done;
	       }
	  }

	FD_ZERO(&fd);
	if (!command)
	   FD_SET(0, &fd);
	FD_SET(ConnectionNumber(disp), &fd);

	if (select(ConnectionNumber(disp) + 1, &fd, NULL, NULL, NULL) < 0)
	   break;

	if (FD_ISSET(0, &fd))
	  {
	     stdin_read();
	  }
     }

 done:
   ClientDestroy(e);
   ClientDestroy(me);

   return 0;
}

#if !USE_LIBC_STRDUP
char               *
Estrdup(const char *s)
{
   char               *ss;
   int                 sz;

   if (!s)
      return NULL;
   sz = strlen(s);
   ss = EMALLOC(char, sz + 1);
   strncpy(ss, s, sz + 1);

   return ss;
}
#endif

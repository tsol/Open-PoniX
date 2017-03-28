/*
 *  input_layer - Kernel ACPI Event Input Layer Interface
 *
 *  Handles the details of getting kernel ACPI events from the input
 *  layer (/dev/input/event*).
 *
 *  Inspired by (and in some cases blatantly lifted from) Vojtech Pavlik's
 *  evtest.c.
 *
 *  Copyright (C) 2008, Ted Felix (www.tedfelix.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  (tabs at 4)
 */

/* system */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <glob.h>

/* local */
#include "connection_list.h"
#include "kacpimon.h"

#include "input_layer.h"

#define DIM(a)  (sizeof(a) / sizeof(a[0]))

#define INPUT_LAYER_FS "/dev/input/event*"

/*-----------------------------------------------------------------*/
/* called when an input layer event is received */
static void process_input(int fd)
{
	struct input_event event;
	ssize_t nbytes;

	nbytes = read(fd, &event, sizeof(event));

	if (nbytes == 0) {
		printf("Input layer connection closed.\n");
		return;
	}
	
	if (nbytes < 0) {
		/* if it's a signal, bail */
		if (errno == EINTR)
			return;
		
		printf("Input layer read error: %s (%d)\n",
			strerror(errno), errno);
		return;
	}

	/* ??? Is it possible for a partial message to come across? */
	/*   If so, we've got more code to write... */
	
	if (nbytes != sizeof(event)) {
		printf("Input Layer unexpected Length\n");
		printf("  Expected: %lu   Got: %zd\n",
			(unsigned long) sizeof(event), nbytes);
		return;
	}

	/* If the Escape key was pressed, set the exitflag to exit. */
	if (event.type == EV_KEY  &&
	    event.code == KEY_ESC  &&
	    event.value == 1) {
		printf("Escape key pressed\n");
		exitflag = 1;
	}

	if (event.type == EV_SYN)
		printf("Input Layer:  Sync\n");
	else
		/* format and display the event struct in decimal */
		printf("Input Layer:  "
			"Type: %hu  Code: %hu  Value: %d\n",
			event.type, event.code, event.value);
}

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

/*-----------------------------------------------------------------*
 * open each of the appropriate /dev/input/event* files for input  */
void open_input(void)
{
	char *filename = NULL;
	glob_t globbuf;
	unsigned i;
	int fd;
	struct connection c;
	int had_some_success = 0;
	char evname[256];

	/* get all the matching event filenames */
	glob(INPUT_LAYER_FS, 0, NULL, &globbuf);

	/* for each event file */
	for (i = 0; i < globbuf.gl_pathc; ++i)
	{
		filename = globbuf.gl_pathv[i];

		fd = open(filename, O_RDONLY | O_NONBLOCK);
		if (fd >= 0) {
			/* get this event file's name */
			strcpy(evname, "Unknown");
			ioctl(fd, EVIOCGNAME(sizeof(evname)), evname);

			printf("%s (%s) opened successfully\n", filename, evname);
			had_some_success = 1;

			/* add a connection to the list */
			c.fd = fd;
			c.process = process_input;
			add_connection(&c);
		}
		else
		{
			if (had_some_success == 1)
				continue;
			int errno2 = errno;
			printf("open for %s failed: %s (%d)\n", 
				filename, strerror(errno2), errno2);
			if (errno2 == EACCES)
				printf("  (try running as root)\n");
			if (errno2 == ENOENT)
				printf("  (input layer driver may not be present)\n");
		}
	}

	globfree(&globbuf);
}


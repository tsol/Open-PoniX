/*
 *  kacpimon - Kernel ACPI Event Monitor
 *
 *  Monitors kernel ACPI events from multiple interfaces and reports them
 *  to the console.
 *
 *  Inspired by (and in some cases blatantly lifted from) Vojtech Pavlik's
 *  evtest.c, Zhang Rui's acpi_genl, and Alexey Kuznetsov's libnetlink.
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
#include <sys/select.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>

/* local */
#include "libnetlink.h"
#include "genetlink.h"
#include "acpi_genetlink.h"

#include "acpi_ids.h"
#include "connection_list.h"
#include "input_layer.h"
#include "netlink.h"

#include "kacpimon.h"

/* ??? Isn't this in a system header someplace? */
#define max(a, b)  (((a)>(b))?(a):(b))

/*********************************************************************/

/* Exit flag that can be set by any of the functions to cause the */
/* program to exit.  */
int exitflag = 0;

/****************************************************************
 *  Old /proc/acpi/event interface
 ****************************************************************/

static void process_proc(int fd)
{
	const int buffsize = 1024;
	char buffer[buffsize];
	ssize_t nbytes;

	for (;;)
	{
		nbytes = read(fd, buffer, buffsize - 1);

		/* ??? Do we need to worry about partial messages? */
		
		/* If there are no data to read, bail. */
		if (nbytes <= 0)
			break;

		/* Ensure we have a zero terminator */
		buffer[nbytes] = 0;
		
		printf("/proc/acpi/event: %s", buffer);
	}
}

// ---------------------------------------------------------------

static void open_proc(void)
{
	char *filename = "/proc/acpi/event";
	int fd;
	struct connection c;

	fd = open(filename, O_RDONLY | O_NONBLOCK);
	if (fd >= 0)
	{
		printf("%s opened successfully\n", filename);

		/* Add a connection to the list. */
		c.fd = fd;
		c.process = process_proc;
		add_connection(&c);
	}
	else
	{
		int errno2 = errno;
		printf("open for %s: %s (%d)\n", 
			filename, strerror(errno2), errno2);
		if (errno2 == EACCES)
			printf("  (try running as root)\n");
		if (errno2 == ENOENT)
			printf("  (ACPI proc filesystem may not be present)\n");
		if (errno2 == EBUSY)
			printf("  (ACPI proc filesystem is in use.  "
				"You might need to kill acpid.)\n");
	}
}

/****************************************************************
 *  Main Program Functions
 ****************************************************************/

static void monitor(void)
{
	while (exitflag == 0)
	{
		fd_set readfds;
		int nready;
		int i;

		/* It's going to get clobbered, so use a copy. */
		readfds = *get_fdset();

		/* Wait on data. */
		nready = select(get_highestfd() + 1, &readfds, NULL, NULL, NULL);

		/* If something goes wrong, bail. */
		if (nready <= 0)
			break;

		/* For each connection */
		for (i = 0; i <= get_number_of_connections(); ++i)
		{
			int fd;
			struct connection *p;

			p = get_connection(i);

			/* If this connection is invalid, bail. */
			if (!p)
				break;

			/* Get the file descriptor */
			fd = p -> fd;

			/* If this file descriptor has data waiting, */
			if (FD_ISSET(fd, &readfds))
			{
				p->process(fd);
			}
		}
	}
}

// ---------------------------------------------------------------

static void close_all(void)
{
	int i = 0;

	/* For each connection */
	for (i = 0; i <= get_number_of_connections(); ++i)
	{
		struct connection *p;

		p = get_connection(i);

		/* If this connection is invalid, try the next. */
		if (p == 0)
			continue;

		close(p -> fd);
	}
}

// ---------------------------------------------------------------

int main(void)
{
	printf("Kernel ACPI Event Monitor...\n");

	open_proc();
	open_input();
	open_netlink();

	printf("Press Escape to exit, or Ctrl-C if that doesn't work.\n");

	monitor();

	printf("Closing files...\n");

	close_all();

	printf("Goodbye\n");

	return 0;
}

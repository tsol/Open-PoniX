/*
 *  connection_list.c - ACPI daemon connection list
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
 *  Tabs at 4
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acpid.h"
#include "log.h"

#include "connection_list.h"

#define max(a, b)  (((a)>(b))?(a):(b))

/*---------------------------------------------------------------*/
/* private objects */

static int capacity = 0;

static struct connection *connection_list = NULL;

static int nconnections = 0;

/* fd_set containing all the fd's that come in */
static fd_set allfds;

/* highest fd that is opened */
/* (-2 + 1) causes select() to return immediately */
static int highestfd = -2;

/*---------------------------------------------------------------*/
/* public functions */

int
add_connection(struct connection *p)
{
	if (nconnections < 0)
		return -1;

	/* if the list is full, allocate more space */
	if (nconnections >= capacity) {
		/* no more than 1024 */
		if (capacity > 1024) {
			acpid_log(LOG_ERR, "Too many connections.");
			return -1;
		}

		/* another 20 */
		capacity += 20;
		connection_list =
			realloc(connection_list, sizeof(struct connection) * capacity);
	}

	if (nconnections == 0)
		FD_ZERO(&allfds);
	
	/* add the connection to the connection list */
	connection_list[nconnections] = *p;
	++nconnections;
	
	/* add to the fd set */
	FD_SET(p->fd, &allfds);
	highestfd = max(highestfd, p->fd);

	return 0;
}

/*---------------------------------------------------------------*/

void
delete_connection(int fd)
{
	int i;

	/* close anything other than stdin/stdout/stderr */
	if (fd > 2)
		close(fd);

	/* remove from the fd set */
	FD_CLR(fd, &allfds);

	for (i = 0; i < nconnections; ++i) {
		/* if the file descriptors match, delete the connection */
		if (connection_list[i].fd == fd) {
			free(connection_list[i].pathname);
			
			--nconnections;
			connection_list[i] = connection_list[nconnections];
			
			break;
		}
	}
	
	/* prepare for recalculation of highestfd */
	highestfd = -2;
 	
	/* recalculate highestfd */
	for (i = 0; i < nconnections; ++i) {
		highestfd = max(highestfd, connection_list[i].fd);
	}
}

/*---------------------------------------------------------------*/

void
delete_all_connections(void)
{
	/* while there are still connections to delete */
	while (nconnections) {
		/* delete the connection at the end of the list */
		delete_connection(connection_list[nconnections-1].fd);
	}

	free(connection_list);
	connection_list = NULL;
}

/*---------------------------------------------------------------*/

struct connection *
find_connection(int fd)
{
	int i;

	/* for each connection */
	for (i = 0; i < nconnections; ++i) {
		/* if the file descriptors match, return the connection */
		if (connection_list[i].fd == fd)
			return &connection_list[i];
	}

	return NULL;
}

/*---------------------------------------------------------------*/

struct connection *
find_connection_name(char *pathname)
{
	int i;

	/* for each connection */
	for (i = 0; i < nconnections; ++i) {
		/* skip null pathnames */
		if (connection_list[i].pathname == NULL)
			continue;

		/* if the pathname matches, return the connection */
		if (strcmp(connection_list[i].pathname, pathname) == 0)
			return &connection_list[i];
	}

	return NULL;
}

/*---------------------------------------------------------------*/

int 
get_number_of_connections()
{
	return nconnections;
}

/*---------------------------------------------------------------*/

struct connection *
get_connection(int i)
{
	if (i < 0  ||  i >= nconnections)
		return NULL;

	return &connection_list[i];
}

/*---------------------------------------------------------------*/

const fd_set *
get_fdset()
{
	return &allfds;
}

/*---------------------------------------------------------------*/

int
get_highestfd()
{
	return highestfd;
}

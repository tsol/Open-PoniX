/*
 *  sock.c - ACPI daemon socket interface
 *
 *  Portions Copyright (C) 2000 Andrew Henroid
 *  Portions Copyright (C) 2001 Sun Microsystems
 *  Portions Copyright (C) 2004 Tim Hockin (thockin@hockin.org)
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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <grp.h>

#include "acpid.h"
#include "log.h"
#include "event.h"
#include "ud_socket.h"
#include "connection_list.h"

#include "sock.h"

const char *socketfile = ACPID_SOCKETFILE;
const char *socketgroup;
mode_t socketmode = ACPID_SOCKETMODE;
int clientmax = ACPID_CLIENTMAX;

/* the number of non-root clients that are connected */
int non_root_clients;

/* determine if a file descriptor is in fact a socket */
int
is_socket(int fd)
{
    return (isfdtype(fd, S_IFSOCK) == 1);
}

/* accept a new client connection */
static void
process_sock(int fd)
{
	int cli_fd;
	struct ucred creds;
	char *buf;
	static int accept_errors;

	/* accept and add to our lists */
	cli_fd = ud_accept(fd, &creds);
	if (cli_fd < 0) {
		acpid_log(LOG_ERR, "can't accept client: %s",
			  strerror(errno));
		accept_errors++;
		if (accept_errors >= 5) {
			acpid_log(LOG_ERR, "giving up");
			clean_exit_with_status(EXIT_FAILURE);
		}
		return;
	}
	accept_errors = 0;

	/* don't allow too many non-root clients  */
	if (creds.uid != 0 && non_root_clients >= clientmax) {
		close(cli_fd);
		acpid_log(LOG_ERR, "too many non-root clients");
		return;
	}
	if (creds.uid != 0) {
		non_root_clients++;
	}

    if(asprintf(&buf, "%d[%d:%d]", creds.pid, creds.uid, creds.gid) < 0) {
        close(cli_fd);
        acpid_log(LOG_ERR, "asprintf: %s", strerror(errno));
        return;
     }
        acpid_add_client(cli_fd, buf);
        free(buf);
}

/* set up the socket for client connections */
void
open_sock()
{
	int fd;
	struct connection c;

	/* if this is a socket passed in via stdin by systemd */
	if (is_socket(STDIN_FILENO)) {
		fd = STDIN_FILENO;
		/* ??? Move CLOEXEC and NONBLOCK settings below up to here? */
	} else {
		/* create our own socket */
		fd = ud_create_socket(socketfile, socketmode);
		if (fd < 0) {
			acpid_log(LOG_ERR, "can't open socket %s: %s",
				socketfile, strerror(errno));
			exit(EXIT_FAILURE);
		}

		/* if we need to change the socket's group, do so */
		if (socketgroup) {
			struct group *gr;
			struct stat buf;

		    gr = getgrnam(socketgroup);
			if (!gr) {
				acpid_log(LOG_ERR, "group %s does not exist", socketgroup);
				exit(EXIT_FAILURE);
			}
			if (fstat(fd, &buf) < 0) {
				acpid_log(LOG_ERR, "can't stat %s: %s", 
		            socketfile, strerror(errno));
				exit(EXIT_FAILURE);
			}
			/* ??? I've tried using fchown(), however it doesn't work here.
			 *     It also doesn't work before bind().  The GNU docs seem to
			 *     indicate it isn't supposed to work with sockets. */
			/* if (fchown(fd, buf.st_uid, gr->gr_gid) < 0) { */
			if (chown(socketfile, buf.st_uid, gr->gr_gid) < 0) {
				acpid_log(LOG_ERR, "can't chown %s: %s", 
		            socketfile, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}

	/* Don't leak fds when execing.
	 * ud_create_socket() already does this, but there is no guarantee that
	 * a socket sent in via STDIN will have this set. */
	if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0) {
		close(fd);
		acpid_log(LOG_ERR, "fcntl() on socket %s for FD_CLOEXEC: %s", 
		          socketfile, strerror(errno));
		return;
	}

	/* Avoid a potential hang.
	 * ud_create_socket() already does this, but there is no guarantee that
	 * a socket sent in via STDIN will have this set. */
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		close(fd);
		acpid_log(LOG_ERR, "fcntl() on socket %s for O_NONBLOCK: %s", 
		          socketfile, strerror(errno));
		return;
	}
	
	/* add a connection to the list */
	c.fd = fd;
	c.process = process_sock;
	c.pathname = NULL;
	c.kybd = 0;

	if (add_connection(&c) < 0) {
		close(fd);
		acpid_log(LOG_ERR, "can't add connection for socket %s",
		          socketfile);
		return;
	}
}

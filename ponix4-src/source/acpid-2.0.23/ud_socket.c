/*
 * $Id: ud_socket.c,v 1.6 2009/04/22 18:22:28 thockin Exp $
 * A few  routines for handling UNIX domain sockets
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#include "acpid.h"
#include "log.h"
#include "ud_socket.h"

int
ud_create_socket(const char *name, mode_t socketmode)
{
	int fd;
	int r;
	struct sockaddr_un uds_addr;

    if (strnlen(name, sizeof(uds_addr.sun_path)) > 
        sizeof(uds_addr.sun_path) - 1) {
        acpid_log(LOG_ERR, "ud_create_socket(): "
            "socket filename longer than %zu characters: %s",
            sizeof(uds_addr.sun_path) - 1, name);
        errno = EINVAL;
        return -1;
    }

    /* JIC */
	unlink(name);

	fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
	if (fd < 0) {
		return fd;
	}

	/* Clear the umask to guarantee predictable results from fchmod(). */
	umask(0);

	if (fchmod(fd, socketmode) < 0) {
		close(fd);
		acpid_log(LOG_ERR, "fchmod() on socket %s: %s",
	        name, strerror(errno));
		return -1;
	}

	/* setup address struct */
	memset(&uds_addr, 0, sizeof(uds_addr));
	uds_addr.sun_family = AF_UNIX;
    strncpy(uds_addr.sun_path, name, sizeof(uds_addr.sun_path) - 1);
	
	/* bind it to the socket */
	r = bind(fd, (struct sockaddr *)&uds_addr, sizeof(uds_addr));
	if (r < 0) {
		close (fd);
		return r;
	}

	/* listen - allow 10 to queue */
	r = listen(fd, 10);
	if (r < 0) {
		close(fd);
		return r;
	}

	return fd;
}

int
ud_accept(int listenfd, struct ucred *cred)
{
	while (1) {
		int newsock = 0;
		struct sockaddr_un cliaddr;
		socklen_t len = sizeof(struct sockaddr_un);

		newsock = TEMP_FAILURE_RETRY (accept4(listenfd, (struct sockaddr *)&cliaddr, &len, SOCK_CLOEXEC|SOCK_NONBLOCK));
		if (newsock < 0) {
			return newsock;
		}
		if (cred) {
			len = sizeof(struct ucred);
			getsockopt(newsock,SOL_SOCKET,SO_PEERCRED,cred,&len);
		}

		return newsock;
	}
}

int
ud_connect(const char *name)
{
	int fd;
	int r;
	struct sockaddr_un addr;

    if (strnlen(name, sizeof(addr.sun_path)) > sizeof(addr.sun_path) - 1) {
        acpid_log(LOG_ERR, "ud_connect(): "
            "socket filename longer than %zu characters: %s",
            sizeof(addr.sun_path) - 1, name);
        errno = EINVAL;
        return -1;
    }
    
	fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
	if (fd < 0) {
		return fd;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	sprintf(addr.sun_path, "%s", name);
    /* safer: */
    /*strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);*/

	r = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (r < 0) {
		close(fd);
		return r;
	}

	return fd;
}

int
ud_get_peercred(int fd, struct ucred *cred)
{
	socklen_t len = sizeof(struct ucred);
	getsockopt(fd, SOL_SOCKET, SO_PEERCRED, cred, &len);
	return 0;
}

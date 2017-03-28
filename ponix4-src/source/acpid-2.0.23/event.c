/*
 *  event.c - ACPI daemon event handler
 *
 *  Copyright (C) 2000 Andrew Henroid
 *  Copyright (C) 2001 Sun Microsystems (thockin@sun.com)
 *  Copyright (C) 2004 Tim Hockin (thockin@hockin.org)
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <regex.h>
#include <signal.h>

#include "acpid.h"
#include "log.h"
#include "sock.h"
#include "ud_socket.h"
#include "event.h"
/*
 * What is a rule?  It's polymorphic, pretty much.
 */
#define RULE_REGEX_FLAGS (REG_EXTENDED | REG_ICASE | REG_NOSUB | REG_NEWLINE)
struct rule {
	enum {
		RULE_NONE = 0,
		RULE_CMD,
		RULE_CLIENT,
	} type;
	char *origin;
	regex_t *event;
	union {
		char *cmd;
		int fd;
	} action;
	struct rule *next;
	struct rule *prev;
};
struct rule_list {
	struct rule *head;
	struct rule *tail;
};

static struct rule_list drop_list;
static struct rule_list client_list;
static struct rule_list cmd_list;

/* rule routines */
static void enlist_rule(struct rule_list *list, struct rule *r);
static void delist_rule(struct rule_list *list, struct rule *r);
static struct rule *new_rule(void);
static void free_rule(struct rule *r);

/* other helper routines */
static void lock_rules(void);
static void unlock_rules(void);
static sigset_t *signals_handled(void);
static struct rule *parse_file(int fd_rule, const char *file);
static struct rule *parse_client(int client);
static int do_cmd_rule(struct rule *r, const char *event);
static int do_client_rule(struct rule *r, const char *event);
static int safe_write(int fd, const char *buf, int len);
static char *parse_cmd(const char *cmd, const char *event);
static int check_escapes(const char *str);

extern const char *dropaction;

/*
 * read in all the configuration files
 */
int
acpid_read_conf(const char *confdir)
{
	DIR *dir;
	struct dirent *dirent;
	int nrules = 0;
	regex_t preg;
	int rc = 0;

	lock_rules();

	dir = opendir(confdir);
	if (!dir) {
		acpid_log(LOG_ERR, "opendir(%s): %s",
			confdir, strerror(errno));
		unlock_rules();
		return -1;
	}

	/* Compile the regular expression.  This is based on run-parts(8). */
	rc = regcomp(&preg, "^[a-zA-Z0-9_-]+$", RULE_REGEX_FLAGS);
	if (rc) {
		closedir(dir);
		acpid_log(LOG_ERR, "regcomp(): %d", rc);
		unlock_rules();
		return -1;
	}

	/* scan all the files */
	while ((dirent = readdir(dir))) {
		struct rule *r;
		struct stat stat_buf;
        char *file = NULL;  /* rename: filename */
        int fd_rule;

		/* skip "." and ".." */
		if (strncmp(dirent->d_name, ".", sizeof(dirent->d_name)) == 0)
			continue;
		if (strncmp(dirent->d_name, "..", sizeof(dirent->d_name)) == 0)
			continue;

        if (asprintf(&file, "%s/%s", confdir, dirent->d_name) < 0) {
            acpid_log(LOG_ERR, "asprintf: %s", strerror(errno));
            regfree(&preg);
            closedir(dir);
            unlock_rules();
            return -1;
        }

		/* skip any files that don't match the run-parts convention */
		if (regexec(&preg, dirent->d_name, 0, NULL, 0) != 0) {
			acpid_log(LOG_INFO, "skipping conf file %s", file);
            free(file);
			continue;
		}

        /* ??? Check for DT_UNKNOWN and do this, then "else if" on not DT_REG
               and do the same as the !S_ISREG() branch below. */
        if (dirent->d_type != DT_REG) { /* may be DT_UNKNOWN ...*/
            /* allow only regular files and symlinks to files */
            if (fstatat(dirfd(dir), dirent->d_name, &stat_buf, 0) != 0) {
			    acpid_log(LOG_ERR, "fstatat(%s): %s", file, strerror(errno));
			    free(file);
			    continue; /* keep trying the rest of the files */
		    }
		    if (!S_ISREG(stat_buf.st_mode)) {
                acpid_log(LOG_INFO, "skipping non-file %s", file);
                free(file);
                continue; /* skip non-regular files */
            }
        }

        /* open the rule file (might want to move this into parse_file()?) */
		if ((fd_rule = openat(dirfd(dir), dirent->d_name, 
                              O_RDONLY|O_CLOEXEC|O_NONBLOCK)) == -1) {
                /* something went _really_ wrong.. Not Gonna Happen(tm) */
                acpid_log(LOG_ERR, "openat(%s): %s", file, strerror(errno));
                free(file);
                /* ??? Too extreme?  Why not just continue? */
                closedir(dir);
                regfree(&preg);
                unlock_rules();
                return -1;
        }
        /* fd is closed by parse_file() */
		r = parse_file(fd_rule, file);
		if (r) {
			/* if this is a drop rule */
			if (!strcmp(r->action.cmd, dropaction))
				enlist_rule(&drop_list, r);
			else
				enlist_rule(&cmd_list, r);
			nrules++;
		}
		free(file);
	}

    regfree(&preg);
	closedir(dir);
	unlock_rules();

	acpid_log(LOG_INFO, "%d rule%s loaded", nrules, (nrules == 1)?"":"s");

	return 0;
}

/*
 * cleanup all rules
 */
int
acpid_cleanup_rules(int do_detach)
{
	struct rule *p;
	struct rule *next;

	lock_rules();

	if (acpid_debug >= 3) {
		acpid_log(LOG_DEBUG, "cleaning up rules");
	}

	if (do_detach) {
		/* tell our clients to buzz off */
		p = client_list.head;
		while (p) {
			next = p->next;
			delist_rule(&client_list, p);
			close(p->action.fd);
			free_rule(p);
			p = next;
		}
	}

	/* clear out our conf rules */
	p = cmd_list.head;
	while (p) {
		next = p->next;
		delist_rule(&cmd_list, p);
		free_rule(p);
		p = next;
	}

	/* drop the drop rules */
	p = drop_list.head;
	while (p) {
		next = p->next;
		delist_rule(&drop_list, p);
		free_rule(p);
		p = next;
	}

	unlock_rules();

	return 0;
}

static struct rule *
parse_file(int fd_rule, const char *file)
{
	FILE *fp;
	char buf[512];
	int line = 0;
	struct rule *r;

	acpid_log(LOG_DEBUG, "parsing conf file %s", file);

    /* r - read-only, e - O_CLOEXEC */
	fp = fdopen(fd_rule, "re");
	if (!fp) {
		acpid_log(LOG_ERR, "fopen(%s): %s", file, strerror(errno));
		return NULL;
	}

	/* make a new rule */
	r = new_rule();
	if (!r) {
		fclose(fp);
		return NULL;
	}
	r->type = RULE_CMD;
	r->origin = strdup(file);
	if (!r->origin) {
		acpid_log(LOG_ERR, "strdup(): %s", strerror(errno));
		free_rule(r);
		fclose(fp);
		return NULL;
	}

	/* read each line */
	while (!feof(fp) && !ferror(fp)) {
		char *p = buf;
		char key[64];
		char val[512];
		int n;

		line++;
		memset(key, 0, sizeof(key));
		memset(val, 0, sizeof(val));

		if (fgets(buf, sizeof(buf)-1, fp) == NULL) {
			continue;
		}

		/* skip leading whitespace */
		while (*p && isspace((int)*p)) {
			p++;
		}
		/* blank lines and comments get ignored */
		if (!*p || *p == '#') {
			continue;
		}

		/* quick parse */
		n = sscanf(p, "%63[^=\n]=%255[^\n]", key, val);
		if (n != 2) {
			acpid_log(LOG_WARNING, "can't parse %s at line %d",
			    file, line);
			continue;
		}
		if (acpid_debug >= 3) {
			acpid_log(LOG_DEBUG, "    key=\"%s\" val=\"%s\"",
			    key, val);
		}
		/* handle the parsed line */
		if (!strcasecmp(key, "event")) {
			int rv;
			r->event = malloc(sizeof(regex_t));
			if (!r->event) {
				acpid_log(LOG_ERR, "malloc(): %s",
					strerror(errno));
				free_rule(r);
				fclose(fp);
				return NULL;
			}
			rv = regcomp(r->event, val, RULE_REGEX_FLAGS);
			if (rv) {
				char rbuf[128];
				regerror(rv, r->event, rbuf, sizeof(rbuf));
				acpid_log(LOG_ERR, "regcomp(): %s", rbuf);
				free_rule(r);
				fclose(fp);
				return NULL;
			}
		} else if (!strcasecmp(key, "action")) {
			if (check_escapes(val) < 0) {
				acpid_log(LOG_ERR, "can't load file %s",
				    file);
				free_rule(r);
				fclose(fp);
				return NULL;
			}
			r->action.cmd = strdup(val);
			if (!r->action.cmd) {
				acpid_log(LOG_ERR, "strdup(): %s",
					strerror(errno));
				free_rule(r);
				fclose(fp);
				return NULL;
			}
		} else {
			acpid_log(LOG_WARNING,
			    "unknown option '%s' in %s at line %d",
			    key, file, line);
			continue;
		}
	}
	if (!r->event || !r->action.cmd) {
		acpid_log(LOG_INFO, "skipping incomplete file %s", file);
		free_rule(r);
		fclose(fp);
		return NULL;
	}
	fclose(fp);

	return r;
}

int
acpid_add_client(int clifd, const char *origin)
{
	struct rule *r;
	int nrules = 0;

	acpid_log(LOG_NOTICE, "client connected from %s", origin);

	r = parse_client(clifd);
	if (r) {
		r->origin = strdup(origin);
		enlist_rule(&client_list, r);
		nrules++;
	}

	acpid_log(LOG_INFO, "%d client rule%s loaded",
	    nrules, (nrules == 1)?"":"s");

	return 0;
}

static struct rule *
parse_client(int client)
{
	struct rule *r;
	int rv;

	/* make a new rule */
	r = new_rule();
	if (!r) {
		return NULL;
	}
	r->type = RULE_CLIENT;
	r->action.fd = client;
	r->event = malloc(sizeof(regex_t));
	if (!r->event) {
		acpid_log(LOG_ERR, "malloc(): %s", strerror(errno));
		free_rule(r);
		return NULL;
	}
	rv = regcomp(r->event, ".*", RULE_REGEX_FLAGS);
	if (rv) {
		char buf[128];
		regerror(rv, r->event, buf, sizeof(buf));
		acpid_log(LOG_ERR, "regcomp(): %s", buf);
		free_rule(r);
		return NULL;
	}

	return r;
}

/*
 * a few rule methods
 */

static void
enlist_rule(struct rule_list *list, struct rule *r)
{
	r->next = r->prev = NULL;
	if (!list->head) {
		list->head = list->tail = r;
	} else {
		list->tail->next = r;
		r->prev = list->tail;
		list->tail = r;
	}
}

static void
delist_rule(struct rule_list *list, struct rule *r)
{
	if (r->next) {
		r->next->prev = r->prev;
	} else {
		list->tail = r->prev;
	}

	if (r->prev) {
		r->prev->next = r->next;
	} else {
		list->head = r->next;;
	}

	r->next = r->prev = NULL;
}

static struct rule *
new_rule(void)
{
	struct rule *r;

	r = malloc(sizeof(*r));
	if (!r) {
		acpid_log(LOG_ERR, "malloc(): %s", strerror(errno));
		return NULL;
	}

	r->type = RULE_NONE;
	r->origin = NULL;
	r->event = NULL;
	r->action.cmd = NULL;
	r->prev = r->next = NULL;

	return r;
}

/* I hope you delisted the rule before you free() it */
static void
free_rule(struct rule *r)
{
	if (r->type == RULE_CMD) {
		if (r->action.cmd) {
			free(r->action.cmd);
		}
	}

	if (r->origin) {
		free(r->origin);
	}
	if (r->event) {
		regfree(r->event);
		free(r->event);
	}

	free(r);
}

static int
client_is_dead(int fd)
{
	struct pollfd pfd;
	int r;

	/* check the fd to see if it is dead */
	pfd.fd = fd;
	pfd.events = POLLERR | POLLHUP;
	r = poll(&pfd, 1, 0);

	if (r < 0) {
		acpid_log(LOG_ERR, "poll(): %s", strerror(errno));
		return 0;
	}

	return pfd.revents;
}

void
acpid_close_dead_clients(void)
{
	struct rule *p;

	lock_rules();

	/* scan our client list */
	p = client_list.head;
	while (p) {
		struct rule *next = p->next;
		if (client_is_dead(p->action.fd)) {
			struct ucred cred;
			/* closed */
			acpid_log(LOG_NOTICE,
			    "client %s has disconnected", p->origin);
			delist_rule(&client_list, p);
			ud_get_peercred(p->action.fd, &cred);
			if (cred.uid != 0) {
				non_root_clients--;
			}
			close(p->action.fd);
			free_rule(p);
		}
		p = next;
	}

	unlock_rules();
}

/*
 * the main hook for propagating events
 */
int
acpid_handle_event(const char *event)
{
	struct rule *p;
	int nrules = 0;
	struct rule_list *ar[] = { &drop_list, &client_list, &cmd_list, NULL };
	struct rule_list **lp;

	/* make an event atomic wrt known signals */
	lock_rules();

	/* scan each rule list for any rules that care about this event */
	for (lp = ar; *lp; lp++) {
		struct rule_list *l = *lp;
		p = l->head;
		while (p) {
			/* the list can change underneath us */
			struct rule *pnext = p->next;
			if (!regexec(p->event, event, 0, NULL, 0)) {
				/* a match! */
				if (logevents) {
					acpid_log(LOG_INFO,
					    "rule from %s matched",
					    p->origin);
				}
				nrules++;
				if (p->type == RULE_CMD) {
					if (do_cmd_rule(p, event) == DROP_EVENT) {
						/* Abort processing if event matches drop rule */
						if (logevents)
							acpid_log(LOG_INFO, "event dropped");
						/* Skip the remaining rules. */
						while (*++lp)
							;
						--lp;
						break;
					}
				} else if (p->type == RULE_CLIENT) {
					do_client_rule(p, event);
				} else {
					acpid_log(LOG_WARNING,
					    "unknown rule type: %d",
					    p->type);
				}
			} else {
				if (acpid_debug >= 3 && logevents) {
					acpid_log(LOG_INFO,
					    "rule from %s did not match",
					    p->origin);
				}
			}
			p = pnext;
		}
	}

	unlock_rules();

	if (logevents) {
		acpid_log(LOG_INFO, "%d total rule%s matched",
			nrules, (nrules == 1)?"":"s");
	}

	return 0;
}

/* helper functions to block signals while iterating */
static sigset_t *
signals_handled(void)
{
	static sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGINT);

	return &set;
}

static void
lock_rules(void)
{
	if (acpid_debug >= 4) {
		acpid_log(LOG_DEBUG, "blocking signals for rule lock");
	}
	sigprocmask(SIG_BLOCK, signals_handled(), NULL);
}

static void
unlock_rules(void)
{
	if (acpid_debug >= 4) {
		acpid_log(LOG_DEBUG, "unblocking signals for rule lock");
	}
	sigprocmask(SIG_UNBLOCK, signals_handled(), NULL);
}

/*
 * the meat of the rules
 */

static int
do_cmd_rule(struct rule *rule, const char *event)
{
	pid_t pid;
	int status;
	const char *action;

	if (!strcmp(rule->action.cmd, dropaction))
		return DROP_EVENT;

	pid = fork();
	switch (pid) {
	case -1:
		acpid_log(LOG_ERR, "fork(): %s", strerror(errno));
		return -1;
	case 0: /* child */
		/* parse the commandline, doing any substitutions needed */
		action = parse_cmd(rule->action.cmd, event);
		if (logevents) {
			acpid_log(LOG_INFO,
			    "executing action \"%s\"", action);
		}

		/* reset signals */
		signal(SIGHUP, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGPIPE, SIG_DFL);
		sigprocmask(SIG_UNBLOCK, signals_handled(), NULL);

		if (acpid_debug && logevents) {
			fprintf(stdout, "BEGIN HANDLER MESSAGES\n");
		}
		umask(0077);
		execl("/bin/sh", "/bin/sh", "-c", action, NULL);
		/* should not get here */
		acpid_log(LOG_ERR, "execl(): %s", strerror(errno));
		_exit(EXIT_FAILURE);
	}

	/* parent */
	waitpid(pid, &status, 0);
	if (acpid_debug && logevents) {
		fprintf(stdout, "END HANDLER MESSAGES\n");
	}

	if (logevents) {
		if (WIFEXITED(status)) {
			acpid_log(LOG_INFO, "action exited with status %d",
			    WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			acpid_log(LOG_INFO, "action exited on signal %d",
			    WTERMSIG(status));
		} else {
			acpid_log(LOG_INFO, "action exited with status %d",
			    status);
		}
	}

	return 0;
}

static int
do_client_rule(struct rule *rule, const char *event)
{
	int r;
	int client = rule->action.fd;

	if (logevents) {
		acpid_log(LOG_INFO, "notifying client %s", rule->origin);
	}

	r = safe_write(client, event, strlen(event));
	if (r < 0 && errno == EPIPE) {
		struct ucred cred;
		/* closed */
		acpid_log(LOG_NOTICE,
		    "client %s has disconnected", rule->origin);
		delist_rule(&client_list, rule);
		ud_get_peercred(rule->action.fd, &cred);
		if (cred.uid != 0) {
			non_root_clients--;
		}
		close(rule->action.fd);
		free_rule(rule);
		return -1;
	}
	safe_write(client, "\n", 1);

	return 0;
}

#define NTRIES 100
static int
safe_write(int fd, const char *buf, int len)
{
	int r;
	int ttl = 0;
	int ntries = NTRIES;

	do {
		r = TEMP_FAILURE_RETRY (write(fd, buf+ttl, len-ttl) );
		if (r < 0) {
			if (errno != EAGAIN) {
				/* a legit error */
				return r;
			}
			ntries--;
		} else if (r > 0) {
			/* as long as we make forward progress, reset ntries */
			ntries = NTRIES;
			ttl += r;
		}
	} while (ttl < len && ntries);

	if (!ntries) {
		if (acpid_debug >= 2) {
			acpid_log(LOG_ERR, "safe_write() timed out");
		}
		return r;
	}

	return ttl;
}

static char *
parse_cmd(const char *cmd, const char *event)
{
	static char buf[4096];
	size_t i;
	const char *p;

	p = cmd;
	i = 0;

	memset(buf, 0, sizeof(buf));
	while (i < (sizeof(buf)-1)) {
		if (*p == '%') {
			p++;
			if (*p == 'e') {
				/* handle an event expansion */
				size_t size = sizeof(buf) - i;
				size = snprintf(buf+i, size, "%s", event);
				i += size;
				p++;
				continue;
			}
		}
		if (!*p) {
			break;
		}
		buf[i++] = *p++;
	}
	if (acpid_debug >= 2) {
		acpid_log(LOG_DEBUG, "expanded \"%s\" -> \"%s\"", cmd, buf);
	}

	return buf;
}

static int
check_escapes(const char *str)
{
	const char *p;
	int r = 0;

	p = str;
	while (*p) {
		/* found an escape */
		if (*p == '%') {
			p++;
			if (!*p) {
				acpid_log(LOG_WARNING,
				    "invalid escape at EOL");
				return -1;
			} else if (*p != '%' && *p != 'e') {
				acpid_log(LOG_WARNING,
				    "invalid escape \"%%%c\"", *p);
				r = -1;
			}
		}
		p++;
	}
	return r;
}

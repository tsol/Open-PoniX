#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <X11/Xlib.h>

char *tzargentina = "America/Buenos_Aires";
char *tzutc = "UTC";
char *tzberlin = "Europe/Berlin";

static Display *dpy;

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

void
settz(char *tzname)
{
	setenv("TZ", tzname, 1);
}

char *
mktimes(char *fmt, char *tzname)
{
	char buf[129];
	time_t tim;
	struct tm *timtm;

	bzero(buf, sizeof(buf));
	settz(tzname);
	tim = time(NULL);
	timtm = localtime(&tim);
	if (timtm == NULL) {
		perror("localtime");
		exit(1);
	}

	if (!strftime(buf, sizeof(buf)-1, fmt, timtm)) {
		fprintf(stderr, "strftime == 0\n");
		exit(1);
	}

	return smprintf("%s", buf);
}

void
setstatus(char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

char *
loadavg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0) {
		perror("getloadavg");
		exit(1);
	}

	return smprintf("%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}

char *
getbattery(char *base)
{
	char *path, line[513];
	FILE *fd;
	int descap, remcap;

	descap = -1;
	remcap = -1;

	path = smprintf("%s/info", base);
	fd = fopen(path, "r");
	if (fd == NULL) {
		perror("fopen");
		exit(1);
	}
	free(path);
	while (!feof(fd)) {
		if (fgets(line, sizeof(line)-1, fd) == NULL)
			break;

		if (!strncmp(line, "present", 7)) {
			if (strstr(line, " no")) {
				descap = 1;
				break;
			}
		}
		if (!strncmp(line, "design capacity", 15)) {
			if (sscanf(line+16, "%*[ ]%d%*[^\n]", &descap))
				break;
		}
	}
	fclose(fd);

	path = smprintf("%s/state", base);
	fd = fopen(path, "r");
	if (fd == NULL) {
		perror("fopen");
		exit(1);
	}
	free(path);
	while (!feof(fd)) {
		if (fgets(line, sizeof(line)-1, fd) == NULL)
			break;

		if (!strncmp(line, "present", 7)) {
			if (strstr(line, " no")) {
				remcap = 1;
				break;
			}
		}
		if (!strncmp(line, "remaining capacity", 18)) {
			if (sscanf(line+19, "%*[ ]%d%*[^\n]", &remcap))
				break;
		}
	}
	fclose(fd);

	if (remcap < 0 || descap < 0)
		return NULL;

	return smprintf("%.0f", ((float)remcap / (float)descap) * 100);
}

int
main(void)
{
	char *status;
	char *avgs;
	char *bat;
	char *tmar;
	char *tmutc;
	char *tmbln;

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	for (;;sleep(90)) {
		avgs = loadavg();
		bat = getbattery("/proc/acpi/battery/BAT0");
		tmar = mktimes("%H:%M", tzargentina);
		tmutc = mktimes("%H:%M", tzutc);
		tmbln = mktimes("KW %W %a %d %b %H:%M %Z %Y", tzberlin);

		status = smprintf("[L: %s|B: %s%%|A: %s|U: %s|%s]",
				avgs, bat, tmar, tmutc, tmbln);
		setstatus(status);
		free(avgs);
		free(bat);
		free(tmar);
		free(tmutc);
		free(tmbln);
		free(status);
	}

	XCloseDisplay(dpy);

	return 0;
}


bin_PROGRAMS = kcompmgr
man_MANS = kcompmgr.1

kcompmgr_LDADD = @KCOMPMGR_LIBS@ -lm

INCLUDES = @KCOMPMGR_CFLAGS@

EXTRA_DIST = \
	autogen.sh \
	ChangeLog \
	${man_MANS}

MAINTAINERCLEANFILES=ChangeLog

.PHONY: ChangeLog

ChangeLog:
	(GIT_DIR=$(top_srcdir)/.git git-log > .changelog.tmp && mv .changelog.tmp ChangeLog; rm -f .changelog.tmp) || (touch ChangeLog; echo 'git directory not found: installing possibly empty changelog.' >&2)

dist-hook: ChangeLog


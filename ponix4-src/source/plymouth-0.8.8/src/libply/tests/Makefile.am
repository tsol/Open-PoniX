INCLUDES =                                                                    \
           -I$(top_srcdir)                                                    \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)
TESTS = 

if ENABLE_TESTS
include $(srcdir)/ply-terminal-session-test.am
include $(srcdir)/ply-logger-test.am
include $(srcdir)/ply-array-test.am
include $(srcdir)/ply-bitarray-test.am
include $(srcdir)/ply-list-test.am
include $(srcdir)/ply-hashtable-test.am
include $(srcdir)/ply-event-loop-test.am
include $(srcdir)/ply-command-parser-test.am
include $(srcdir)/ply-progress-test.am
include $(srcdir)/ply-region.am
endif

noinst_PROGRAMS = $(TESTS)

# Our tests aren't unit tests so clear for now
TESTS =

MAINTAINERCLEANFILES = Makefile.in

INCLUDES =                                                                    \
           -I$(top_srcdir)                                                    \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)/../libply                                              \
           -I$(srcdir)/../libply-splash-core                                  \
           -I$(srcdir)
TESTS =

if ENABLE_TESTS
include $(srcdir)/ply-boot-server-test.am
include $(srcdir)/ply-boot-splash-test.am
endif

noinst_PROGRAMS = $(TESTS)

# our tests aren't unit tests, so clear for now
TESTS =
MAINTAINERCLEANFILES = Makefile.in

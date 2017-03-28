## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign no-dependencies

localedir = $(datadir)/locale
INCLUDES = -DLOCALEDIR=\"$(localedir)\" -I.

noinst_LIBRARIES = libgetopt.a

libgetopt_a_SOURCES = getopt.c getopt1.c getopt.h


#! /usr/bin/make -f

version=`gawk 'BEGIN{FS=","}/AM_INIT_AUTOMAKE/{gsub(/[)]/,"",$$2);gsub(/ /,"",$$2);printf "%s",$$2}' configure.in`

all: tar

tar:
	@echo $(version)
#	aclocal
#	autoheader
#	autoconf
#	automake
	@if [ -f Makefile ]; \
	then \
	make distclean; \
	fi
	@ln -s gamix ../gamix-$(version)
	@tar --exclude CVS -zch -C .. -f ../gamix-$(version).tar.gz gamix-$(version)
	@rm ../gamix-$(version)

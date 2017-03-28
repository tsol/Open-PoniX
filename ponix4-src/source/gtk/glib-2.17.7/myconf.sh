#!/bin/sh

./configure --prefix=/usr --disable-man

#  --enable-gc-friendly    turn on garbage collector friendliness [default=no]
#  --disable-mem-pools     disable all glib memory pools
#  --enable-threads        turn on basic thread support [default=yes] ([=no]
#                          will override --with-threads)
#  --disable-rebuilds      disable all source autogeneration rules
#  --disable-visibility    don't use ELF visibility attributes
#  --disable-largefile     omit support for large files
#  --enable-iconv-cache=[yes/no/auto]
#                          cache iconv descriptors [default=auto]
#  --disable-selinux       build without selinux support
#  --disable-fam          build without enabling fam for file system monitoring
#  --disable-xattr           build without xattr support
#  --disable-regex         disable the compilation of GRegex
#  --enable-gtk-doc        use gtk-doc to build documentation [default=no]
#  --enable-man            regenerate man pages from Docbook [default=no]
#
#Optional Packages:
#  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
#  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
#  --with-libiconv=[no/gnu/native]
#                          use the libiconv library
#  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]
#  --with-pic              try to use only PIC/non-PIC objects [default=use
#                          both]
#  --with-tags[=TAGS]      include additional configurations [automatic]
#  --with-gio-module-dir=PATH
#                          Load gio modules from this directory
#                          [LIBDIR/gio/modules]
#  --with-threads=[none/posix/dce/win32]
#                          specify a thread implementation to use
#  --with-pcre=[internal/system]
#                          specify whether to use the internal or the
#                          system-supplied PCRE library
#  --with-html-dir=PATH    path to installed docs
#  --with-xml-catalog=CATALOG
#                          path to xml catalog to use

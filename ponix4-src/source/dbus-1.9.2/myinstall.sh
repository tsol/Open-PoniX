#!/bin/sh

./configure --prefix=/usr --disable-tests --x-includes=/usr/xorg/include --x-libraries=/usr/xorg/lib \
--disable-doxygen-docs --disable-systemd --disable-embedded-tests --disable-modular-tests \
--disable-tests --disable-installed-tests --enable-x11-autolaunch --disable-stats--enable-x11-autolaunch \
--disable-Werror --with-x --with-dbus-glib=no


#X features:
#  --x-includes=DIR    X include files are in DIR
#  --x-libraries=DIR   X library files are in DIR
#  --enable-developer      set defaults to be appropriate for a D-Bus developer
#                          instead of a distribution/end-user
#  --enable-ansi           enable -ansi -pedantic gcc flags
#  --enable-verbose-mode   support verbose debug mode
#  --enable-asserts        include assertion checks
#  --enable-checks         include sanity checks on public API
#  --enable-xml-docs       build XML documentation (requires xmlto)
#  --enable-doxygen-docs   build DOXYGEN documentation (requires Doxygen)
#  --enable-abstract-sockets
#                          use abstract socket namespace (linux only)
#  --enable-selinux        build with SELinux support
#  --enable-libaudit       build audit daemon support for SELinux
#  --enable-inotify        build with inotify support (linux only)
#  --enable-kqueue         build with kqueue support
#  --enable-console-owner-file
#                          enable console owner file
#  --enable-launchd        build with launchd auto-launch support
#  --enable-systemd        build with systemd at_console support
#  --enable-embedded-tests enable unit test code in the library and binaries
#  --enable-modular-tests  enable modular regression tests (requires GLib)
#  --enable-tests          enable/disable all tests, overriding
#                          embedded-tests/modular-tests
#  --enable-installed-tests
#                          enable unit test code in the library and binaries
#  --enable-epoll          use epoll(4) on Linux
#  --enable-x11-autolaunch build with X11 auto-launch support
#  --disable-Werror        compile without -Werror (normally enabled in
#                          development builds)
#  --disable-stats         disable bus daemon usage statistics
#
#Optional Packages:
#  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
#  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
#  --with-pic[=PKGS]       try to use only PIC/non-PIC objects [default=use
#                          both]
#  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]
#  --with-sysroot=DIR Search for dependent libraries within DIR
#                        (or the compiler's sysroot if not specified).
#  --with-init-scripts=redhat
#                          Style of init scripts to install
#  --with-session-socket-dir=dirname
#                          Where to put sockets for the per-login-session
#                          message bus
#  --with-test-socket-dir=dirname
#                          Where to put sockets for make check
#  --with-system-pid-file=pidfile
#                          PID file for systemwide daemon
#  --with-system-socket=filename
#                          UNIX domain socket for systemwide daemon
#  --with-console-auth-dir=dirname
#                          directory to check for console ownerhip
#  --with-console-owner-file=filename
#                          file whose owner determines current console owner
#  --with-launchd-agent-dir=dirname
#                          directory to put the launchd agent (default:
#                          /Library/LaunchAgents)
#  --with-dbus-user=<user> User for running the DBUS daemon (messagebus)
#  --with-dbus-daemondir=dirname
#                          Directory for installing the DBUS daemon
#  --with-dbus-glib        Use dbus-glib for regression tests
#  --with-valgrind         Add instrumentation to help valgrind to understand
#                          our allocator
#  --with-x                use the X Window System
#  --with-systemdsystemunitdir=DIR
#                          Directory for systemd service files
#  --with-dbus-test-dir=dirname
#                          path where the tests tools are available
#  --with-dbus-session-bus-listen-address=ADDRESS
#                          default address for a session bus to listen on (see
#                          configure.ac)
#  --with-dbus-session-bus-connect-address=ADDRESS
#                          fallback address for a session bus client to connect
#                          to (see configure.ac)

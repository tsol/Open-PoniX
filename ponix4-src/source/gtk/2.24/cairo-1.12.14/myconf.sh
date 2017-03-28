#!/bin/sh

./configure --prefix=/usr --x-includes=/usr/xorg/include --x-libraries=/usr/xorg/lib --enable-quartz=no \
--enable-quartz-font=no --enable-quartz-image=no --enable-win32=no  --enable-win32-font=no \
--enable-os2=no --enable-beos=no --enable-glitz=no --enable-directfb=no \
--enable-gtk-doc=no --enable-test-surfaces=no --with-x

#X features:
#  --x-includes=DIR    X include files are in DIR
#  --x-libraries=DIR   X library files are in DIR
#
#  --enable-xlib=[no/auto/yes]
#                          Enable cairo's Xlib backend [default=auto]
#  --enable-xlib-xrender=[no/auto/yes]
#                          Enable cairo's Xlib Xrender backend [default=auto]
#  --enable-xcb=[no/auto/yes]
#                          Enable cairo's XCB backend [default=no]
#  --enable-quartz=[no/auto/yes]
#                          Enable cairo's Quartz backend [default=auto]
#  --enable-quartz-font=[no/auto/yes]
#                          Enable cairo's Quartz font backend [default=auto]
#  --enable-quartz-image=[no/auto/yes]
#                          Enable cairo's Quartz Image backend [default=no]
#  --enable-win32=[no/auto/yes]
#                          Enable cairo's Microsoft Windows backend
#                          [default=auto]
#  --enable-win32-font=[no/auto/yes]
#                          Enable cairo's Microsoft Windows font backend
#                          [default=auto]
#  --enable-os2=[no/auto/yes]
#                          Enable cairo's OS/2 backend [default=no]
#  --enable-beos=[no/auto/yes]
#                          Enable cairo's BeOS/Zeta backend [default=no]
#  --enable-png=[no/auto/yes]
#                          Enable cairo's PNG backend [default=yes]
#  --enable-glitz=[no/auto/yes]
#                          Enable cairo's glitz backend [default=no]
#  --enable-directfb=[no/auto/yes]
#                          Enable cairo's directfb backend [default=no]
#  --enable-freetype=[no/auto/yes]
#                          Enable cairo's FreeType font backend [default=auto]
#  --disable-pthread       Do not use pthread
#  --enable-ps=[no/auto/yes]
#                          Enable cairo's PostScript backend [default=yes]
#  --enable-pdf=[no/auto/yes]
#                          Enable cairo's PDF backend [default=yes]
#  --enable-svg=[no/auto/yes]
#                          Enable cairo's SVG backend [default=yes]
#  --enable-gtk-doc        use gtk-doc to build documentation default=no
#  --enable-gcov           Enable gcov
#  --enable-test-surfaces  Add backends for more test suite coverage (no
#                          additional public functionality)
#  --disable-some-floating-point
#                          Disable certain code paths that rely heavily on
#                          double precision floating-point calculation. This
#                          option can improve performance on systems without a
#                          double precision floating-point unit, but might
#                          degrade performance on those that do.

#Optional Packages:
#  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
#  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
#  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]
#  --with-pic              try to use only PIC/non-PIC objects [default=use
#                          both]
#  --with-tags[=TAGS]      include additional configurations [automatic]
#  --with-x                use the X Window System
#  --with-html-dir=PATH    path to installed docs


# Note that this is NOT a relocatable package
%define ver      1.8.2
%define rel      SNAP
%define prefix   /usr

Summary: Image loading and rendering library for X11R6
Name: imlib
Version: %ver
Release: %rel
Copyright: LGPL
Group: X11/Libraries
Source: ftp://ftp.labs.redhat.com/pub/imlib/imlib-%{ver}.tar.gz
Obsoletes: Imlib
BuildRoot: /var/tmp/imlib-root
Packager: The Rasterman <raster@redhat.com>
URL: http://www.labs.redhat.com/imlib
Requires: libpng 
Requires: libtiff 
Requires: libjpeg
Requires: zlib 
Requires: libgr-progs 
Requires: gtk+ >= 1.1
Requires: libungif
Requires: ImageMagick 
Docdir: %{prefix}/doc

%description
Imlib is an advanced replacement library for libraries like libXpm that
provides many more features with much greater flexability and
speed.

%package devel
Summary: Imlib headers, static libraries and documentation
Group: X11/Libraries
Requires: imlib = %{PACKAGE_VERSION}
Obsoletes: Imlib

%description devel
Headers, static libraries and documentation for Imlib.

%package cfgeditor
Summary: Imlib configuration editor
Group: X11/Libraries
Requires: imlib = %{PACKAGE_VERSION}

%description cfgeditor
The imlib_config program allows you to control the way imlib uses
color and handles gamma correction/etc.

%prep
%setup

%build
##### Boring normal rpm build method
# old method of building - does not auto-detect for options in OPT_FLAGS
###########################################################################
#CFLAGS="${RPM_OPT_FLAGS}" ./configure --prefix=%prefix
#make
###########################################################################

##### High-perfromance auto-detect
# Needed for snapshot releases.
# Optimize that damned code all the way
###########################################################################
#if [ ! -z "echo -n ${RPM_OPT_FLAGS} | grep pentium" ]; then
#  if [ -z "${CC}" -a ! -z "`which egcs`" ]; then
#    CC="egcs"
#    export CC
#  else
#    if [ -z "${CC}" -a ! -z "`which pgcc`" ]; then
#      CC="pgcc"
#      export CC
#    fi
#  fi
#  CFLAGS="${RPM_OPT_FLAGS}"
#else
#  CFLAGS="${RPM_OPT_FLAGS}"
#fi
#export CFLAGS
#
if [ ! -f configure ]; then
  ./autogen.sh --prefix=%prefix
else
  ./configure --prefix=%prefix
fi

if [ "$SMP" != "" ]; then
  (make "MAKE=make -k -j $SMP"; exit 0)
  make
else
  make
fi
###########################################################################

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%doc README AUTHORS ChangeLog NEWS
%attr(755,root,root) %{prefix}/lib/lib*.so.*
%{prefix}/etc/*

%files cfgeditor
%defattr(-,root,root)
%{prefix}/bin/*

%files devel
%defattr(-,root,root)
%doc doc/*.gif doc/*.html
%{prefix}/lib/lib*.so
%{prefix}/lib/*a
%{prefix}/include/*
%{prefix}/share/aclocal/*

%changelog
* Wed Sep 23 1998 Carsten Haitzler <raster@redhat.com>
- up to 1.8.1

* Tue Sep 22 1998 Cristian Gafton <gafton@redhat.com>
- yet another build for today (%defattr and %attr in the files lists)
- devel docs are back on the spec file

* Tue Sep 22 1998 Carsten Haitzler <raster@redhat.com>
- Added minor patch for ps saving code.

* Mon Sep 21 1998 Cristian Gafton <gafton@redhat.com>
- updated to version 1.8

* Fri Sep 11 1998 Cristian Gafton <gafton@redhat.com>
- take out imlib_config from devel package

* Wed Sep 9 1998 Michael Fulbright <msf@redhat.com>
- upgraded to 1.7
- changed name so it will persist if user later install devel imlib
- added subpackage for imlib_config

* Fri Apr 3 1998 Michael K. Johnson <johnsonm@redhat.com>
- fixed typo

* Fri Mar 13 1998 Marc Ewing <marc@redhat.com>
- Added -k, Obsoletes
- Integrate into CVS source tree


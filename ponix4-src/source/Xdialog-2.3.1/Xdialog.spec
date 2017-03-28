%define name	Xdialog
%define version	2.3.1
%define release	1

Name:		%{name}
Summary:	Xdialog is a X11 drop in replacement for cdialog.
Version:	%{version}
Release:	%{release}
Source:		%{name}-%{version}.tar.bz2
Group:		X11/Administration
URL:		http://xdialog.dyns.net/
Copyright:	GPL
Packager:	Thierry Godefroy <xdialog@free.fr>
Prefix:		/usr
BuildRoot:	/var/tmp/%{name}-buildroot
BuildRequires:	gettext
Provides:	Xdialog

%description
Xdialog is designed to be a drop in replacement for the cdialog program.
It converts any terminal based program into a program with an X-windows
interface. The dialogs are easier to see and use and Xdialog adds even
more functionalities (help button+box, treeview, editbox, file selector,
range box, and much more).

%prep
rm -rf $RPM_BUILD_ROOT
%setup -q

%build
# CFLAGS="%optflags"
# ./configure
%configure
# Fix bugs in many automake versions:
RANLIBBIN=`which ranlib`
sed -e "s:RANLIB = @RANLIB@:RANLIB = $RANLIBBIN:" lib/Makefile > lib/Makefile.patched
mv -f lib/Makefile.patched lib/Makefile
XGETTEXTBIN=`which xgettext`
sed -e "s;XGETTEXT = :;XGETTEXT = $XGETTEXTBIN;" po/Makefile > po/Makefile.patched
mv -f po/Makefile.patched po/Makefile

make

%install
%makeinstall

%{find_lang} %{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0755)
%doc samples doc/*.html doc/*.png
%_mandir/man1/Xdialog.1*
%_bindir/*
%{prefix}/share/locale/*/LC_MESSAGES/Xdialog.mo

%changelog
* Fri Aug 18 2006 Thierry Godefroy <xdialog@free.fr>
        v2.3.1  - Fixed the bug, introduced in v2.3.0, where the "Hide typing"
                  check button always appeared in the input boxes, regardless
                  of the existence of a password field.

* Sun Aug 13 2006 Thierry Godefroy <xdialog@free.fr>
        v2.3.0  - Added a new --password=1|2 syntax to allow a password field
                  in one of the first fields of --2inputsbox and --3inputsbox.
                - Added optional <red> <green> <blue> parameters for the
                  --colorsel widget.
                - Corrected a bug in the --treeview widget which was failing to
                  properly build a tree when decreasing the level of an item by
                  more than 1, compared to the level of the previous item.
                - Corrected a bug in the --colorsel widget which triggered a
                  GTK warning when the --icon option was used.
                - Implemented the use of the GTK2 stock icons for the buttons
                  of the GTK2 version of Xdialog.

* Sat May 06 2006 Thierry Godefroy <xdialog@free.fr>
        v2.2.1  - Corrected the bug in the --dselect widget which was
                  returning a filename by default, instead of the directory
                  name. Also removed the file specific fields and buttons from
                  the directory selector.
                - Corrected a typo in the man page.
* Thu Jan 26 2006 Thierry Godefroy <xdialog@free.fr>
	v2.2.0  - Added preliminary GTK2 support (BEWARE: still broken !).
                - Added the --timeout transient option.
                - Fixed a problem with some patched GTK versions and the
                  --rc-file option. Still an issue left with fixed fonts
                  for XDIALOG_HIGH_DIALOG_COMPAT mode (with Mandrake's
                  GTK+ packages)...
                - Less restrictive default fixed font specification.
                - Custom log message handler to prevent GTK/GDK/GLIB from using
                  stdout as the message output.
                - Added XDIALOG_NO_GMSGS environment variable support to forbid
                  (when TRUE) emission of GTK/GDK/GLIB messages on stderr.

* Fri Jul 23 2004 Thierry Godefroy <xdialog@free.fr>
	v2.1.2  - Added the ESC keyboard shortcut for the CANCEL/NO button.
	        - Added the Indonesian, Dutch, Polish, Italian, Catalan and
                  Swedish translations.
                - Changed Xmessage and textbox samples for compatibility with
                  bash-less systems.
                - Updated for gettext v0.13.1.
                - Added fixes in the spec file for automake bugs.

* Wed Apr 02 2003 Thierry Godefroy <xdialog@free.fr>
	v2.1.1  - Fixed a bug in --calendar where an improper date (month) was
                  preset when a null parameter was passed.
                - Applied a patch to allow Xdialog to build outside the source
                  tree with VPATH.

* Tue Mar 25 2003 Thierry Godefroy <xdialog@free.fr>
	v2.1.0  - Adapted a patch allowing to pass optional <hours>, <minutes>
                  and <seconds> parameters to the --timebox.
                  Also made the <day>, <month> and <year> parameters of the
                  --calendar optional.
		- The --infobox and --gauge widgets now use non-blocking read()
		  calls instead of scanf() ones.
                - Implemented the new --colorsel and --fontsel widgets.
                - Added an optional <status> parameter to the --check option.
                - Fixed a bug which made Xdialog coredump when the first row of
                  a --menubox was set "unselectable" (NULL tag) and when the
                  "OK" button was pressed without selecting another row.
                - Fixed the charset for pt_BR.po and hu.po.
                - Applied a patch to avoid using a temporary file for printing
                  and using a pipe instead (safer, cleaner). Removed the
                  corresponding "--with-print-tmpfile" option from configure.in
                - New Makefile.am, doc/Makefile.am and configure.in. The doc
                  files are now built during the 'make all' instead of the
                  'make install' step. It should also fix the problems with
                  the bad paths to the Xdialog doc files appearing in the man
                  page on some systems.
                - Changed the URL for Xdialog website in sources and doc.
                - Documentation updates.

* Thu Apr 11 2002 Thierry Godefroy <xdialog@free.fr>
	v2.0.6	- Added the "Hide typing" check button into input boxes when
		  --password option is in use.
		- Corrected a bug where "unchecked" was not returned with
		  --check and --inputbox combination when the enter key was
		  pressed to close the box.
		- Norvegian translations added (courtessy of Jostein
		  Christoffer Andersen).

See the ChangeLog (or changelog.html) file for details about older changes...

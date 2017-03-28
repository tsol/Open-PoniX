# $Aumix: aumix/packaging/aumix.spec,v 1.4 2002/12/11 01:48:14 trevor Exp $
Name: aumix
Version: 2.7
Release: 1
Copyright: GNU GPL
Group: Applications/Sound
Source: ftp://metalab.unc.edu/pub/Linux/apps/sound/mixers/%{name}-%{version}.tar.gz
Buildroot: /var/tmp/aumix-root
Summary: curses based audio mixer
Summary(de): Audio-Mixer auf curses-Basis
Summary(fr): Mixer audio basИ sur curses.
Summary(tr): Metin ekranlЩ ses karЩЧtЩrЩcЩ
Summary(ru): Аудио микшер на базе библиотеки curses
Summary(uk): Ауд╕о м╕кшер, базований на б╕бл╕отец╕ curses
Vendor: Trevor Johnson <trevor@jpj.net>


%description
This program adjusts an audio mixer from X, the console, a terminal,
the command line or a script.

%description -l ru
Эта программа - консольный, интерактивный регулятор уровней микшера
звуковой карты. Она позволяет изменять как входные уровни сигналов с
CD, микрофона, синтезаторов на звуковой плате, так и выходной уровень.

%description -l uk
Ця програма - консольний, ╕нтерактивний регулятор р╕вней м╕кшеру
звуково╖ картки. Вона дозволя╓ зм╕нювати як вх╕дн╕ р╕вн╕ сигнал╕в з
CD, м╕крофону, синтезатор╕в на звуков╕й плат╕, так ╕ вих╕дний р╕вень.

%description -l de
Dieses Programm bietet eine interaktive Methode auf tty-Basis zur 
Steuerung eines Soundkarten-Mixers. Sie kЖnnen damit die 
Eingangspegel der CD, des Mikrophons und von Synthesizer-Karten 
sowie auch die AusgabelautstДrke regeln. 

%description -l fr
Ce programme offre une mИthode intaractive en mode texte pour contrТler
le mixer des cartes son. Il permet d'ajuster les niveaux d'entrИe du CD,
du micro et des synthИtiseurs de la carte, tout comme le volume de sortie.

%description -l tr
Bu program metin ekranda, etkileЧimli olarak ses kartЩ mixer denetimi
yapmanЩzЩ saglar. гЩktЩ sesinin yanЩsЩra, CD, mikrofon ve panel Эzerindeki
birleЧtiriciden girdi seviyelerini ayarlamanЩza olanak verir.

%prep
%setup
%build
%configure --without-alsa
make

%install
rm -rf $RPM_BUILD_ROOT
make install prefix=$RPM_BUILD_ROOT/usr
make prefix=$RPM_BUILD_ROOT/usr install -C po
strip -s $RPM_BUILD_ROOT/usr/bin/aumix

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README TODO NEWS ChangeLog
/usr/bin/aumix
/usr/bin/mute
/usr/bin/xaumix
/usr/man/man1/aumix.1
/usr/man/man1/mute.1
/usr/man/man1/xaumix.1
/usr/share/locale/*/*/*
/usr/share/aumix/ansi
/usr/share/aumix/aumix.xpm
/usr/share/aumix/vt100
/usr/share/aumix/xterm
/usr/share/aumix/fadein.set
/usr/share/aumix/fadeout.set

%changelog
* Wed Dec 11 2002 Trevor Johnson <trevor@jpj.net>
- add man page for mute script

* Tue Mar 20 2002 Olexander Kunytsa <kunia@istc.kiev.ua>
- fix typo in Ukrainian summary

* Mon Mar 19 2002 Trevor Johnson <trevor@jpj.net>
- add mute script

* Thu Jul 13 2000 Trevor Johnson <trevor@jpj.net>
- version 2.7

* Fri May 26 2000 Trevor Johnson <trevor@jpj.net>
- version 2.6.1
- update English description to mention X interface
- fix packing list (Christian Weisgerber)

* Wed Apr 12 2000 Trevor Johnson <trevor@jpj.net>
- version 2.6
- removed aumix-mini.xpm
- added examples/fadein and examples/fadeout

* Wed Mar 29 2000 Trevor Johnson <trevor@jpj.net>
- version 2.5

* Thu Mar 23 2000 Trevor Johnson <trevor@jpj.net>
- version 2.4

* Thu Mar 16 2000 Trevor Johnson <trevor@jpj.net>
- version 2.3

* Tue Mar  7 2000 Trevor Johnson <trevor@jpj.net>
- version 2.2

* Mon Jan 31 2000 Trevor Johnson <trevor@jpj.net>
- version 2.1   

* Sun Dec 19 1999 Trevor Johnson <trevor@jpj.net>
- version 1.30.1

* Mon Dec  6 1999 Trevor Johnson <trevor@jpj.net>
- version 1.30
- add xaumix man page

* Sat Nov 27 1999 Trevor Johnson <trevor@jpj.net>
- version 1.29
- change "./configure --prefix=/usr --without-alsa" to
  "%configure --without-alsa"

* Sat Nov 27 1999 Trevor Johnson <trevor@jpj.net>
- version 1.28

* Wed Nov 24 1999 Trevor Johnson <trevor@jpj.net>
- version 1.27.4

* Tue Nov 23 1999 Trevor Johnson <trevor@jpj.net>
- version 1.27.3

* Fri Nov 19 1999 Trevor Johnson <trevor@jpj.net>
- version 1.27.2

* Sat Nov 13 1999 Trevor Johnson <trevor@jpj.net>
- version 1.27.1

* Mon Nov  8 1999 Trevor Johnson <trevor@jpj.net>
- version 1.27

* Wed Nov  3 1999 Trevor Johnson <trevor@jpj.net>
- version 1.26

* Mon Nov  1 1999 Trevor Johnson <trevor@jpj.net>
- version 1.25.1 (missed another)
- strip symbols too (strip -s)
- add xaumix, schemes, and icons to files

* Tue Oct 26 1999 Trevor Johnson <trevor@jpj.net>
- version 1.24 (missed a couple more)
- changed sunsite.unc.edu to metalab.unc.edu

* Tue Jun  8 1999 Trevor Johnson <trevor@jpj.net>
- version 1.21 (whoops, missed one)

* Mon May 31 1999 Trevor Johnson <trevor@jpj.net>
- version 1.19

* Thu May 20 1999 Trevor Johnson <trevor@jpj.net>
- updated to 1.18.4

* Tue May 5 1999 Trevor Johnson <trevor@jpj.net>
- updated to 1.18.3

* Fri Feb  5 1999 Trevor Johnson <trevor@jpj.net>
- added to sources for version 1.16

* Fri Dec 18 1998 Bill Nottingham <notting@redhat.com>
- update to 1.14

* Sat Oct 10 1998 Cristian Gafton <gafton@redhat.com>
- strip binary

* Fri Oct  2 1998 Bill Nottingham <notting@redhat.com>
- updated to 1.13

* Fri Aug 28 1998 Bill Nottingham <notting@redhat.com>
- updated to 1.12

* Mon Aug 17 1998 Bill Nottingham <notting@redhat.com>
- updated to 1.11

* Tue May 05 1998 Prospector System <bugs@redhat.com>
- translations modified for de, fr, tr

* Thu Apr 09 1998 Cristian Gafton <gafton@redhat.com>
- updated to 1.8

* Tue Oct 21 1997 Otto Hammersmith <otto@redhat.com>
- fixed source url
- updated version

* Thu Jul 10 1997 Erik Troan <ewt@redhat.com>
- built with glibc

* Thu Mar 20 1997 Michael Fulbright <msf@redhat.com>
- Updated to v. 1.6.1.

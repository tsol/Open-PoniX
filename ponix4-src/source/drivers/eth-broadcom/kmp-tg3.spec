Name     : tg3
Version  : 3.134f
Release  : 2.rhel5u10
Group    : System Environment/Base
Vendor   : Broadcom Corporation
URL	 : http://www.broadcom.com
License  : GPL
Source0  : %{name}-%{version}.tar.gz
Source10 : find-requires.ksyms
Source11 : symbols.greylist
Source20 : tg3.files
Source21 : tg3.conf
Source22 : kmodtool

BuildRoot: %{_tmppath}/%{name}-buildroot
BuildRequires: %kernel_module_package_buildreqs
Summary  : HP NC-Series Broadcom NetXtreme Gigabit Ethernet Driver

%define packinfo "This package contains the Broadcom NetXtreme Gigabit ethernet driver."
%define sourceinfo Users can fetch the source for this driver from http://go.broadcom.com/ethernetnic_tg3_rpms
%define gpginfo GPG Key: http://go.broadcom.com/ethernetnic_packagingkey
%define docsinfo "Documentation for the Broadcom NetXtreme Gigabit ethernet driver for HP ProLiant systems."
%define debug_package %{nil}

%define __find_requires %_sourcedir/find-requires.ksyms

%define kmodtool %{-s*}%{!-s:/usr/lib/rpm/redhat/kmodtool}

%if %{!?KVER:1}%{?KVER:0}
%define flav ""
%define kverrel %(%{kmodtool} verrel %{?kernel_version} 2>/dev/null)
%endif
%if %{!?KVER:0}%{?KVER:1}
%define flav %(echo %{KVER} | awk -F"el5" '{print $2}')
%define kverrel %(echo %{KVER} | awk -F"el5" '{print $1"el5"}')
%endif

%define default %(rpm -q kernel-devel-%{kverrel}|head -n 1)
%define xen %(rpm -q kernel-xen-devel-%{kverrel}|head -n 1)
%define pae %(rpm -q kernel-PAE-devel-%{kverrel}|head -n 1)

%kernel_module_package %flav -f %{SOURCE20} -s %{SOURCE22}

%description
%{packinfo}
%{sourceinfo}
%{gpginfo}

%if "%{default}" == "kernel-devel-%{kverrel}"
%description   -n kmod-%name
This package provides the %name kernel modules built for the Linux
kernel %(echo `uname -r`) for the %_target_cpu family of processors.
%{sourceinfo}
%{gpginfo}
%endif

%if "%{xen}" == "kernel-xen-devel-%{kverrel}"
%description   -n kmod-%name-xen
This package provides the %name kernel modules built for the Linux
kernel %(echo `uname -r`)-xen for the %_target_cpu family of processors.
%{sourceinfo}
%{gpginfo}
%endif

%if "%{pae}" == "kernel-PAE-devel-%{kverrel}"
%description   -n kmod-%name-PAE
This package provides the %name kernel modules built for the Linux
kernel %(echo `uname -r`)-PAE for the %_target_cpu family of processors.
%{sourceinfo}
%{gpginfo}
%endif

%package doc
Summary: Documentation for the Broadcom NetXtreme Gigabit ethernet driver
Group: System/Kernel

%description doc
%{docsinfo}

%prep
%setup 
set -- *
mkdir source
mv "$@" source/
mkdir obj

%build
export KVER=%{kverrel}
export KABIRPM=1
export EXTRA_CFLAGS='-DVERSION=\"%version\"'

%if %{!?KVER:0}%{?KVER:1}
%define flavors %{flav}
%else
%define flavors %{flavors_to_build}
%endif

for flavor in %{flavors}; do
  rm -rf obj/$flavor
  cp -r source obj/$flavor
  export LINUXSRC=%{kernel_source $flavor}
  if [ $flavor == "default" ]; then
  	kernel_flavor=${KVER}
  else
  	kernel_flavor=${KVER}${flavor}
  fi
  make -C obj/$flavor KVER=$kernel_flavor
done

%install
export KVER=%{kverrel}
export KABIRPM=1
export EXTRA_CFLAGS='-DVERSION=\"%version\"'

%if %{!?KVER:0}%{?KVER:1}
%define flavors %{flav}
%else
%define flavors %{flavors_to_build}
%endif

for flavor in %{flavors}; do
  export LINUXSRC=%{kernel_source $flavor}
  if [ $flavor == "default" ]; then
  	kernel_flavor=${KVER}
  else
  	kernel_flavor=${KVER}${flavor}
  fi
  dest_dir=${RPM_BUILD_ROOT}/lib/modules/$kernel_flavor/extra/%{name}
  
  mkdir -p ${dest_dir}
  install -m 444 obj/$flavor/tg3.ko ${dest_dir}
done

install -m 644 -D %{SOURCE21} ${RPM_BUILD_ROOT}/etc/depmod.d/tg3.conf

man_dir=${RPM_BUILD_ROOT}/usr/share/man/man4
mkdir -p ${man_dir}
gzip -c source/tg3.4 > ${man_dir}/tg3.4.gz

%clean
rm -rf %{buildroot}

%files doc
%defattr(-, root, root)
%doc source/README.TXT 
%_mandir/man4/tg3.4.gz

%define major_change (Severity:Critical)
%define medium_change (Severity:Medium)
%define minor_change (Severity:Minor)

%changelog
* Fri Dec 13 2013 Broadcom Build Environment
-Add RH5.10 support.
-Add RH6.5 support.
-Linux : 5725 failed to enable EEE function. (70347)
-Unable to wakeup from S3/S4 sleep mode. (70369)
-Unexpected system shutdown when reload drivers. (70501)
-tg3 ISO Driver has "Unknown symbol in Module" message on SUSE10.4. (70526)
* Fri Jun 28 2013 Broadcom Build Environment
-FW upgrade not possible when NIC ethernet interface is not up (61314)
-5719 rx_fcs_errors randomly increment when connected to  Dell power connect 6224 (66991)
-5719 shows link when cable is NOT plugged in (67200)
-BCM5720 unknown messages show in lspci on RHEL6.4 beta (67849)
-Changes in flow control not reflected in OEM specific system stats when using Rhel6.2_x64 (66411)
-The system fails to wake up from S1 by magic packet. (66759)


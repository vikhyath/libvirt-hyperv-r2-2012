%{?mingw_package_header}

# Default to skipping autoreconf.  Distros can change just this one line
# (or provide a command-line override) if they backport any patches that
# touch configure.ac or Makefile.am.
%{!?enable_autotools:%define enable_autotools 0}

# The mingw build is client only.  Set up defaults for hypervisor drivers
# that talk via a native remote protocol, and for which prereq mingw
# libraries exist.
%define with_phyp          0%{!?_without_phyp:1}
%define with_esx           0%{!?_without_esx:1}
# missing libwsman, so can't build hyper-v
%define with_hyperv        0%{!?_without_hyperv:0}
%define with_xenapi        0%{!?_without_xenapi:1}
%define with_parallels           0%{!?_without_parallels:0}

# RHEL ships ESX but not PowerHypervisor, HyperV, or libxenserver (xenapi)
%if 0%{?rhel}
    %define with_phyp 0
    %define with_xenapi 0
    %define with_hyperv 0
%endif

Name:           mingw-libvirt
Version:        1.2.4
Release:        1%{?dist}%{?extra_release}
Summary:        MinGW Windows libvirt virtualization library

License:        LGPLv2+
Group:          Development/Libraries
URL:            http://libvirt.org/
Source0:        ftp://libvirt.org/libvirt/libvirt-%{version}.tar.gz

BuildRequires:  mingw32-filesystem >= 95
BuildRequires:  mingw64-filesystem >= 95
BuildRequires:  mingw32-gcc
BuildRequires:  mingw64-gcc
BuildRequires:  mingw32-binutils
BuildRequires:  mingw64-binutils
BuildRequires:  mingw32-libgpg-error
BuildRequires:  mingw64-libgpg-error
BuildRequires:  mingw32-libgcrypt
BuildRequires:  mingw64-libgcrypt
BuildRequires:  mingw32-gnutls
BuildRequires:  mingw64-gnutls
BuildRequires:  mingw32-gettext
BuildRequires:  mingw64-gettext
BuildRequires:  mingw32-libxml2
BuildRequires:  mingw64-libxml2
BuildRequires:  mingw32-portablexdr
BuildRequires:  mingw64-portablexdr

BuildRequires:  pkgconfig
# Need native version for msgfmt
BuildRequires:  gettext
%if 0%{?enable_autotools}
BuildRequires: autoconf
BuildRequires: automake
BuildRequires: gettext-devel
BuildRequires: libtool
%endif

%if %{with_phyp}
BuildRequires: mingw32-libssh2
BuildRequires: mingw64-libssh2
%endif
%if %{with_esx}
BuildRequires: mingw32-curl
BuildRequires: mingw64-curl
%endif

BuildArch:      noarch

%description
MinGW Windows libvirt virtualization library.

# Mingw32
%package -n mingw32-libvirt
Summary: %{summary}

%description -n mingw32-libvirt
MinGW Windows libvirt virtualization library.

%package -n mingw32-libvirt-static
Summary: %{summary}
Requires: mingw32-libvirt = %{version}-%{release}

%description -n mingw32-libvirt-static
MinGW Windows libvirt virtualization library, static version.

# Mingw64
%package -n mingw64-libvirt
Summary: %{summary}

%description -n mingw64-libvirt
MinGW Windows libvirt virtualization library.

%package -n mingw64-libvirt-static
Summary: %{summary}
Requires: mingw64-libvirt = %{version}-%{release}

%description -n mingw64-libvirt-static
MinGW Windows libvirt virtualization library, static version.

%{?mingw_debug_package}


%prep
%setup -q -n libvirt-%{version}

%build
%if ! %{with_phyp}
    %define _without_phyp --without-phyp
%endif

%if ! %{with_esx}
    %define _without_esx --without-esx
%endif

%if ! %{with_hyperv}
    %define _without_hyperv --without-hyperv
%endif

%if ! %{with_xenapi}
    %define _without_xenapi --without-xenapi
%endif

%if ! %{with_parallels}
    %define _without_parallels --without-parallels
%endif

%if 0%{?enable_autotools}
autoreconf -if
%endif

# XXX enable SASL in future
%mingw_configure \
  --enable-static \
  --without-xen \
  --without-qemu \
  --without-openvz \
  --without-lxc \
  --without-vbox \
  %{?_without_xenapi} \
  --without-sasl \
  --without-avahi \
  --without-polkit \
  --without-libvirtd \
  --without-uml \
  %{?_without_phyp} \
  %{?_without_esx} \
  %{?_without_hyperv} \
  --without-vmware \
  --without-parallels \
  --without-netcf \
  --without-audit \
  --without-dtrace \
  --enable-expensive-tests

%mingw_make %{?_smp_mflags}


%install
%mingw_make_install "DESTDIR=$RPM_BUILD_ROOT"

# Libtool files don't need to be bundled
find $RPM_BUILD_ROOT -name "*.la" -delete

rm -rf $RPM_BUILD_ROOT%{mingw32_sysconfdir}/libvirt/nwfilter
rm -rf $RPM_BUILD_ROOT%{mingw64_sysconfdir}/libvirt/nwfilter
rm -rf $RPM_BUILD_ROOT%{mingw32_datadir}/doc/*
rm -rf $RPM_BUILD_ROOT%{mingw64_datadir}/doc/*
rm -rf $RPM_BUILD_ROOT%{mingw32_datadir}/gtk-doc/*
rm -rf $RPM_BUILD_ROOT%{mingw64_datadir}/gtk-doc/*

rm -rf $RPM_BUILD_ROOT%{mingw32_libexecdir}/libvirt_iohelper.exe
rm -rf $RPM_BUILD_ROOT%{mingw64_libexecdir}/libvirt_iohelper.exe
rm -rf $RPM_BUILD_ROOT%{mingw32_libexecdir}/libvirt-guests.sh
rm -rf $RPM_BUILD_ROOT%{mingw64_libexecdir}/libvirt-guests.sh


# Mingw32
%files -n mingw32-libvirt
%dir %{mingw32_sysconfdir}/libvirt/
%config(noreplace) %{mingw32_sysconfdir}/libvirt/libvirt.conf

%{mingw32_bindir}/libvirt-0.dll
%{mingw32_bindir}/virsh.exe
%{mingw32_bindir}/virt-xml-validate
%{mingw32_bindir}/virt-pki-validate
%{mingw32_bindir}/virt-host-validate.exe
%{mingw32_bindir}/libvirt-lxc-0.dll
%{mingw32_bindir}/libvirt-qemu-0.dll

%{mingw32_libdir}/libvirt.dll.a
%{mingw32_libdir}/pkgconfig/libvirt.pc
%{mingw32_libdir}/libvirt-lxc.dll.a
%{mingw32_libdir}/libvirt-qemu.dll.a

%dir %{mingw32_datadir}/libvirt/
%dir %{mingw32_datadir}/libvirt/schemas/
%{mingw32_datadir}/libvirt/schemas/basictypes.rng
%{mingw32_datadir}/libvirt/schemas/capability.rng
%{mingw32_datadir}/libvirt/schemas/domain.rng
%{mingw32_datadir}/libvirt/schemas/domaincommon.rng
%{mingw32_datadir}/libvirt/schemas/domainsnapshot.rng
%{mingw32_datadir}/libvirt/schemas/interface.rng
%{mingw32_datadir}/libvirt/schemas/network.rng
%{mingw32_datadir}/libvirt/schemas/networkcommon.rng
%{mingw32_datadir}/libvirt/schemas/nodedev.rng
%{mingw32_datadir}/libvirt/schemas/nwfilter.rng
%{mingw32_datadir}/libvirt/schemas/secret.rng
%{mingw32_datadir}/libvirt/schemas/storagecommon.rng
%{mingw32_datadir}/libvirt/schemas/storagepool.rng
%{mingw32_datadir}/libvirt/schemas/storagevol.rng
%dir %{mingw32_datadir}/libvirt/api/
%{mingw32_datadir}/libvirt/api/libvirt-api.xml
%{mingw32_datadir}/libvirt/api/libvirt-lxc-api.xml
%{mingw32_datadir}/libvirt/api/libvirt-qemu-api.xml

%{mingw32_datadir}/libvirt/cpu_map.xml
%{mingw32_datadir}/libvirt/libvirtLogo.png

%{mingw32_datadir}/locale/*/LC_MESSAGES/libvirt.mo

%dir %{mingw32_includedir}/libvirt
%{mingw32_includedir}/libvirt/libvirt.h
%{mingw32_includedir}/libvirt/virterror.h
%{mingw32_includedir}/libvirt/libvirt-lxc.h
%{mingw32_includedir}/libvirt/libvirt-qemu.h

%{mingw32_mandir}/man1/virsh.1*
%{mingw32_mandir}/man1/virt-xml-validate.1*
%{mingw32_mandir}/man1/virt-pki-validate.1*
%{mingw32_mandir}/man1/virt-host-validate.1*

%files -n mingw32-libvirt-static
%{mingw32_libdir}/libvirt.a
%{mingw32_libdir}/libvirt-lxc.a
%{mingw32_libdir}/libvirt-qemu.a

# Mingw64
%files -n mingw64-libvirt
%dir %{mingw64_sysconfdir}/libvirt/
%config(noreplace) %{mingw64_sysconfdir}/libvirt/libvirt.conf

%{mingw64_bindir}/libvirt-0.dll
%{mingw64_bindir}/virsh.exe
%{mingw64_bindir}/virt-xml-validate
%{mingw64_bindir}/virt-pki-validate
%{mingw64_bindir}/virt-host-validate.exe
%{mingw64_bindir}/libvirt-lxc-0.dll
%{mingw64_bindir}/libvirt-qemu-0.dll

%{mingw64_libdir}/libvirt.dll.a
%{mingw64_libdir}/pkgconfig/libvirt.pc
%{mingw64_libdir}/libvirt-lxc.dll.a
%{mingw64_libdir}/libvirt-qemu.dll.a

%dir %{mingw64_datadir}/libvirt/
%dir %{mingw64_datadir}/libvirt/schemas/
%{mingw64_datadir}/libvirt/schemas/basictypes.rng
%{mingw64_datadir}/libvirt/schemas/capability.rng
%{mingw64_datadir}/libvirt/schemas/domain.rng
%{mingw64_datadir}/libvirt/schemas/domaincommon.rng
%{mingw64_datadir}/libvirt/schemas/domainsnapshot.rng
%{mingw64_datadir}/libvirt/schemas/interface.rng
%{mingw64_datadir}/libvirt/schemas/network.rng
%{mingw64_datadir}/libvirt/schemas/networkcommon.rng
%{mingw64_datadir}/libvirt/schemas/nodedev.rng
%{mingw64_datadir}/libvirt/schemas/nwfilter.rng
%{mingw64_datadir}/libvirt/schemas/secret.rng
%{mingw64_datadir}/libvirt/schemas/storagecommon.rng
%{mingw64_datadir}/libvirt/schemas/storagepool.rng
%{mingw64_datadir}/libvirt/schemas/storagevol.rng
%dir %{mingw64_datadir}/libvirt/api/
%{mingw64_datadir}/libvirt/api/libvirt-api.xml
%{mingw64_datadir}/libvirt/api/libvirt-lxc-api.xml
%{mingw64_datadir}/libvirt/api/libvirt-qemu-api.xml

%{mingw64_datadir}/libvirt/cpu_map.xml
%{mingw64_datadir}/libvirt/libvirtLogo.png

%{mingw64_datadir}/locale/*/LC_MESSAGES/libvirt.mo

%dir %{mingw64_includedir}/libvirt
%{mingw64_includedir}/libvirt/libvirt.h
%{mingw64_includedir}/libvirt/virterror.h
%{mingw64_includedir}/libvirt/libvirt-lxc.h
%{mingw64_includedir}/libvirt/libvirt-qemu.h

%{mingw64_mandir}/man1/virsh.1*
%{mingw64_mandir}/man1/virt-xml-validate.1*
%{mingw64_mandir}/man1/virt-pki-validate.1*
%{mingw64_mandir}/man1/virt-host-validate.1*

%files -n mingw64-libvirt-static
%{mingw64_libdir}/libvirt.a
%{mingw64_libdir}/libvirt-lxc.a
%{mingw64_libdir}/libvirt-qemu.a


%changelog

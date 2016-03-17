%bcond_with wayland
%bcond_with x

Name: e-tizen-testcase
Version: 0.0.7
Release: 1
Summary: The test case runner for enlightenment
URL: http://www.enlightenment.org
Group: Graphics & UI Framework/Other
Source0: %{name}-%{version}.tar.gz
License: BSD-2-Clause
BuildRequires: pkgconfig(enlightenment)
BuildRequires: pkgconfig(eldbus)
BuildRequires: pkgconfig(capi-ui-efl-util)
BuildRequires: gettext
%if %{with wayland}
BuildRequires: pkgconfig(tizen-extension-client)
%endif

%global TZ_SYS_RO_SHARE  %{?TZ_SYS_RO_SHARE:%TZ_SYS_RO_SHARE}%{!?TZ_SYS_RO_SHARE:/usr/share}

%description
This package is a test case runner for enlightenment.

%prep
%setup -q

%build

export GC_SECTIONS_FLAGS="-fdata-sections -ffunction-sections -Wl,--gc-sections"
export CFLAGS+=" -Wall -g -fPIC -rdynamic ${GC_SECTIONS_FLAGS}"
export LDFLAGS+=" -Wl,--hash-style=both -Wl,--as-needed -Wl,--rpath=/usr/lib"

%autogen
%configure --prefix=/usr \
%if %{with x}
      --with-x11
%endif
%if %{with wayland}
      --with-wayland
%endif
make %{?_smp_mflags}

%install
rm -rf %{buildroot}

# for license notification
mkdir -p %{buildroot}/%{TZ_SYS_RO_SHARE}/license
cp -a %{_builddir}/%{buildsubdir}/COPYING %{buildroot}/%{TZ_SYS_RO_SHARE}/license/%{name}

# install
make install DESTDIR=%{buildroot}

%files
%defattr(-,root,root,-)
%{TZ_SYS_RO_SHARE}/license/%{name}
%{_bindir}/e_test_runner

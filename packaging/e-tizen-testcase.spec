Name: e-tizen-testcase
Version: 0.0.1
Release: 1
Summary: The test case module for enlightenment
URL: http://www.enlightenment.org
Group: Graphics & UI Framework/Other
Source0: %{name}-%{version}.tar.gz
License: BSD-2-Clause
BuildRequires: pkgconfig(enlightenment)
BuildRequires: pkgconfig(eldbus)
BuildRequires: pkgconfig(capi-ui-efl-util)
BuildRequires:  gettext

%description
This package is a test case module for enlightenment.

%prep
%setup -q

%build

export GC_SECTIONS_FLAGS="-fdata-sections -ffunction-sections -Wl,--gc-sections"
export CFLAGS+=" -Wall -g -fPIC -rdynamic ${GC_SECTIONS_FLAGS}"
export LDFLAGS+=" -Wl,--hash-style=both -Wl,--as-needed -Wl,--rpath=/usr/lib"

%autogen
%configure --prefix=/usr
make %{?_smp_mflags}

%install
rm -rf %{buildroot}

# for license notification
mkdir -p %{buildroot}/usr/share/license
cp -a %{_builddir}/%{buildsubdir}/COPYING %{buildroot}/usr/share/license/%{name}

# install
make install DESTDIR=%{buildroot}

%files
%defattr(-,root,root,-)
/usr/share/license/%{name}
%{_bindir}/e_test_case

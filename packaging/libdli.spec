%bcond_with wayland

Name:       libdli
Summary:    DLI scene loading library
Version:    0.1.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://github.com/dalihub/libdli.git
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dlog)
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  pkgconfig(dali2-toolkit)
BuildRequires:  pkgconfig(libtzplatform-config)

%description
Provides functionality for loading and displaying DLI format scenes. See README.md for more details.

%package devel
Summary:    Development components for libdli
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}

%description devel
Development components for libdli.

##############################
# Preparation
##############################
%prep
%setup -q

#Use TZ_PATH when tizen version is >= 3.x

%define dali_xml_file_dir     %TZ_SYS_RO_PACKAGES
%define smack_rule_dir        %TZ_SYS_SMACK/accesses2.d/

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -O2"
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed"

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_"
%endif

cd %{_builddir}/%{name}-%{version}/build/tizen

cmake \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      .

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen
%make_install

mkdir -p %{buildroot}%{dali_xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{name}.xml %{buildroot}%{dali_xml_file_dir}

%if 0%{?enable_dali_smack_rules} && !%{with wayland}
mkdir -p %{buildroot}%{smack_rule_dir}
cp -f %{_builddir}/%{name}-%{version}/%{name}.rule %{buildroot}%{smack_rule_dir}
%endif

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

##############################
# Files in Binary Packages
##############################

%files
%if 0%{?enable_dali_smack_rules}
%manifest %{name}.manifest-smack
%else
%manifest %{name}.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdli.so
%{dali_xml_file_dir}/%{name}.xml
%license LICENSE

%files devel
%defattr(-,root,root,-)
%{_includedir}/libdli/*

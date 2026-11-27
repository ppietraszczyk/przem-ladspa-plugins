#
# spec file for package przem-ladspa-plugins
#
# Copyright (c) 2020 SUSE LLC
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#

%define unmangled_version 0.261126-0
Name:           przem-ladspa-plugins
Version:	    0.261126
Release:        0
Summary:    	Completely simple LADSPA filters
License:        GPL3
URL:            http://prymula.ct8.pl
Source0:	%{name}-%{unmangled_version}.tar.gz
%if 0%{?suse_version} > 1600
BuildRequires:	pkgconfig, gcc-c++
%endif
%if 0%{?sle_version} >= 150400 && 0%{?is_opensuse}
BuildRequires:	pkgconfig, gcc-c++
%endif
%if 0%{?fedora} >= 37
BuildRequires:	pkgconfig, g++
%endif
BuildArch:	x86_64

%global debug_package %{nil}

%description
Completely simple LADSPA filters
 * The first plugin is a filter that implements the Low-Shelf (Biquad) filter.

%prep
%setup -n %{name}-%{unmangled_version}

%build
make

%make_install
make DESTDIR="%{buildroot}" install 
 
%files
%defattr(0755,root,root)
/usr/lib64/ladspa
/usr/lib64/ladspa/przem-low-shelf.so

%changelog
* Wed Nov 24 19:49:00 UTC 2026 - Przemysław Pietraszczyk <przem@prymula.ct8.pl>
* Create package 0.261124-0 for openSUSE:

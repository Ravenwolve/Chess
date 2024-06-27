%define _unpackaged_files_terminate_build 1

Name: xitchess
Version: 0.1.0
Release: alt1

Summary: Chess
License: %gpl2plus
Group: Other
Url: https://github.com/Ravenwolve/Chess
BuildArch: x86_64

BuildRequires(pre): rpm-build-licenses rpm-macros-cmake
BuildRequires: cmake gcc13 gcc-c++

Source0: %name-%version.tar

%description
Open-source chess by student from science work.

%prep
%setup -q

%build
%cmake
%cmake_build

%install
install -m 755 -D %_cmake__builddir/Chess %buildroot%_bindir/xitchess

%files
%_bindir/xitchess

%changelog
* Fri Jun 28 2024 Kirill Sharov <sheriffkorov@altlinux.org> 0.1.0-alt1
- Initial build.

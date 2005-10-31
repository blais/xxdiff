Summary: a graphical front end to the diff command
Name: xxdiff
Version: 1.11.0
Release: 2
Copyright: GNU GPL
Group: Development/Tools
Source: http://prdownloads.sourceforge.net/xxdiff/xxdiff-%{version}.tar.gz
URL: http://xxdiff.sourceforge.net
Packager: Martin Blais <blais@iro.umontreal.ca>
Buildroot: %{_tmppath}/%{name}-%{version}-root
Requires: qt
BuildRequires: qt-devel

%description
xxdiff is a graphical browser for viewing the differences between two
files and can be used to produce a merged version.  The text of the
two or three files are presented side by side with their differences
highlighted for easy identification.

%prep
%setup -q

%build
# Redo automoc before compiling because 'make dist' ignores automoc.
perl ./automoc -v
%configure
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

mkdir -p ${RPM_BUILD_ROOT}/usr/X11R6/bin
mkdir -p ${RPM_BUILD_ROOT}/usr/X11R6/man/man1

install -c -m 755 -s src/xxdiff ${RPM_BUILD_ROOT}/usr/X11R6/bin/
install -c -m 644 src/xxdiff.1 ${RPM_BUILD_ROOT}/usr/X11R6/man/man1/

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%doc README
%doc CHANGES

/usr/X11R6/bin/xxdiff
/usr/X11R6/man/man1/xxdiff.1*


%changelog
* Wed May 30 2001 Peder Stray <peder@linpro.no>
- fixed .spec-file

* Tue May 29 2001 Martin Blais <blais@discreet.com> 1.11.0-1
- 1.11.0

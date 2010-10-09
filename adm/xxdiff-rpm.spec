Summary: A graphical front end to the diff command
Name: xxdiff
Version: 3.2
Release: 2
License: GNU GPL
Group: Development/Tools
Source: http://prdownloads.sourceforge.net/xxdiff/xxdiff-%{version}.tar.bz2
URL: http://furius.ca/xxdiff/
Packager: Martin Blais <blais@furius.ca>
Buildroot: %{_tmppath}/%{name}-%{version}-root
Requires: qt >= 3.2, python
BuildRequires: qt-devel >= 3.2, %{?_with_tmake:tmake >= 1.7}, flex >= 2.5.31, bison, python

%description
xxdiff is a graphical browser for viewing the differences between two
files and can be used to produce a merged version.  The text of the
two or three files are presented side by side with their differences
highlighted for easy identification.

%package tools
Summary: tools for xxdiff
Group: Development/Tools
URL: http://furius.ca/xxdiff/doc/xxdiff-scripts.html

%description tools
tools for xxdiff

%prep
%setup -q

%build
if [ -z "$QTDIR" ]; then 
	. /etc/profile.d/qt.sh
fi

cd src
%{!?_with_tmake:make -f Makefile.bootstrap makefile}
%ifarch x86_64
qmake -o Makefile.qmake xxdiff.pro
cat Makefile.qmake Makefile.extra > Makefile
%else
qmake -o Makefile.qmake xxdiff.pro
cat Makefile.qmake Makefile.extra > Makefile
%endif

make
cd ..
bin/xxdiff --help-html > xxdiff-doc.html

# tools
CFLAGS="$RPM_OPT_FLAGS" python setup.py build

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

mkdir -p ${RPM_BUILD_ROOT}%{_bindir}
mkdir -p ${RPM_BUILD_ROOT}%{_mandir}/man1

install -c -m 755 -s bin/xxdiff ${RPM_BUILD_ROOT}%{_bindir}/
install -c -m 644 src/xxdiff.1 ${RPM_BUILD_ROOT}%{_mandir}/man1/xxdiff.1

# tools
%ifarch x86_64
# dodgy hack for x86_64
pyver=`python -V 2>&1 | cut -d' ' -f2 | cut -d. -f-2`
python setup.py install --root=$RPM_BUILD_ROOT --record=INSTALLED_FILES --install-lib=%{_libdir}/python${pyver}/site-packages/
%else
python setup.py install --root=$RPM_BUILD_ROOT --record=INSTALLED_FILES
%endif

%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root, root)
%doc README
%doc CHANGES
%doc xxdiff-doc.html
%{_bindir}/xxdiff
%{_mandir}/man1/xxdiff.1*

%files tools
%{_libdir}/python*/site-packages/*
%{_bindir}/svn-foreign
%{_bindir}/xx-cond-replace
%{_bindir}/xx-cvs-diff
%{_bindir}/xx-cvs-revcmp
%{_bindir}/xx-diff-proxy
%{_bindir}/xx-encrypted
%{_bindir}/xx-filter
%{_bindir}/xx-find-grep-sed
%{_bindir}/xx-match
%{_bindir}/xx-pyline
%{_bindir}/xx-rename
%{_bindir}/xx-sql-schemas
%{_bindir}/xx-svn-diff
%{_bindir}/xx-svn-resolve

%changelog
* Thu Sept 2006 Philippe Corbes <philippe.corbes@laposte.net>
- released 3.2-1 Added contextuals menus on right click

* Mon Jul 3 2006 Marcin Zajaczkowski <mszpak@wp.pl> - 3.2-1
- updated to 3.2
- default version doesn't use tmake (can be turn on by "--with tmake")

* Tue May 2 2006 Robin Humble
- fc5, x86_64 hacks

* Thu Apr 27 2006 Robin Humble
- updated to 3.2b4
- fc5, i386
- changed all paths to get rid of X11R6
- added in tools package

* Fri Nov 6 2004 Martin Blais <blais@furius.ca>
- released 3.0.4.

* Fri Nov 2 2004 Martin Blais <blais@furius.ca>
- released 3.0.3.

* Fri Jan 30 2004 Martin Blais <blais@furius.ca>
- released 3.0.2.

* Sun Jan 25 2004 Martin Blais <blais@furius.ca>
- released 3.0.1 for fixes for RedHat-8.0.

* Fri Jan 23 2004 Andre Costa <acosta@ar.microlink.com.br>
- fixed wrong relative paths

* Wed Jan 21 2004 Martin Blais <blais@furius.ca>
- released 3.0. Did not test this spec file, I do run RedHat anymore.

* Wed Jan 14 2004 Martin Blais <blais@furius.ca>
- added conditional file copy operation to support for qt2 (untested, no RH).
- took required version of qt down to qt2, not sure if we should not do a
  separate spec file instead.

* Wed Mar 17 2003 Martin Blais <blais@furius.ca>
- released 2.9.2.

* Wed Sep 20 2002 Martin Blais <blais@furius.ca>
- released 2.9.1.

* Wed Sep 19 2002 Martin Blais <blais@furius.ca>
- released 2.9.

* Wed Aug 28 2002 Martin Blais <blais@furius.ca>
- released 2.8.

* Tue Aug 10 2002 Martin Blais <blais@furius.ca>
- released 2.7.

* Tue Aug 8 2002 Martin Blais <blais@furius.ca>
- released 2.7.1.

* Tue Aug 6 2002 Martin Blais <blais@furius.ca>
- released 2.7.

* Mon Jun 3 2002 Martin Blais <blais@furius.ca>
- released 2.6.

* Thu Feb 26 2002 Martin Blais <blais@furius.ca>
- released 2.5.

* Thu Feb 23 2002 Martin Blais <blais@furius.ca>
- added documentation generation and released 2.4.

* Sat Feb 9 2002 Martin Blais <blais@furius.ca>
- removed setting of user/group for man (thanks for crazy_pete@swissinfo.org)

* Thu Feb 6 2002 Martin Blais <blais@furius.ca>
- added documentation generation and released 2.3.

* Wed Jan 30 2002 Martin Blais <blais@furius.ca>
- updated version number for interim release

* Sat Nov 24 2001 Martin Blais <blais@furius.ca>
- added generic man page install.

* Thu Nov 22 2001 Martin Blais <blais@furius.ca>
- 2.1

* Sun Nov 5 2001 Martin Blais <blais@discreet.com>
- Made it work with tmake.

* Tue Jul 24 2001 Martin Blais <blais@discreet.com>
- added defattr field, thanks to Mirko Zeibig for suggestion.

* Thu Jun  7 2001 Martin Blais <blais@discreet.com> 1.13
- released 1.13

* Thu Jun  7 2001 Martin Blais <blais@discreet.com> 1.12-2
- don't reset QTDIR if it is already set.

* Mon Jun  4 2001 Martin Blais <blais@furius.ca>
- fixed for QTDIR

* Mon Jun  4 2001 Martin Blais <blais@discreet.com> 1.12-1
- 1.12

* Wed May 30 2001 Peder Stray <peder@linpro.no>
- fixed .spec-file

* Tue May 29 2001 Martin Blais <blais@discreet.com> 1.11.0-1
- 1.11.0


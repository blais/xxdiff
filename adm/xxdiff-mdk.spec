Summary: A graphical front end to the diff command
Name: xxdiff
Version: 3.1
Release: 1
Copyright: GNU GPL
Group: Development/Tools
Source: http://prdownloads.sourceforge.net/xxdiff/xxdiff-%{version}.tar.gz
URL: http://xxdiff.sourceforge.net
Packager: Philippe Corbes <philippe.corbes@laposte.net>
Buildroot: %{_tmppath}/%{name}-%{version}-root
Requires: libqt3 >= 3.0
BuildRequires: libqt3-devel >= 3.0
BuildRequires: tmake flex bison

%description
xxdiff is a graphical browser for viewing the differences between two
files and can be used to produce a merged version.  The text of the
two or three files are presented side by side with their differences
highlighted for easy identification.

%prep
%setup -q

%build
if [ -z "$QTDIR" ]; then 
	. /etc/profile.d/qt.sh
fi

cd src
tmake xxdiff.pro > Makefile

# Copy files for qt2 if necessary.
if ! grep -q 'define.*QT_VERSION.*0x03' ${QTDIR}/include/qglobal.h ; then 
    cp qt2/*.{h,cpp} .
fi

make
cd ..
bin/xxdiff --help-html > xxdiff-doc.html

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

mkdir -p ${RPM_BUILD_ROOT}/usr/X11R6/bin
mkdir -p ${RPM_BUILD_ROOT}/usr/X11R6/man/man1

install -c -m 755 -s bin/xxdiff ${RPM_BUILD_ROOT}/usr/X11R6/bin/
install -c -m 644 src/xxdiff.1 ${RPM_BUILD_ROOT}/usr/X11R6/man/man1/xxdiff.1

%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root, root)
%doc README
%doc CHANGES
%doc xxdiff-doc.html

/usr/X11R6/bin/xxdiff
/usr/X11R6/man/man1/xxdiff.1*


%changelog
* Thu Sept 2006 Philippe Corbes <philippe.corbes@laposte.net>
- released 3.2-1 Added contextuals menus on right click 

* Thu Feb 14 2006 Philippe Corbes <philippe.corbes@laposte.net>
- released 3.1-1 Compiled for a Mandrake 9.1 with QT3

* Fri Nov 5 2004 Martin Blais <blais@furius.ca>
- released 3.1.

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
- took required version of qt down to qt2, not sure if we should not do a separate spec file instead.

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

Summary: a graphical front end to the diff command
Name: xxdiff
Version: 1.10.0
Release: 1
Copyright: GNU GPL
Group: Development/Tools
Source: xxdiff-1.10.0.tar.gz
URL: http://xxdiff.sourceforge.net
Packager: Martin Blais <blais@iro.umontreal.ca>
%description
xxdiff is a graphical browser for viewing the differences between two files and
can be used to produce a merged version.  The text of the two or three files are
presented side by side with their differences highlighted for easy
identification.


%prep
%setup

%build
# Redo automoc before compiling because 'make dist' ignores automoc.
perl ./automoc -v
./configure
make

%install
install -s -m 755 -o 0 -g 0 src/xxdiff /usr/X11R6/bin/xxdiff
install -m 644 -o 0 -g 0 src/xxdiff.1 /usr/X11R6/man/man1/xxdiff.1

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc README
%doc CHANGES

/usr/X11R6/bin/xxdiff
/usr/X11R6/man/man1/xxdiff.1

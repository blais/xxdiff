# -*- mode: Makefile -*-
#*****************************************************************************\
# $RCSfile$
#
# Copyright (C) 2001-2003  Martin Blais <blais@furius.ca>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#****************************************************************************/
#
# Generate with xxdiff.t template, as `qmake -t xxdiff.t xxdiff.pro > Makefile'
#

# Note: the version number is now in the VERSION file at the root and there is a
# special make rule defined to generate an include file for it.

TEMPLATE = app
CONFIG -= debug
CONFIG += qt warn_on thread
QT = widgets gui core

DESTDIR=../bin
TARGET = xxdiff


# FIXME: Is this needed for Windows compile?
#REQUIRES=full-config
#REQUIRES=large-config


# To enable KDE support, uncomment the following lines:
#QMAKE_CFLAGS += -DXX_KDE -I/usr/include/kde
#QMAKE_CXXFLAGS += -DXX_KDE -I/usr/include/kde
#QMAKE_LIBS += -lkdeui -lkio -lqt-mt


# Enable debugging modes within xxdiff.
debug:QMAKE_CFLAGS += -DXX_DEBUG
debug:QMAKE_CXXFLAGS += -DXX_DEBUG

# Support Debian hardening flags
QMAKE_CXXFLAGS += $$(CPPFLAGS) $$(CXXFLAGS)
QMAKE_CFLAGS += $$(CPPFLAGS) $$(CFLAGS)
QMAKE_LFLAGS += $$(LDFLAGS)


#
# Parser/grammar generation.
#
QMAKE_LEX = flex
QMAKE_YACC = bison

QMAKE_YACCFLAGS = -d -o y.tab.c
QMAKE_YACC_HEADER = y.tab.h
QMAKE_YACC_SOURCE = y.tab.c

# Don't generate unused functions (warning suppression)
QMAKE_LEXFLAGS = --noyy_push_state --noyy_pop_state --noyy_top_state

LEXSOURCES = resParser.l

YACCSOURCES = resParser.y

# We don't need to actually build the parser, we include it from resParser.o to
# isolate it in its namespace, but this means that we do need to add appropriate
# dependencies.






#===============================================================================
# platform-specific tweaks

#----------------------------------------
# irix-n32
irix-n32:QMAKE_CXXFLAGS += -woff 1375,1424,3201,1209,1110 -LANG:std

# always disable full warnings for C code.
irix-n32:QMAKE_CFLAGS_WARN_ON =
irix-n32:QMAKE_CC = cc
irix-n32:QMAKE_CFLAGS = $$QMAKE_CXXFLAGS

irix-n32:QMAKE_MOC = ${QTDIR}/bin/moc

irix-n32:QMAKE_LIBS += -lC -lCio -lm
irix-n32:QMAKE_LIBDIR_QT = $(QTDIR)/lib
irix-n32:QMAKE_LIBS += -Wl,-rpath -Wl,$(QTDIR)/lib

irix-n32:QMAKE_CFLAGS_RELEASE += -OPT:Olimit=4000


#----------------------------------------
# linux-g++

# debugging memory problems
#linux-g++:QMAKE_CXXFLAGS += -fcheck-memory-usage
#linux-g++:QMAKE_LIBS += -lmpatrol -lbfd -liberty

# auto_ptr deprecated in C++11, removed in C++17
linux: QMAKE_CXXFLAGS += -std=c++11

#----------------------------------------
# Max OS X with XFree86 port, macx-g++

# The following does not work with qmake-1.11

## macx-g++:QMAKE_CXXFLAGS += -D__GNU_LIBRARY__
## macx-g++:QMAKE_CXXFLAGS -= -fno-exceptions

#----------------------------------------
# Max OS X (macx-g++ for command line build)

macx {
   # Icon used for the application bundle
   ICON = xxdiff.icns

   # Special targets to quickly deploy a standalone mac package (just
   # containing the application) in a DMG image. Call it with "make deploy"

   BUNDLE = $$DESTDIR/$$TARGET".app"

   # Copy all required frameworks (libs) in the bundle, and remove i386 part of libs (only keep x86_64)
   macdeployqt.target = $$BUNDLE/Contents/Resources/qt.conf
   macdeployqt.commands = macdeployqt $$BUNDLE; for l in `find $$BUNDLE -type f -name '*.dylib'; find $$BUNDLE/Contents/Frameworks -type f -name 'Qt*'`; do if [ \$\$(lipo \$\$l -info | grep -c 'Non-fat') != 1 ]; then lipo \$\$l -thin x86_64 -output \$\$l; fi; done
   macdeployqt.depends = $$BUNDLE

   # Create a dmg package
   VER = $$system(cat ../VERSION)
   DMG = $$DESTDIR/$$TARGET"_"$$VER".dmg"
   dmg.target = $$DMG
   dmg.commands = @hdiutil create -ov -fs HFS+ -srcfolder $$BUNDLE -volname $$quote("xxdiff\\ $$VER") $$DMG
   dmg.depends = $$macdeployqt.target $(TARGET)

   # Crappy crap to generate and use a specific bison source file that is compatible with bison 2.3 (the default on OSX)
   bison23lnk.target = resParser_yacc.h
   bison23lnk.commands = rm -f resParser_yacc.h resParser_yacc.cpp; ln -s resParser_bison23_yacc.cpp resParser_yacc.cpp; ln -s resParser_bison23_yacc.h resParser_yacc.h
   bison23lnk.depends = bison23src resParser_bison23_yacc.h resParser_bison23.y
   bison23src.target = resParser_bison23.y
   bison23src.commands = perl -pe \'s/define api.pure/pure-parser/\' resParser.y > resParser_bison23.y
   bison23src.depends = 
   YACCSOURCES = resParser_bison23.y
   QMAKE_YACCFLAGS_MANGLE = -p resParser -b resParser
   resParser_lex_obj.target = resParser_lex.o
   resParser_lex_obj.depends = bison23lnk

   # "register" deprecated in C++11 but the MacOS flex still uses it in files it generates
   QMAKE_LEXFLAGS += -Dregister=

   # "public" rule
   deploy.depends = $$dmg.target

   QMAKE_EXTRA_TARGETS += macdeployqt dmg deploy bison23src bison23lnk resParser_lex_obj
   QMAKE_CFLAGS_RELEASE   -= -O2
   QMAKE_CXXFLAGS_RELEASE -= -O2
   QMAKE_CFLAGS   += -mdynamic-no-pic -O3 -msse2 -msse3 -mssse3 -ftree-vectorize
   QMAKE_CXXFLAGS += -mdynamic-no-pic -O3 -msse2 -msse3 -mssse3 -ftree-vectorize
}

#----------------------------------------
# win32-msvc

win32-msvc*:DEFINES += QT_DLL QT_THREAD_SUPPORT WINDOWS HAVE_STRING_H
#win32-msvc:QMAKE_CFLAGS += -GX
win32-msvc*:QMAKE_CXXFLAGS += -EHsc
win32-msvc*:INCLUDEPATH += winfixes

#win32-msvc:QMAKE_LFLAGS += /NODEFAULTLIB:MSVCRT
win32-msvc*:LIBS += winmm.lib


#===============================================================================
# source files and targets

HEADERS = \
	optionsDialog.h \
	searchDialog.h \
	app.h \
	app.inline.h \
	cmdline.h \
	suicideMessageBox.h \
	defs.h \
	types.h \
	diffs.h \
	diffs.inline.h \
	builder.h \
	builder.inline.h \
	builderFiles2.h \
	builderFiles3.h \
	builderDirs2.h \
	builderUnmerge.h \
	builderSingle.h \
	exceptions.h \
	buffer.h \
	buffer.inline.h \
	help.h \
	hordiffImp.h \
	line.h \
	line.inline.h \
	main.h \
	overview.h \
	resParser.h \
	resParser_lex.h \
	resources.h \
	resources.inline.h \
	accelUtil.h \
	copyLabel.h \
	text.h \
	text.inline.h \
	scrollView.h \
	central.h \
	merged.h \
	lineNumbers.h \
	util.h \
	markers.h \
	borderLabel.h \
	getopt.h \
	diffutils.h \
	diffutils_hack.h \
	doc.html \
	kdeSupport.h \
	proginfo.h

SOURCES = \
	optionsDialog.cpp \
	searchDialog.cpp \
	app.cpp \
	cmdline.cpp \
	suicideMessageBox.cpp \
	main.cpp \
	overview.cpp \
	copyLabel.cpp \
	text.cpp \
	scrollView.cpp \
	central.cpp \
	merged.cpp \
	lineNumbers.cpp \
	util.cpp \
	help.cpp \
	exceptions.cpp \
	builder.cpp \
	builderFiles2.cpp \
	builderFiles3.cpp \
	builderDirs2.cpp \
	builderUnmerge.cpp \
	builderSingle.cpp \
	diffs.cpp \
	hordiffImp.cpp \
	line.cpp \
	buffer.cpp \
	resources.cpp \
	accelUtil.cpp \
	resParser.cpp \
	markers.cpp \
	borderLabel.cpp \
	getopt.c \
	getopt1.c \
	proginfo.c

FORMS = \
	markersWidgetBase.ui \
	optionsDialogBase.ui \
	searchDialogBase.ui



#===============================================================================
# statically compiled diffutils experiment


# Add diff files to link against directly
# DIFFUTILS_DIR = ../diffutils-2.7

# QMAKE_LIBS +=  \
# 	$$DIFFUTILS_DIR/analyze.o \
# 	$$DIFFUTILS_DIR/cmpbuf.o \
# 	$$DIFFUTILS_DIR/dir.o \
# 	$$DIFFUTILS_DIR/io.o \
# 	$$DIFFUTILS_DIR/util.o \
# 	$$DIFFUTILS_DIR/context.o \
# 	$$DIFFUTILS_DIR/ed.o \
# 	$$DIFFUTILS_DIR/ifdef.o \
# 	$$DIFFUTILS_DIR/normal.o \
# 	$$DIFFUTILS_DIR/side.o \
# 	$$DIFFUTILS_DIR/fnmatch.o \
# 	$$DIFFUTILS_DIR/regex.o \
# 	$$DIFFUTILS_DIR/proginfo.o \
# 	$$DIFFUTILS_DIR/diff.o \
# 	$$DIFFUTILS_DIR/diff3.o

# SOURCES += diffutils.cpp

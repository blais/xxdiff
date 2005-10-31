#*****************************************************************************\
# $Id: xxdiff.pro 247 2001-10-04 01:01:13Z blais $
# $Date: 2001-10-03 21:01:13 -0400 (Wed, 03 Oct 2001) $
#
# Copyright (C) 2001  Martin Blais <blais@iro.umontreal.ca>
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
# Generate with xxdiff.t template, as `tmake -t xxdiff.t xxdiff.pro > Makefile'
#

TEMPLATE = app
CONFIG = debug qt warn_on

INCLUDEPATH += .
TMAKE_CXXFLAGS += -DPACKAGE="xxdiff" -DVERSION="1.99" -DXX_USE_RCFILE=1

LEXYACC  = rcfileParser

#  PARSER    = parser.cpp
#  SOURCES   = $$PARSER    \
#	       node.cpp    \
#	       asmgen.cpp
#  TARGET    = parser

#
# win32-msvc
#

win32-msvc:TMAKE_CXXFLAGS += -GX -DWINDOWS
win32-msvc:INCLUDEPATH += winfixes

#
# irix-n32
#

irix-n32:TMAKE_CXXFLAGS += -woff 1375,1424,3201,1209,1110 -LANG:std
irix-n32:TMAKE_CXXFLAGS += -DCOMPILER_MIPSPRO -DHAVE_DIRENT_H -DHAVE_VPRINTF

# always disable full warnings for C code.
irix-n32:TMAKE_CFLAGS_WARN_ON =
irix-n32:TMAKE_CC = cc
irix-n32:TMAKE_CFLAGS = $$TMAKE_CXXFLAGS

irix-n32:TMAKE_MOC = ${QTDIR}/bin/moc

irix-n32:TMAKE_LIBS += -lC -lCio -lgen -lm
irix-n32:TMAKE_LIBS += -Wl,-rpath -Wl,/usr/freeware/lib32

#
# linux-g++
#

linux-g++:TMAKE_CXXFLAGS += -DCOMPILER_GNU -DHAVE_DIRENT_H -DHAVE_VPRINTF

# Add diff files to link against directly
DIFFUTILS_DIR = ../diffutils-2.7

TMAKE_LIBS +=  \
	$$DIFFUTILS_DIR/analyze.o \
	$$DIFFUTILS_DIR/cmpbuf.o \
	$$DIFFUTILS_DIR/dir.o \
	$$DIFFUTILS_DIR/io.o \
	$$DIFFUTILS_DIR/util.o \
	$$DIFFUTILS_DIR/context.o \
	$$DIFFUTILS_DIR/ed.o \
	$$DIFFUTILS_DIR/ifdef.o \
	$$DIFFUTILS_DIR/normal.o \
	$$DIFFUTILS_DIR/side.o \
	$$DIFFUTILS_DIR/fnmatch.o \
	$$DIFFUTILS_DIR/regex.o \
	$$DIFFUTILS_DIR/version.o \
	$$DIFFUTILS_DIR/diff.o \
	$$DIFFUTILS_DIR/diff3.o

HEADERS = \
	optionsDialog.h \
	searchDialog.h \
	app.h \
	app.inline.h \
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
	exceptions.h \
	buffer.h \
	buffer.inline.h \
	help.h \
	line.h \
	line.inline.h \
	main.h \
	man.h \
	overview.h \
	stringResParser.h \
	rcfileParser.h \
	resources.h \
	resources.inline.h \
	text.h \
	merged.h \
	lineNumbers.h \
	util.h \
	markersFileDialog.h \
	getopt.h

SOURCES = \
	optionsDialog.cpp \
	searchDialog.cpp \
	app.cpp \
	suicideMessageBox.cpp \
	main.cpp \
	overview.cpp \
	text.cpp \
	merged.cpp \
	lineNumbers.cpp \
	util.cpp \
	help.cpp \
	exceptions.cpp \
	builder.cpp \
	builderFiles2.cpp \
	builderFiles3.cpp \
	builderDirs2.cpp \
	diffs.cpp \
	line.cpp \
	buffer.cpp \
	resources.cpp \
	stringResParser.cpp \
	rcfileParser.cpp \
	markersFileDialog.cpp \
	diffutils.cpp \
	getopt.c \
	getopt1.c

INTERFACES = \
	markersWidgetBase.ui \
	optionsDialogBase.ui \
	searchDialogBase.ui

TARGET		= xxdiff

#DEPENDPATH=../../include

#REQUIRES=large-config

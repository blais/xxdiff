# -*- mode: Makefile -*-

TEMPLATE = app
CONFIG = debug qt warn_on

INCLUDEPATH += .

#
# irix-n32
#

irix-n32:TMAKE_CXXFLAGS += -woff 1375,1424,3201,1209,1110 -LANG:std
#irix-n32:TMAKE_CXXFLAGS += -DCOMPILER_MIPSPRO

# always disable full warnings for C code.
#irix-n32:TMAKE_CFLAGS_WARN_ON =
#irix-n32:TMAKE_CC = cc
#irix-n32:TMAKE_CFLAGS = $$TMAKE_CXXFLAGS

irix-n32:TMAKE_MOC = ${QTDIR}/bin/moc

irix-n32:TMAKE_LIBS += -lC -lCio -lm
irix-n32:TMAKE_LIBDIR_QT = $(QTDIR)/lib32
irix-n32:TMAKE_LIBS += -Wl,-rpath -Wl,$(QTDIR)/lib32

# irix-n32:TMAKE_CFLAGS_RELEASE += -OPT:Olimit=4000


#
# linux-g++
#

linux-g++:TMAKE_CXXFLAGS += -DCOMPILER_GNU

HEADERS = st.h

SOURCES = st.cpp

TARGET = st


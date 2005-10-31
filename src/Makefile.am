################################################################################
# targets

bin_PROGRAMS = xxdiff



################################################################################
# designer dialogs

SUFFIXES += .ui

UIC=$(QTDIR)/bin/uic

UIDESIGNER_FILES = optionsDialogBase.ui

UIDESIGNER_MYHEADERS = $(UIDESIGNER_FILES:.ui=.h)

UIDESIGNER_MYSOURCES = $(UIDESIGNER_FILES:.ui=.cpp)

.ui.h:
	$(UIC) $< -o $@

.ui.cpp:
	$(UIC) -impl $(<:.ui=.h) $< -o $@

ui_dialogs: $(UIDESIGNER_FILES)

EXTRA_DIST += $(UIDESIGNER_FILES)



################################################################################
# sources

if XRM_RESOURCES
XRM_FILES = xrmParser.cpp
endif

if RCFILE_RESOURCES
RCFILE_FILES = rcfileParser.cpp
endif

xxdiff_SOURCES = \
	app.cpp \
	dialogs.cpp \
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
	xrmParser.cpp \
	stringResParser.cpp \
	rcfileParser.cpp \
	$(UIDESIGNER_MYSOURCES) \
	optionsDialog.cpp \
	getopt.c \
	getopt1.c


# This determines which files go in the distribution.
noinst_HEADERS = \
	app.h \
	app.inline.h \
	dialogs.h \
	defs.h \
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
	xrmParser.h \
	$(UIDESIGNER_MYHEADERS) \
	optionsDialog.h \
	getopt.h

BUILT_SOURCES =	man.h \
		$(UIDESIGNER_MYHEADERS) \
		$(UIDESIGNER_MYSOURCES)


# FIXME automoc fails when I try to optionally not compile the parser files.

man_MANS = xxdiff.man

EXTRA_DIST += xxdiff.man mine older yours pixmaps mydir yourdir old


################################################################################
# lex/yacc targets
#
# flex: use -f for fast scanner
# flex: use -d for debugging
# bison: use --debug for debugging

SUFFIXES += .y .lex

rcfileParser.lex.c rcfileParser.lex.h: \
	rcfileParser.lex \
	rcfileParser.h \
	rcfileParser.cpp
	flex -orcfileParser.lex.c rcfileParser.lex

rcfileParser.y.c: rcfileParser.y rcfileParser.h rcfileParser.cpp
	bison -d -orcfileParser.y.c rcfileParser.y

rcfileParser.o: rcfileParser.y.c rcfileParser.lex.c

EXTRA_DIST += rcfileParser.lex rcfileParser.y



################################################################################
# Qt automoc targets

xxdiff_METASOURCES = USE_AUTOMOC

CXXFLAGS += -I$(QTDIR)/include
LDFLAGS += -L$(QTDIR)/lib


################################################################################
# flags

CXXFLAGS += $(USER_INCLUDES)
LDFLAGS += $(USER_LDFLAGS) -L/usr/X11R6/lib


################################################################################
# --enable-debug option, optimized vs. debug mode
# XX_DEBUG flag is added from configure.in

if DEBUG

DEBUGFLAGS = -g

else

DEBUGFLAGS = -O3

endif
CXXFLAGS += $(DEBUGFLAGS)

################################################################################
# --enable-linkstatic option, libraries, dynamic or static

if LINKSTATIC

LINKSTATIC_LIBS = $(QTDIR)/lib/libqt.a

#LINKSTATIC_LIBS = -Wl,-Bstatic -lqt -Wl,-Bdynamic
#
# this dosesn't work under IRIX and I haven't found the way to let libtool
# specify the correct -B or -Wl,-B flags.

else

LINKSTATIC_LIBS = -lqt

endif
LIBS += $(LINKSTATIC_LIBS) -lX11 -lXext -lm

################################################################################
# --enable-runtime-help option

man.h: xxdiff.man
	echo "char manText[] = " > man.h
	nroff -man xxdiff.man | col -b | sed -e 's/\(.*\)/"\1\\\n"/' >> man.h
	echo ";" >> man.h

help.o: man.h


################################################################################
# MIPSpro options

if IS_COMPILER_MIPSPRO

# waaay too much remarks from MIPSpro 7.3's STL
#COMPILER_MIPSPRO_CXXFLAGS += -fullwarn
COMPILER_MIPSPRO_CXXFLAGS += -DCOMPILER_MIPSPRO
COMPILER_MIPSPRO_CXXFLAGS += -OPT:Olimit_opt=on

#COMPILER_MIPSPRO_CXXFLAGS += -ptnone -prelink -ptv
#COMPILER_MIPSPRO_CXXFLAGS += -no_prelink -ptused -ptv
COMPILER_MIPSPRO_CXXFLAGS += -ptv

# this is not really for CPP flags, it's just a hack not to get LANG:std into
# the linker cmd line.
COMPILER_MIPSPRO_CPPFLAGS += -LANG:std

# STL libraries
#COMPILER_MIPSPRO_LDADDA = -lC -lCio -lCsup
COMPILER_MIPSPRO_LDADDA = -lC -lCio

# for basename
COMPILER_MIPSPRO_LIBS += -lgen

MIPSSTRIPFLAGS = -s

endif
CXXFLAGS += $(COMPILER_MIPSPRO_CXXFLAGS)
CPPFLAGS += $(COMPILER_MIPSPRO_CPPFLAGS)
LDADD += $(COMPILER_MIPSPRO_LDADDA)
LIBS += $(COMPILER_MIPSPRO_LIBS)
 
if DEBUG
else
DEBUGFLAGS += $(MIPSSTRIPFLAGS)
endif

################################################################################
# SUN options

if IS_COMPILER_SUNWSPRO

COMPILER_SUNWSPRO_LDADDA = -lsocket

endif
LDADD += $(COMPILER_SUNWSPRO_LDADDA)
LIBS += $(COMPILER_SUNWSPRO_LIBS)
 
################################################################################
# Compaq Tru64 cxx options

if IS_COMPILER_COMPAQCXX

# using std namespace
COMPILER_COMPAQCXX_CXXFLAGS += -using_std

# #including <iostream>
COMPILER_COMPAQCXX_CXXFLAGS += -D__USE_STD_IOSTREAM

# To allow for proper definition of XX_THROW_NOTHING
COMPILER_COMPAQCXX_CXXFLAGS += -DCOMPILER_COMPAQCXX

endif
CXXFLAGS += $(COMPILER_COMPAQCXX_CXXFLAGS)

################################################################################
# gcc options and STLport

if IS_COMPILER_GCC

#
# for compiler
#
COMPILER_GCC_CXXFLAGS += -Wall -DCOMPILER_GNU

#
# for STLport
#
COMPILER_GCC_CXXFLAGS += -nostdinc++ -I/usr/include/stlport
COMPILER_GCC_CXXFLAGS += -D__SGI_STL_OWN_IOSTREAMS
#COMPILER_GCC_CXXFLAGS += -D__STL_USE_OWN_NAMESPACE

COMPILER_GCC_LIBS += -Wl,-Bstatic -lstlport_gcc -Wl,-Bdynamic

endif

# The following important when using gcc-c++-2.95.2 with STLport:
# You can either patch up the compiler manually by making 
#    /usr/lib/gcc-lib/i386-redhat-linux/2.95.2/libstdc++.a
# point to the stlport library, or you can link with gcc instead.
#
# The problem is that g++ insists on linking with libstdc++, which I
# would like to avoid in order to produce a more portable binary.
#
# ...but this barfs on IRIX.  Arg!  Source Carpentry, I implore you, get me out
# of this mess!  In the meantime: comment this line when building on IRIX.
#
# 2000-05-10 14:39:20 blais... it turns out that defining CXXLD within the ifdef
# cancel's its definition under IRIX, which is needed, so define it in both
# cases.

if IS_COMPILER_GCC
CXXLD=$(CC)
else
CXXLD=$(CXX)
endif

CXXFLAGS += $(COMPILER_GCC_CXXFLAGS)
LIBS += $(COMPILER_GCC_LIBS)


################################################################################
# add support for automatically-generated code documentation

SUFFIXES += .h .html

.h.html:
	cxx2html -copy $(top_srcdir)/copyright.html -dir $(top_srcdir)/html -rel $(PWD)/$<

htmlheaders=$(SOURCES:.cpp=.h)

htmldoc=$(htmlheaders:.h=.html)

$(htmldoc) : $(htmlheaders)

html: $(htmldoc)



################################################################################
# clean

CLEANFILES = *.moc.cpp ii_files/* cxx_repository/* .deps/*

DISTCLEANFILES = rcfileParser.y.c rcfileParser.y.h rcfileParser.lex.c

MAINTAINERCLEANFILES = xxdiff.catman man.h \
	$(UIDESIGNER_HEADERS) $(UIDESIGNER_SOURCES)

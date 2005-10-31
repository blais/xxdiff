# -*- mode: Makefile -*-
#############################################################################
#!
#! $RCSfile$
#!
#! This is a tmake template for building Unix applications.
#!
#$ IncludeTemplate("app.t");

#
# Compute current date/time.
#
COMPILE_DATE=#$ Now()

#
# Lex/Yacc
#
####### Lex/yacc programs and options

LEX = flex
#LEXDEBUG = -d

YACC = #$ $text = "bison -d";
#YACCDEBUG = -t --verbose

####### Lex/yacc files

LEXIN = #$ $text = Expand("LEXYACC") . ".l";
LEXOUT = #$ $text = Expand("LEXYACC") . ".l.c";
YACCIN = #$ $text = Expand("LEXYACC") . ".y";
YACCOUTC = #$ $text = Expand("LEXYACC") . ".y.c";
YACCOUTH = #$ $text = Expand("LEXYACC") . ".y.h";
PARSOBJ = #$ $text = Expand("LEXYACC") . ".o";

####### Process lex/yacc files

$(LEXOUT): $(LEXIN)
	$(LEX) $(LEXDEBUG) -o$(LEXOUT) $(LEXIN)

$(YACCOUTC) $(YACCOUTH): $(YACCIN) $(LEXOUT)
	$(YACC) $(YACCDEBUG) -o$(YACCOUTC) $(YACCIN)

$(PARSOBJ): $(YACCOUTC) $(YACCOUTH) $(LEXOUT)

#-------------------------------------------------------------------------------
# support for documentation transformation

.SUFFIXES: .qml

.qml.h: 
	sed -e 's/\"/\\\"/g;s/$$/\\\n\\/;1s/^/char text[]=\"/;$$s/\\$$/\"\;/' $< > $@
	echo 's/\"/\\\"/g;s/$$/\\\n\\/;1s/^/char text[]=\"/;$$s/\\$$/\"\;/' 

help.o: doc.h
help.obj: doc.h

# Make the version number dependent on everything, so that we always recompile
# this file whenever we link, thus updating that number.
version.o: $(HEADERS) $(SOURCES) $(INTERFACES)
version.obj: $(HEADERS) $(SOURCES) $(INTERFACES)


#
# Additional dependencies.
# Note: this is lame, find a better way to do this.  The problem is that tmake
# does note include dependencies for files it cannot find in the include path.
#
cmdline.o: $(YACCOUTH)
cmdline.obj: $(YACCOUTH)

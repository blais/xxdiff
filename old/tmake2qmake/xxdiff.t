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

.SUFFIXES: .html .txt

.txt.html:
	rst2html.py --output-encoding=iso-8859-1 $< $@

.html.h: 
	sed -e 's/\"/\\\"/g;s/$$/\\n\\/;1s/^/char text[]=\"/;$$s/\\$$/\"\;/' $< > $@

help.o: doc.h
help.obj: doc.h

# Automatically generate a simple include file from the version file.
version.h: ../VERSION
	echo "#define XX_VERSION \"`cat ../VERSION`\"" > $@

proginfo.o: version.h
proginfo.obj: version.h

#
# Additional dependencies.
# Note: this is lame, find a better way to do this.  The problem is that tmake
# does note include dependencies for files it cannot find in the include path.
#
cmdline.o: $(YACCOUTH)
cmdline.obj: $(YACCOUTH)


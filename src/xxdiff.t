# -*- mode: Makefile -*-
#############################################################################
#!
#! This is a tmake template for building Unix applications.
#!
#$ IncludeTemplate("app.t");

#
# Lex/Yacc
#
####### Lex/yacc programs and options

LEX = flex
#LEXDEBUG = -d

YACC = #$ $text = "bison -d";
#YACCDEBUG = -t --verbose

####### Lex/yacc files

LEXIN = #$ $text = Expand("LEXYACC") . ".lex";
LEXOUT = #$ $text = Expand("LEXYACC") . ".lex.c";
YACCIN = #$ $text = Expand("LEXYACC") . ".y";
YACCOUTC = #$ $text = Expand("LEXYACC") . ".y.c";
YACCOUTH = #$ $text = Expand("LEXYACC") . ".y.h";
PARSOBJ = #$ $text = Expand("LEXYACC") . ".o";

####### Process lex/yacc files

$(LEXOUT): $(LEXIN)
	$(LEX) $(LEXDEBUG) -o$(LEXOUT) $(LEXIN)

$(YACCOUTC) $(YACCOUTH): $(YACCIN) $(LEXOUT)
	$(YACC) $(YACCDEBUG) -o$(YACCOUTC) $(YACCIN)

$(PARSOBJ): $(YACCOUTC) $(LEXOUT)



#-------------------------------------------------------------------------------
# support for documentation transformation

.SUFFIXES: .qml

.qml.h: 
	sed -e 's/\"/\\\"/g;s/$$/\\\n\\/;1s/^/char text[]=\"/;$$s/\\$$/\"\;/' $< > $@

help.o: doc.h

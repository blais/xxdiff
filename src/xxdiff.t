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
YACC = #$ $text = "bison -d";

####### Lex/yacc files

LEXIN   =     #$ $text = Expand("LEXYACC") . ".lex";
LEXOUT  =     #$ $text = Expand("LEXYACC") . ".lex.c";
YACCIN  =     #$ $text = Expand("LEXYACC") . ".y";
YACCOUT =     #$ $text = Expand("LEXYACC") . ".y.c";
PARSOBJ =     #$ $text = Expand("LEXYACC") . ".o";

####### Process lex/yacc files

$(LEXOUT): $(LEXIN)
	$(LEX) -o$(LEXOUT) $(LEXIN)

$(YACCOUT): $(YACCIN) $(LEXOUT)
	$(YACC) -o$(YACCOUT) $(YACCIN)

$(PARSOBJ): $(YACCOUT) $(LEXOUT)

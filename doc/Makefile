#!/usr/bin/env make
#
# $Source$
# $Id: Makefile 766 2004-02-25 17:37:35Z blais $
#
# Automatic generation of HTML for tools.

USERMANUAL=xxdiff-doc.html

TXTDOC=						\
	xxdiff-integration.txt			\
	xxdiff-secrets.txt			\
	xxdiff-scripts.txt

TXTDOC_HTML = $(TXTDOC:.txt=.html)

TXTHTML_CONVERT=projects-rst2html --project-header  --project-home=".."

BIN=../bin

# TOOLS=						\
# 	condreplace				\
# 	findgrepsed				\
# 	cvsdiff					\
# 	diffproxy				\
# 	svndiff					\
# 	encrypted				\
# 	match					\
# 	patch

# TMPFILE=/tmp/xxdiff-doc-convert.tmp
# SCRIPTS = $(patsubst %,$(BIN)/%,$(TOOLS))
# TOOLS_HTML = $(TOOLS:=.html)

# $(TOOLS_HTML) : %.html : $(BIN)/%
# 	python-genpage -l $< -o $(TMPFILE) $<
# 	$(TXTHTML_CONVERT) $(TMPFILE) $@
# 	rm -f $(TMPFILE)


.SUFFIXES: .txt .html

all: $(USERMANUAL) $(TXTDOC_HTML)

.txt.html:
	$(TXTHTML_CONVERT) $< $@

$(USERMANUAL): $(BIN)/xxdiff
	$(BIN)/xxdiff --help-html > $@


clean:
	rm -f $(USERMANUAL)
	rm -f $(TOOLS_HTML)
	rm -f $(TXTDOC_HTML)

debug:
	@echo TOOLS
	@echo $(TOOLS)
	@echo SCRIPTS
	@echo $(SCRIPTS)
	@echo TOOLS_HTML
	@echo $(TOOLS_HTML)


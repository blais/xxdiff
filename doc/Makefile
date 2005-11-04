#!/usr/bin/env make
#
# $Source$
# $Id: Makefile 766 2004-02-25 17:37:35Z blais $
#
# Automatic generation of HTML for tools.

USERMANUAL=xxdiff-doc.html

TXTDOC=\
	xxdiff-integration.txt \
	xxdiff-secrets.txt

TXTDOC_HTML = $(TXTDOC:.txt=.html)

TXTHTML_CONVERT=projects-rst2html --project-header  --project-home=".."

TOOLS=\
	xxdiff-cond-replace \
	xxdiff-find-grep-sed \
	xxdiff-cvs-diff \
	xxdiff-subversion \
	xxdiff-encrypted \
	xxdiff-match \
	xxdiff-patch-UNFINISHED

BIN=../bin
SCRIPTS = $(patsubst %,$(BIN)/%,$(TOOLS))
TMPFILE=/tmp/xxdiff-doc-convert.tmp
TOOLS_HTML = $(TOOLS:=.html)

.SUFFIXES: .txt .html

all: $(USERMANUAL) $(TOOLS_HTML) $(TXTDOC_HTML)

.txt.html:
	$(TXTHTML_CONVERT) $< $@

$(USERMANUAL): $(BIN)/xxdiff
	$(BIN)/xxdiff --help-html > $@

$(TOOLS_HTML) : %.html : $(BIN)/%
	python-genpage -l $< -o $(TMPFILE) $<
	$(TXTHTML_CONVERT) $(TMPFILE) $@
	rm -f $(TMPFILE)

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

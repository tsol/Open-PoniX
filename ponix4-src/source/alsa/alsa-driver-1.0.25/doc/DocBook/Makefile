TOPDIR=../..

DOCBOOKS := alsa-driver-api.xml writing-an-alsa-driver.xml

include $(TOPDIR)/Makefile.conf

TOPDIR = $(MAINSRCDIR)
CURDIR = $(subst $(MAINSRCDIR)/,,$(shell /bin/pwd))

# The targets that may be used.
.PHONY:	depend xmldocs psdocs pdfdocs htmldocs clean mrproper

BOOKS := $(DOCBOOKS)
xmldocs: $(BOOKS)

PS := $(patsubst %.xml, %.ps, $(BOOKS))
psdocs: $(PS) $(DOCPROC)

PDF := $(patsubst %.xml, %.pdf, $(BOOKS))
pdfdocs: $(PDF) $(DOCPROC)

HTML := $(patsubst %.xml, %, $(BOOKS))
htmldocs: $(HTML) $(DOCPROC)

###
# non-standard rules below to simplify...

KERNELDOC=$(TOPDIR)/utils/kernel-doc
DOCPROC=$(TOPDIR)/utils/docproc

$(DOCPROC):
	$(MAKE) -C $(TOPDIR)/utils docproc

%.tmpl:
	ln -s $(TOPDIR)/alsa-kernel/Documentation/DocBook/$@

%.xml: %.tmpl
	(cd $(TOPDIR);\
	 $(DOCPROC) doc $(CURDIR)/$< > $(CURDIR)/$@)

###

DVI	:=	$(patsubst %.xml, %.dvi, $(BOOKS))
AUX	:=	$(patsubst %.xml, %.aux, $(BOOKS))
TEX	:=	$(patsubst %.xml, %.tex, $(BOOKS))
LOG	:=	$(patsubst %.xml, %.log, $(BOOKS))
OUT	:=	$(patsubst %.xml, %.out, $(BOOKS))

clean:
	rm -f core *~
	rm -f $(BOOKS) $(patsubst %.xml, %.tmpl, $(BOOKS))
	rm -f $(DVI) $(AUX) $(TEX) $(LOG) $(OUT)
	rm -f $(patsubst %.fo, %.tmpl, $(BOOKS))
	rm -f $(PNG-parportbook) $(EPS-parportbook)
	rm -f $(C-procfs-example)

mrproper: clean
	rm -f $(PS) $(PDF)
	rm -f -r $(HTML)
	rm -f .depend
#	rm -f $(TOPDIR)/scripts/mkdep-docbook
	rm -rf DBTOHTML_OUTPUT*

XMLTOFLAGS = -m $(TOPDIR)/alsa-kernel/Documentation/DocBook/stylesheet.xsl

%.html: %.xml
	xmlto xhtml $(XMLTOFLAGS) $<

# %.pdf: %.xml
# 	xmlto $(XMLTOFLAGS) pdf $<

%.fo: %.xml
	xmlto $(XMLTOFLAGS) fo $<
%.pdf: %.fo
	fop $< $@

%.ps: %.xml
	xmlto ps $(XMLTOFLAGS) $<

%: %.xml
	rm -rf $@
	mkdir $@
	(cd $@; xmlto xhtml $(XMLTOFLAGS) ../$<)

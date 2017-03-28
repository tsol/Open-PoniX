
master_PRE = masterdb.xml
sgmldbs_DATA =				\
	$(master_PRE:xml=html.xml)	\
	$(master_PRE:xml=pdf.xml)

EXTRA_DIST = $(master_PRE)
CLEANFILES = $(sgmldbs_DATA)

%.html.xml: %.xml
	$(AM_V_GEN)$(SED) \
	-e 's|__db__|html|g' \
	-e 's|@datarootdir[@]|$(datarootdir)|g' < $< > $@

%.pdf.xml: %.xml
	$(AM_V_GEN)$(SED) \
	-e 's|__db__|pdf|g' \
	-e 's|@datarootdir[@]|$(datarootdir)|g' < $< > $@

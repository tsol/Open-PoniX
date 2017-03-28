INCSRC=../../alsa-kernel/include

.PHONY: all
all:
	@echo "Nothing to do.."

ifeq (.includes,$(wildcard .includes))
include .includes
endif

version.h: ../version.h
	ln -s $<

%.h: %.patch
	cp $(INCSRC)/$@ .
	patch -p0 -i $< $@

%.h:
	ln -s $(INCSRC)/$@

.includes.tmp:
	@echo -n "HEADERS=" > $@
	@echo $(INCSRC)/*.h | sed -es@$(INCSRC)/@@g >> $@
	@echo >> $@
	@for i in $(INCSRC)/*.h ; do \
		s=$$(echo $$i | sed -es@$(INCSRC)/@@g); \
		b=$$(basename $$s .h); \
		if test -f $$b.patch; then \
			echo "$$s: $$i $$b.patch" >> $@; \
			echo "	cp \$$(INCSRC)/\$$@ ." >> $@; \
			echo "	patch -p0 -i $$b.patch \$$@" >> $@; \
			echo >> $@; \
		fi; \
	 done

.includes:
	$(MAKE) .includes.tmp
	@if [ -f $@ ]; then \
	 cmp -s $@ .includes.tmp || mv -f .includes.tmp $@; \
	else \
	 mv -f .includes.tmp $@; \
	fi; \
	rm -f .includes.tmp

.PHONY: prepare prepare2
prepare2: $(HEADERS)

prepare: .includes
	$(MAKE) prepare2

clean:
	rm -f *.h
	rm -f .includes*

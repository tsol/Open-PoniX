
######################################################
## standart targets + recursion rule

.DEFAULT_GOAL := all
.PHONY : all clean distclean
.PHONY : install subdirs $(SUBDIRS) svnignore svnwarning strip
all clean distclean install svnignore strip: subdirs FORCE
# this one to prevent printing make[1]: Nothing to be done for `all'
FORCE:
	@#
subdirs : $(SUBDIRS)
$(SUBDIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)
unexport SUBDIRS
export DESTDIR


######################################################
## make output customization

# quiet -  'Q = smth' set Q to any non-empty value to get terse output
# normal - 'Q =' set Q to empty val (default) to see normal unchaged output
ifeq ($Q,)
summary = @true
else
override Q := @ 
summary = @echo "$(1)" $(subst $(TOPDIR)/,,$(CURDIR)/)$(2)
MAKEFLAGS += --no-print-directory
endif

######################################################
## Compilation flags

##
# if CFLAGS wasn't set by package builder set it to -O2
# warnings on, and path to #include <config.h>
ifeq ($(DEBUG),enabled)
	override CFLAGS = -Wall -I$(TOPDIR) -g -Werror 
else
	CFLAGS += -O2 -Wall -I$(TOPDIR) 
endif

 
T := $(subst $(TOPDIR)/,,$(CURDIR)/)
T := $(subst /, ,$(T))
T := $(firstword $(T))
ifeq ($(T),plugins)
    CFLAGSX += -DPLUGIN
endif

override CFLAGS  += $(CFLAGSX)
override LDFLAGS += $(LDFLAGSX)

TINS = $(wildcard *.in)
INS = $(TINS:.in=)

######################################################
## Compilation rules

export EXTRA_DEPS += $(CURDIR)/Makefile
#$(warning "EXTRA_DEPS=$(EXTRA_DEPS)")	    
# SRCS - list of C source files to compile
ifneq (,$(SRCS))
OBJS += $(SRCS:.c=.o)
DEPS +=$(SRCS:.c=.d)
CLEANLIST += $(SRCS:.c=.o) $(SRCS:.c=.d)
$(OBJS) : $(EXTRA_DEPS)
endif


%.o : %.c
	$(call summary,CC  ,$@)
	$Q$(CC) $(CFLAGS) -c -o $@ $<


%.d : %.c
	$(call summary,DEP ,$@)
	$Q$(CC) $(CFLAGS) -M $< | sed 's,\(.*\)\.o[ :]*,$(@D)/\1.o $@ : ,g' > $@

% : %.in
	$(call summary,TEXT,$@)
	@#to have same perm
	$Qsed -f $(TOPDIR)/subst.sed  < $< > $@
	$Qchmod `stat --printf=%a $<` $@

# autoamtically updates files derived from *.in
all : $(INS)
#$(warning $(INS))	

######################################################
## Linkage rules

# binary
ifneq (,$(BINTARGET))
all : $(BINTARGET)
$(BINTARGET) : $(OBJS)
	$(call summary,LD  ,$@)
	$Q$(CC) $(OBJS) -o $@ $(LDFLAGS)

CLEANLIST += $(BINTARGET)
endif


# shared library lib*.so
ifneq (,$(LIBTARGET))
all : $(LIBTARGET)
$(LIBTARGET) : $(OBJS)
	$(call summary,LD  ,$@)
	$Q$(CC) $(OBJS) -o $@ $(LDFLAGS) -shared

CLEANLIST += $(LIBTARGET)
endif

# ar archive
ifneq (,$(ARTARGET))
all : $(ARTARGET)
$(ARTARGET) : $(OBJS)
	@echo AR   $@
	$(call summary,CC  ,$@)
	@$(AR) r $@ $(OBJS)

CLEANLIST += $(ARTARGET)
endif

######################################################
## Strip rules
strip:
ifneq (,$(strip $(BINTARGET) $(LIBTARGET)))
	$(call summary,STRIP ,$@)
	$Qstrip $(BINTARGET) $(LIBTARGET)
endif

######################################################
## Clean rules

ifneq (,$(CLEANLIST))
clean:
	rm -f $(CLEANLIST)
endif

DISTCLEANLIST += $(INS)
ifeq ($(TOPDIR),$(CURDIR))
DISTCLEANLIST += config.h config.mk subst.sed tags
endif
distclean : clean
	rm -f $(DISTCLEANLIST)


######################################################
## Dependancy

ifeq ($(DEPENDENCY),enabled)
ifeq (,$(MAKECMDGOALS))
MAKECMDGOALS=all
endif
ifneq ($(findstring all, $(MAKECMDGOALS)),)
-include $(DEPS)
endif
endif


######################################################
## svn:ignore property automatiozation

ifeq (0,$(MAKELEVEL))
svnignore : svnwarning
ifneq ($(findstring svnignore, $(MAKECMDGOALS)),)
$(SUBDIRS) : svnwarning
endif
endif

svnwarning :
	@echo 
	@echo " Make is about to set svn:ignore property for every directory in a project"
	@echo " and update a repository. Property's value will consist from files" 
	@echo " that would be normaly deleted by clean or distclean target."
	@echo 
	@echo " Note: It's recommended to commit all changes and update a working copy beforehand."
	@echo " Note: If in doubt - press Ctrl-C."
	@echo 
	@read -p " Continue [y/N] ? " -e -t 5 ok; echo; [ "$$ok" == "y" -o "$$ok" == "Y" ]

svnignore:
	@prop=prop-$$$$.txt; \
	for i in $(DISTCLEANLIST) $(CLEANLIST); do echo "$$i"; done > $$prop;  \
	cat $$prop; \
	svn propset svn:ignore --file $$prop .; \
	rm -f $$prop


######################################################
## tar'ing the project

tar :
	cd $(TOPDIR)
	$(MAKE) distclean
	scripts/mk_tar


install=@$(TOPDIR)/scripts/install.sh

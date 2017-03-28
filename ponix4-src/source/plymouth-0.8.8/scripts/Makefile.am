noinst_SCRIPTS = new-object.sh 

initrdscriptdir = $(libexecdir)/plymouth
initrdscript_SCRIPTS = plymouth-update-initrd plymouth-generate-initrd plymouth-populate-initrd

sbinscriptsdir = $(sbindir)
sbinscripts_SCRIPTS = plymouth-set-default-theme

EXTRA_DIST= plymouth-update-initrd                                             \
	    plymouth-generate-initrd.in                                        \
	    plymouth-populate-initrd.in                                        \
	    plymouth-set-default-theme.in                                     \
	    $(noinst_SCRIPTS)                                                  \
	    default.cfg

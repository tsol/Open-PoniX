cmd_arch/x86/boot/voffset.h := nm vmlinux | sed -n -e 's/^\([0-9a-fA-F]*\) [ABCDGRSTVW] \(_text\|_end\)$$/\#define VO_\2 0x\1/p' > arch/x86/boot/voffset.h

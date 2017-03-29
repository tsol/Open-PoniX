	.file	"bounds.c"
# GNU C (crosstool-NG 1.15.2) version 4.6.3 (i486-TSOL-linux-gnu)
#	compiled by GNU C version 4.4.3, GMP version 5.0.2, MPFR version 3.1.0, MPC version 0.9
# GGC heuristics: --param ggc-min-expand=30 --param ggc-min-heapsize=4096
# options passed:  -nostdinc -I ./arch/x86/include
# -I arch/x86/include/generated -I include -I ./arch/x86/include/uapi
# -I arch/x86/include/generated/uapi -I ./include/uapi
# -I include/generated/uapi
# -iprefix /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/
# -D __KERNEL__ -D CONFIG_AS_CFI=1 -D CONFIG_AS_CFI_SIGNAL_FRAME=1
# -D CONFIG_AS_CFI_SECTIONS=1 -D CONFIG_AS_CRC32=1 -D CONFIG_AS_AVX=1
# -D CC_HAVE_ASM_GOTO -D KBUILD_STR(s)=#s
# -D KBUILD_BASENAME=KBUILD_STR(bounds)
# -D KBUILD_MODNAME=KBUILD_STR(bounds)
# -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include
# -include ./include/linux/kconfig.h -MD kernel/.bounds.s.d kernel/bounds.c
# -m32 -msoft-float -mregparm=3 -mpreferred-stack-boundary=2 -march=i486
# -mtune=generic -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx
# -auxbase-strip kernel/bounds.s -Os -Wall -Wundef -Wstrict-prototypes
# -Wno-trigraphs -Werror=implicit-function-declaration -Wno-format-security
# -Wno-sign-compare -Wframe-larger-than=1024 -Wno-unused-but-set-variable
# -Wdeclaration-after-statement -Wno-pointer-sign -Werror=implicit-int
# -Werror=strict-prototypes -fno-strict-aliasing -fno-common
# -freg-struct-return -fno-pic -ffreestanding
# -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks
# -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls
# -fno-var-tracking-assignments -fno-strict-overflow -fconserve-stack
# -fverbose-asm
# options enabled:  -fauto-inc-dec -fbranch-count-reg -fcaller-saves
# -fcombine-stack-adjustments -fcompare-elim -fcprop-registers
# -fcrossjumping -fcse-follow-jumps -fdefer-pop -fdevirtualize
# -fdwarf2-cfi-asm -fearly-inlining -feliminate-unused-debug-types
# -fexpensive-optimizations -fforward-propagate -ffunction-cse -fgcse
# -fgcse-lm -fguess-branch-probability -fident -fif-conversion
# -fif-conversion2 -findirect-inlining -finline -finline-functions
# -finline-functions-called-once -finline-small-functions -fipa-cp
# -fipa-profile -fipa-pure-const -fipa-reference -fipa-sra
# -fira-share-save-slots -fira-share-spill-slots -fivopts
# -fkeep-static-consts -fleading-underscore -fmath-errno -fmerge-constants
# -fmerge-debug-strings -fmove-loop-invariants -foptimize-register-move
# -fpartial-inlining -fpeephole -fpeephole2 -fprefetch-loop-arrays
# -freg-struct-return -fregmove -freorder-blocks -freorder-functions
# -frerun-cse-after-loop -fsched-critical-path-heuristic
# -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
# -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
# -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fschedule-insns2
# -fshow-column -fsigned-zeros -fsplit-ivs-in-unroller -fsplit-wide-types
# -fstrict-volatile-bitfields -fthread-jumps -ftoplevel-reorder
# -ftrapping-math -ftree-bit-ccp -ftree-builtin-call-dce -ftree-ccp
# -ftree-ch -ftree-copy-prop -ftree-copyrename -ftree-cselim -ftree-dce
# -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
# -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
# -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop -ftree-pre
# -ftree-pta -ftree-reassoc -ftree-scev-cprop -ftree-sink
# -ftree-slp-vectorize -ftree-sra -ftree-switch-conversion -ftree-ter
# -ftree-vect-loop-version -ftree-vrp -funit-at-a-time -fvect-cost-model
# -fverbose-asm -fzero-initialized-in-bss -m32 -m96bit-long-double
# -malign-stringops -mglibc -mieee-fp -mno-fancy-math-387 -mno-red-zone
# -mno-sse4 -mpush-args -msahf -mtls-direct-seg-refs

# Compiler executable checksum: 0e1304aa8a73c2b30d48ced65433bc8d

	.text
	.globl	foo
	.type	foo, @function
foo:
	pushl	%ebp	#
	movl	%esp, %ebp	#,
#APP
# 19 "kernel/bounds.c" 1
	
->NR_PAGEFLAGS $22 __NR_PAGEFLAGS	#
# 0 "" 2
# 20 "kernel/bounds.c" 1
	
->MAX_NR_ZONES $3 __MAX_NR_ZONES	#
# 0 "" 2
# 21 "kernel/bounds.c" 1
	
->NR_PCG_FLAGS $3 __NR_PCG_FLAGS	#
# 0 "" 2
# 25 "kernel/bounds.c" 1
	
->SPINLOCK_SIZE $0 sizeof(spinlock_t)	#
# 0 "" 2
#NO_APP
	popl	%ebp	#
	ret
	.size	foo, .-foo
	.ident	"GCC: (crosstool-NG 1.15.2) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

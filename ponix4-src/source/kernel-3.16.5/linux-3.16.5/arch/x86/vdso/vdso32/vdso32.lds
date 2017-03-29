/*
 *
 * Automatically generated file; DO NOT EDIT.
 * Linux/x86 3.16.5 Kernel Configuration
 *
 */
/*
 * Helper macros to use CONFIG_ options in C/CPP expressions. Note that
 * these only work with boolean and tristate options.
 */
/*
 * Getting something that works in C and CPP for an arg that may or may
 * not be defined is tricky.  Here, if we have "#define CONFIG_BOOGER 1"
 * we match on the placeholder define, insert the "0," for arg1 and generate
 * the triplet (0, 1, 0).  Then the last step cherry picks the 2nd arg (a one).
 * When CONFIG_BOOGER is not defined, we generate a (... 1, 0) pair, and when
 * the last step cherry picks the 2nd arg, we get a zero.
 */






/*
 * IS_ENABLED(CONFIG_FOO) evaluates to 1 if CONFIG_FOO is set to 'y' or 'm',
 * 0 otherwise.
 *
 */



/*
 * IS_BUILTIN(CONFIG_FOO) evaluates to 1 if CONFIG_FOO is set to 'y', 0
 * otherwise. For boolean options, this is equivalent to
 * IS_ENABLED(CONFIG_FOO).
 */


/*
 * IS_MODULE(CONFIG_FOO) evaluates to 1 if CONFIG_FOO is set to 'm', 0
 * otherwise.
 */
/*
 * Linker script for 32-bit vDSO.
 * We #include the file to define the layout details.
 *
 * This file defines the version script giving the user-exported symbols in
 * the DSO.
 */
/*
 * int-ll64 is used everywhere now.
 */
/*
 * asm-generic/int-ll64.h
 *
 * Integer declarations for architectures which use "long long"
 * for 64-bit types.
 */
/*
 * asm-generic/int-ll64.h
 *
 * Integer declarations for architectures which use "long long"
 * for 64-bit types.
 */
/*
 * There seems to be no way of detecting this automatically from user
 * space, so 64 bit architectures should override this in their
 * bitsperlong.h. In particular, an architecture that supports
 * both 32 and 64 bit user space must not rely on CONFIG_64BIT
 * to decide it, but rather check a compiler provided macro.
 */
/*
 * FIXME: The check currently breaks x86-64 build, so it's
 * temporarily disabled. Please fix x86-64 and reenable
 */
/* const.h: Macros for dealing with constants.  */
/* Some constant macros are used in both assembler and
 * C code.  Therefore we cannot annotate them always with
 * 'UL' and other type specifiers unilaterally.  We
 * use the following macros to deal with this.
 *
 * Similarly, _AT() will cast an expression with a type in C, but
 * leave it unchanged in asm.
 */
/* PAGE_SHIFT determines the page size */
/* Cast PAGE_MASK to a signed type so that it is sign-extended if
   virtual addresses are 32-bits but physical addresses are larger
   (ie, 32-bit PAE). */
/* const.h: Macros for dealing with constants.  */
/*
 * This handles the memory map.
 *
 * A __PAGE_OFFSET of 0xC0000000 means that the kernel has
 * a virtual address space of one gigabyte, which limits the
 * amount of physical memory you can use to about 950MB.
 *
 * If you want more physical memory than this then see the CONFIG_HIGHMEM4G
 * and CONFIG_HIGHMEM64G options in the kernel configuration.
 */
/*
 * Kernel image size is limited to 512 MB (see in arch/x86/kernel/head_32.S)
 */
/*
 * Allow us to mark functions as 'deprecated' and have gcc emit a nice
 * warning for each use, in hopes of speeding the functions removal.
 * Usage is:
 * 		int __deprecated foo(void)
 */
/*
 * Allow us to avoid 'defined but not used' warnings on functions and data,
 * as well as force them to be emitted to the assembly file.
 *
 * As of gcc 3.4, static functions that are not marked with attribute((used))
 * may be elided from the assembly file.  As of gcc 3.4, static data not so
 * marked will not be elided, but this may change in a future gcc version.
 *
 * NOTE: Because distributions shipped with a backported unit-at-a-time
 * compiler in gcc 3.3, we must define __used to be __attribute__((used))
 * for gcc >=3.3 instead of 3.4.
 *
 * In prior versions of gcc, such functions and data would be emitted, but
 * would be warned about except with attribute((unused)).
 *
 * Mark functions that are referenced only in inline assembly as __used so
 * the code is emitted even though it appears to be unreferenced.
 */
/*
 * Rather then using noinline to prevent stack consumption, use
 * noinline_for_stack instead.  For documentation reasons.
 */
/*
 * From the GCC manual:
 *
 * Many functions do not examine any values except their arguments,
 * and have no effects except the return value.  Basically this is
 * just slightly more strict class than the `pure' attribute above,
 * since function is not allowed to read global memory.
 *
 * Note that a function that has pointer arguments and examines the
 * data pointed to must _not_ be declared `const'.  Likewise, a
 * function that calls a non-`const' function usually must not be
 * `const'.  It does not make sense for a `const' function to return
 * `void'.
 */
/*
 * Tell gcc if a function is cold. The compiler will assume any path
 * directly leading to the call is unlikely.
 */
/* Simple shorthand for a section definition */
/* Are two types/vars the same type (ignoring qualifiers)? */
/* Is this type a native word size -- useful for atomic operations */
/* Compile time object size, -1 for unknown */
/*
 * Sparse complains of variable sized arrays due to the temporary variable in
 * __compiletime_assert. Unfortunately we can't just expand it out to make
 * sparse see a constant array size without breaking compiletime_assert on old
 * versions of GCC (e.g. 4.2.4), so hide the array from sparse altogether.
 */
/**
 * compiletime_assert - break build and emit msg if condition is false
 * @condition: a compile-time constant condition to check
 * @msg:       a message to emit if condition is false
 *
 * In tradition of POSIX assert, this macro will break the build if the
 * supplied condition is *false*, emitting the supplied error message if the
 * compiler has support to do so.
 */
/*
 * Prevent the compiler from merging or refetching accesses.  The compiler
 * is also forbidden from reordering successive instances of ACCESS_ONCE(),
 * but only when the compiler is aware of some particular ordering.  One way
 * to make the compiler aware of ordering is to put the two invocations of
 * ACCESS_ONCE() in different C statements.
 *
 * This macro does absolutely -nothing- to prevent the CPU from reordering,
 * merging, or refetching absolutely anything at any time.  Its main intended
 * use is to mediate communication between process-level code and irq/NMI
 * handlers, all running on the same CPU.
 */
/* Ignore/forbid kprobes attach on very low level functions marked by this attribute: */
/* Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */
/*
 * Export symbols from the kernel to modules.  Forked from module.h
 * to reduce the amount of pointless cruft we feed to gcc when only
 * exporting a simple symbol or two.
 *
 * Try not to add #includes here.  It slows compilation and makes kernel
 * hackers place grumpy comments in header files.
 */
/* Some toolchains use a `_' prefix for all user symbols. */
/* Indirect, so macros are expanded before pasting. */
/*
 * Make sure the compiler doesn't do anything stupid with the
 * arguments on the stack - they are owned by the *caller*, not
 * the callee. This just fools gcc into not spilling into them,
 * and keeps it from doing tailcall recursion and/or using the
 * stack slots for temporaries, since they are live and "used"
 * all the way to the end of the function.
 *
 * NOTE! On x86-64, all the arguments are in registers, so this
 * only matters on a 32-bit kernel.
 */
/* Some toolchains use other characters (e.g. '`') to mark new line in macro */
/*
 * For assembly routines.
 *
 * Note when using these that you must specify the appropriate
 * alignment directives yourself
 */
/*
 * This is used by architectures to keep arguments on the stack
 * untouched by the compiler by keeping them live until the end.
 * The argument stack may be owned by the assembly-language
 * caller, not the callee, and gcc doesn't always understand
 * that.
 *
 * We have the return value, and a maximum of six arguments.
 *
 * This should always be followed by a "return ret" for the
 * protection to work (ie no more work that the compiler might
 * end up needing stack temporaries for).
 */
/* Assembly files may be compiled with -traditional .. */
/* If symbol 'name' is treated as a subroutine (gets called, and returns)
 * then please use ENDPROC to mark 'name' as STT_FUNC for the benefit of
 * static analysis tools such as stack depth analyzer.
 */
/* These macros are used to mark some functions or 
 * initialized data (doesn't apply to uninitialized data)
 * as `initialization' functions. The kernel can take this
 * as hint that the function is used only during the initialization
 * phase and free up used memory resources after
 *
 * Usage:
 * For functions:
 * 
 * You should add __init immediately before the function name, like:
 *
 * static void __init initme(int x, int y)
 * {
 *    extern int z; z = x * y;
 * }
 *
 * If the function has a prototype somewhere, you can also add
 * __init between closing brace of the prototype and semicolon:
 *
 * extern int initialize_foobar_device(int, int, int) __init;
 *
 * For initialized data:
 * You should insert __initdata or __initconst between the variable name
 * and equal sign followed by value, e.g.:
 *
 * static int init_variable __initdata = 0;
 * static const char linux_logo[] __initconst = { 0x32, 0x36, ... };
 *
 * Don't forget to initialize data not at file scope, i.e. within a function,
 * as gcc otherwise puts the data into the bss section and not into the init
 * section.
 */
/* These are for everybody (although not all archs will actually
   discard it in modules) */
/*
 * Some architecture have tool chains which do not handle rodata attributes
 * correctly. For those disable special sections for const, so that other
 * architectures can annotate correctly.
 */
/*
 * modpost check for section mismatches during the kernel build.
 * A section mismatch happens when there are references from a
 * code or data section to an init section (both code or data).
 * The init sections are (for most archs) discarded by the kernel
 * when early init has completed so all such references are potential bugs.
 * For exit sections the same issue exists.
 *
 * The following markers are used for the cases where the reference to
 * the *init / *exit section (code or data) is valid and will teach
 * modpost not to issue a warning.  Intended semantics is that a code or
 * data tagged __ref* can reference code or data from init section without
 * producing a warning (of course, no warning does not mean code is
 * correct, so optimally document why the __ref is needed and why it's OK).
 *
 * The markers follow same syntax rules as __init / __initdata.
 */
/* compatibility defines */
/* temporary, until all users are removed */
/* Used for MEMORY_HOTPLUG */
/* For assembly routines */
/* temporary, until all users are removed */
/* silence warnings when references are OK */
/**
 * module_init() - driver initialization entry point
 * @x: function to be run at kernel boot time or module insertion
 * 
 * module_init() will either be called during do_initcalls() (if
 * builtin) or at module insertion time (if a module).  There can only
 * be one per module.
 */
/**
 * module_exit() - driver exit entry point
 * @x: function to be run when driver is removed
 * 
 * module_exit() will wrap the driver clean-up code
 * with cleanup_module() when used with rmmod when
 * the driver is a module.  If the driver is statically
 * compiled into the kernel, module_exit() has no effect.
 * There can only be one per module.
 */
/* Data marked not to be saved by software suspend */
/* This means "can be init if no module support, otherwise module load
   may call it." */
/*
 * Linker script for vDSO.  This is an ELF shared object prelinked to
 * its virtual address, and with only one read-only segment.
 * This script controls its layout.
 */
SECTIONS
{
 . = SIZEOF_HEADERS;
 .hash : { *(.hash) } :text
 .gnu.hash : { *(.gnu.hash) }
 .dynsym : { *(.dynsym) }
 .dynstr : { *(.dynstr) }
 .gnu.version : { *(.gnu.version) }
 .gnu.version_d : { *(.gnu.version_d) }
 .gnu.version_r : { *(.gnu.version_r) }
 .dynamic : { *(.dynamic) } :text :dynamic
 .rodata : {
  *(.rodata*)
  *(.data*)
  *(.sdata*)
  *(.got.plt) *(.got)
  *(.gnu.linkonce.d.*)
  *(.bss*)
  *(.dynbss*)
  *(.gnu.linkonce.b.*)
  /*
		 * Ideally this would live in a C file, but that won't
		 * work cleanly for x32 until we start building the x32
		 * C code using an x32 toolchain.
		 */
  VDSO_FAKE_SECTION_TABLE_START = .;
  . = . + 13 * 40;
  VDSO_FAKE_SECTION_TABLE_END = .;
 } :text
 .fake_shstrtab : { *(.fake_shstrtab) } :text
 .note : { *(.note.*) } :text :note
 .eh_frame_hdr : { *(.eh_frame_hdr) } :text :eh_frame_hdr
 .eh_frame : { KEEP (*(.eh_frame)) } :text
 /*
	 * Text is well-separated from actual data: there's plenty of
	 * stuff that isn't used at runtime in between.
	 */
 .text : { *(.text*) } :text =0x90909090,
 /*
	 * At the end so that eu-elflint stays happy when vdso2c strips
	 * these.  A better implementation would avoid allocating space
	 * for these.
	 */
 .altinstructions : { *(.altinstructions) } :text
 .altinstr_replacement : { *(.altinstr_replacement) } :text
 /*
	 * The remainder of the vDSO consists of special pages that are
	 * shared between the kernel and userspace.  It needs to be at the
	 * end so that it doesn't overlap the mapping of the actual
	 * vDSO image.
	 */
 . = ALIGN((1 << 12));
 vvar_page = .;
 /* Place all vvars at the offsets in asm/vvar.h. */
/*
 * vvar.h: Shared vDSO/kernel variable declarations
 * Copyright (c) 2011 Andy Lutomirski
 * Subject to the GNU General Public License, version 2
 *
 * A handful of variables are accessible (read-only) from userspace
 * code in the vsyscall page and the vdso.  They are declared here.
 * Some other file must define them with DEFINE_VVAR.
 *
 * In normal kernel code, they are used like any other variable.
 * In user code, they are accessed through the VVAR macro.
 *
 * These variables live in a page of kernel data that has an extra RO
 * mapping for userspace.  Each variable needs a unique offset within
 * that page; specify that offset with the DECLARE_VVAR macro.  (If
 * you mess up, the linker will catch it.)
 */
/* The kernel linker script defines its own magic to put vvars in the
 * right place.
 */
/* DECLARE_VVAR(offset, type, name) */
vvar_jiffies = vvar_page + 0;
vvar_vgetcpu_mode = vvar_page + 16;
vvar_vsyscall_gtod_data = vvar_page + 128;
 . = vvar_page + (1 << 12);
 hpet_page = .;
 . = . + (1 << 12);
 . = ALIGN((1 << 12));
 end_mapping = .;
 /DISCARD/ : {
  *(.discard)
  *(.discard.*)
  *(__bug_table)
 }
}
/*
 * Very old versions of ld do not recognize this name token; use the constant.
 */
/*
 * We must supply the ELF program headers explicitly to get just one
 * PT_LOAD segment, and set the flags explicitly to make segments read-only.
 */
PHDRS
{
 text PT_LOAD FLAGS(5) FILEHDR PHDRS; /* PF_R|PF_X */
 dynamic PT_DYNAMIC FLAGS(4); /* PF_R */
 note PT_NOTE FLAGS(4); /* PF_R */
 eh_frame_hdr 0x6474e550;
}
/* The ELF entry point can be used to set the AT_SYSINFO value.  */
ENTRY(__kernel_vsyscall);
/*
 * This controls what userland symbols we export from the vDSO.
 */
VERSION
{
 LINUX_2.6 {
 global:
  __vdso_clock_gettime;
  __vdso_gettimeofday;
  __vdso_time;
 };
 LINUX_2.5 {
 global:
  __kernel_vsyscall;
  __kernel_sigreturn;
  __kernel_rt_sigreturn;
 local: *;
 };
}

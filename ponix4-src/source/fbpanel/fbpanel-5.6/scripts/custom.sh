

#########################
## Custom Settings     ##
#########################

# Note 1: PWD will be that of configure, not scripts directory, so to run script from
# this directory refer it as 'scripts/name'

# Note 2: values will be evaluated in the same order they were added, so
# if you want libdir's default value to be '$eprefix/lib', add it after prefix

# Usefull stuff
add_var endianess "detect endianess (big or little)" '`scripts/endianess.sh`'
add_var os "detect OS flavour" '`uname -s | tr [:lower:] [:upper:]`'

# Root File System for embedded or cross-compiler builds
#add_var rfs "embedded RFS (root file system)" /nfsboot/rfs


# Custom
add_feature dependency "disable dependency tracking" enabled

add_var glib_cflags "glib cflags" '`RFS=$rfs scripts/rfs-pkg-config --cflags glib-2.0`'
add_var gtk_cflags "gtk cflags" '`RFS=$rfs scripts/rfs-pkg-config --cflags gtk+-2.0`'

add_var glib_libs "glib libs" '`RFS=$rfs scripts/rfs-pkg-config --libs glib-2.0`'
add_var gtk_libs "gtk libs" '`RFS=$rfs scripts/rfs-pkg-config --libs gtk+-2.0`'

add_var cflagsx "C flags" '-I$topdir/panel $glib_cflags $gtk_cflags -fPIC'
add_var ldflagsx "linker flags" '$glib_libs $gtk_libs'

add_var gmodule_libs "gmodule libs" '`scripts/rfs-pkg-config --libs gmodule-2.0`'

add_feature static_build "build all pluginis into main binary" disabled

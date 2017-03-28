FILE(REMOVE_RECURSE
  "libfreerdp.pdb"
  "libfreerdp.a"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/freerdp.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)

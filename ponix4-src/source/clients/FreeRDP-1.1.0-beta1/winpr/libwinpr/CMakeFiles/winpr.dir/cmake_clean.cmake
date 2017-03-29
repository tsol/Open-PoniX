FILE(REMOVE_RECURSE
  "libwinpr.pdb"
  "libwinpr.a"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/winpr.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)

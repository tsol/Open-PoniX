#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
SET(CMAKE_IMPORT_FILE_VERSION 1)

# Compute the installation prefix relative to this file.
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "winpr" for configuration "Release"
SET_PROPERTY(TARGET winpr APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(winpr PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "rt;/usr/lib/libssl.so;/usr/lib/libcrypto.so;m;-lpthread;dl"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libwinpr.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS winpr )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_winpr "${_IMPORT_PREFIX}/lib/libwinpr.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "winpr-tools" for configuration "Release"
SET_PROPERTY(TARGET winpr-tools APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(winpr-tools PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "crypto;winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libwinpr-tools.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS winpr-tools )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_winpr-tools "${_IMPORT_PREFIX}/lib/libwinpr-tools.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "winpr-makecert" for configuration "Release"
SET_PROPERTY(TARGET winpr-makecert APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(winpr-makecert PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/winpr-makecert"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS winpr-makecert )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_winpr-makecert "${_IMPORT_PREFIX}/bin/winpr-makecert" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "winpr-hash" for configuration "Release"
SET_PROPERTY(TARGET winpr-hash APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(winpr-hash PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/winpr-hash"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS winpr-hash )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_winpr-hash "${_IMPORT_PREFIX}/bin/winpr-hash" )

# Loop over all imported files and verify that they actually exist
FOREACH(target ${_IMPORT_CHECK_TARGETS} )
  FOREACH(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    IF(NOT EXISTS "${file}" )
      MESSAGE(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    ENDIF()
  ENDFOREACH()
  UNSET(_IMPORT_CHECK_FILES_FOR_${target})
ENDFOREACH()
UNSET(_IMPORT_CHECK_TARGETS)

# Cleanup temporary variables.
SET(_IMPORT_PREFIX)

# Commands beyond this point should not need to know the version.
SET(CMAKE_IMPORT_FILE_VERSION)

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
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()
IF(NOT TARGET "freerdp" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"freerdp\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"freerdp\" not found ! ")
  ENDIF()
ENDIF()

# Import target "rdtk" for configuration "Release"
SET_PROPERTY(TARGET rdtk APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(rdtk PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/librdtk.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdtk )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdtk "${_IMPORT_PREFIX}/lib/librdtk.a" )

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

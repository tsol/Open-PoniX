#----------------------------------------------------------------
# Generated CMake target import file for configuration "Test Release with JPEG decoder".
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
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()

# Import target "freerdp" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET freerdp APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(freerdp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "-lpthread;dl;winpr;winpr;winpr;/usr/lib/libssl.so;/usr/lib/libcrypto.so;winpr;/usr/lib/libz.so;winpr;/usr/xorg/lib/libX11.so;/usr/xorg/lib/libxkbfile.so;winpr;winpr;/usr/lib/libz.so;/usr/lib/libssl.so;/usr/lib/libcrypto.so;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/libfreerdp.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS freerdp )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_freerdp "${_IMPORT_PREFIX}/lib/libfreerdp.a" )

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

# Import target "parallel-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET parallel-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(parallel-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/parallel-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS parallel-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_parallel-client "${_IMPORT_PREFIX}/lib/freerdp/parallel-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "tsmf-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET tsmf-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(tsmf-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/tsmf-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS tsmf-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_tsmf-client "${_IMPORT_PREFIX}/lib/freerdp/tsmf-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "tsmf-client-alsa-audio" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET tsmf-client-alsa-audio APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(tsmf-client-alsa-audio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;/usr/lib/libasound.so"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/tsmf-client-alsa-audio.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS tsmf-client-alsa-audio )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_tsmf-client-alsa-audio "${_IMPORT_PREFIX}/lib/freerdp/tsmf-client-alsa-audio.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "cliprdr-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET cliprdr-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(cliprdr-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/cliprdr-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS cliprdr-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_cliprdr-client "${_IMPORT_PREFIX}/lib/freerdp/cliprdr-client.a" )

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

# Import target "rdpgfx-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET rdpgfx-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(rdpgfx-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/rdpgfx-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpgfx-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpgfx-client "${_IMPORT_PREFIX}/lib/freerdp/rdpgfx-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "rail-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET rail-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(rail-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/rail-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rail-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rail-client "${_IMPORT_PREFIX}/lib/freerdp/rail-client.a" )

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

# Import target "drive-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET drive-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(drive-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/drive-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drive-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drive-client "${_IMPORT_PREFIX}/lib/freerdp/drive-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "audin-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET audin-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(audin-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/audin-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS audin-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_audin-client "${_IMPORT_PREFIX}/lib/freerdp/audin-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "audin-client-alsa" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET audin-client-alsa APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(audin-client-alsa PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;/usr/lib/libasound.so"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/audin-client-alsa.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS audin-client-alsa )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_audin-client-alsa "${_IMPORT_PREFIX}/lib/audin-client-alsa.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drdynvc-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET drdynvc-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(drdynvc-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/drdynvc-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drdynvc-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drdynvc-client "${_IMPORT_PREFIX}/lib/freerdp/drdynvc-client.a" )

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

# Import target "rdpsnd-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET rdpsnd-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(rdpsnd-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/rdpsnd-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpsnd-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpsnd-client "${_IMPORT_PREFIX}/lib/freerdp/rdpsnd-client.a" )

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

# Import target "rdpsnd-client-alsa" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET rdpsnd-client-alsa APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(rdpsnd-client-alsa PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr;/usr/lib/libasound.so"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/rdpsnd-client-alsa.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpsnd-client-alsa )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpsnd-client-alsa "${_IMPORT_PREFIX}/lib/freerdp/rdpsnd-client-alsa.a" )

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

# Import target "rdpei-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET rdpei-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(rdpei-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/rdpei-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpei-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpei-client "${_IMPORT_PREFIX}/lib/freerdp/rdpei-client.a" )

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

# Import target "rdpdr-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET rdpdr-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(rdpdr-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/rdpdr-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpdr-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpdr-client "${_IMPORT_PREFIX}/lib/freerdp/rdpdr-client.a" )

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

# Import target "smartcard-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET smartcard-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(smartcard-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr;pcsclite"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/smartcard-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS smartcard-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_smartcard-client "${_IMPORT_PREFIX}/lib/freerdp/smartcard-client.a" )

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

# Import target "disp-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET disp-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(disp-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/disp-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS disp-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_disp-client "${_IMPORT_PREFIX}/lib/freerdp/disp-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "serial-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET serial-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(serial-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/serial-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS serial-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_serial-client "${_IMPORT_PREFIX}/lib/freerdp/serial-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "echo-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET echo-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(echo-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "freerdp"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/freerdp/echo-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS echo-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_echo-client "${_IMPORT_PREFIX}/lib/freerdp/echo-client.a" )

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
IF(NOT TARGET "winpr" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"winpr\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"winpr\" not found ! ")
  ENDIF()
ENDIF()

# Import target "freerdp-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET freerdp-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(freerdp-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "parallel-client;drive-client;smartcard-client;serial-client;tsmf-client;rdpgfx-client;audin-client;rdpei-client;disp-client;echo-client;cliprdr-client;rail-client;drdynvc-client;rdpsnd-client;rdpdr-client;tsmf-client-alsa-audio;audin-client-alsa;rdpsnd-client-alsa;freerdp;winpr;freerdp;winpr"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/libfreerdp-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS freerdp-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_freerdp-client "${_IMPORT_PREFIX}/lib/libfreerdp-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "xfreerdp-client" for configuration "Test Release with JPEG decoder"
SET_PROPERTY(TARGET xfreerdp-client APPEND PROPERTY IMPORTED_CONFIGURATIONS TEST RELEASE WITH JPEG DECODER)
SET_TARGET_PROPERTIES(xfreerdp-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_TEST RELEASE WITH JPEG DECODER "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_TEST RELEASE WITH JPEG DECODER "/usr/xorg/lib/libX11.so;dl;/usr/xorg/lib/libXinerama.so;/usr/xorg/lib/libXext.so;/usr/xorg/lib/libXcursor.so;/usr/xorg/lib/libXv.so;/usr/xorg/lib/libXrender.so;freerdp-client;freerdp"
  IMPORTED_LOCATION_TEST RELEASE WITH JPEG DECODER "${_IMPORT_PREFIX}/lib/libxfreerdp-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS xfreerdp-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_xfreerdp-client "${_IMPORT_PREFIX}/lib/libxfreerdp-client.a" )

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

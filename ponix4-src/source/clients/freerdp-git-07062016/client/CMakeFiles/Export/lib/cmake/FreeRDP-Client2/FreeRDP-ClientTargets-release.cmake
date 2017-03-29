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
IF(NOT TARGET "freerdp" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"freerdp\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"freerdp\" not found ! ")
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

# Import target "parallel-client" for configuration "Release"
SET_PROPERTY(TARGET parallel-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(parallel-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "freerdp;winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libparallel-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS parallel-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_parallel-client "${_IMPORT_PREFIX}/lib/freerdp2/libparallel-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.
IF(NOT TARGET "freerdp" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"freerdp\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"freerdp\" not found ! ")
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

# Import target "tsmf-client" for configuration "Release"
SET_PROPERTY(TARGET tsmf-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(tsmf-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "freerdp;winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libtsmf-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS tsmf-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_tsmf-client "${_IMPORT_PREFIX}/lib/freerdp2/libtsmf-client.a" )

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

# Import target "tsmf-client-oss-audio" for configuration "Release"
SET_PROPERTY(TARGET tsmf-client-oss-audio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(tsmf-client-oss-audio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libtsmf-client-oss-audio.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS tsmf-client-oss-audio )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_tsmf-client-oss-audio "${_IMPORT_PREFIX}/lib/freerdp2/libtsmf-client-oss-audio.a" )

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

# Import target "tsmf-client-alsa-audio" for configuration "Release"
SET_PROPERTY(TARGET tsmf-client-alsa-audio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(tsmf-client-alsa-audio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/usr/lib/libasound.so;winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libtsmf-client-alsa-audio.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS tsmf-client-alsa-audio )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_tsmf-client-alsa-audio "${_IMPORT_PREFIX}/lib/freerdp2/libtsmf-client-alsa-audio.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "cliprdr-client" for configuration "Release"
SET_PROPERTY(TARGET cliprdr-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(cliprdr-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libcliprdr-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS cliprdr-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_cliprdr-client "${_IMPORT_PREFIX}/lib/freerdp2/libcliprdr-client.a" )

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

# Import target "rdpgfx-client" for configuration "Release"
SET_PROPERTY(TARGET rdpgfx-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(rdpgfx-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/librdpgfx-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpgfx-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpgfx-client "${_IMPORT_PREFIX}/lib/freerdp2/librdpgfx-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.
IF(NOT TARGET "freerdp" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"freerdp\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"freerdp\" not found ! ")
  ENDIF()
ENDIF()

# Import target "rail-client" for configuration "Release"
SET_PROPERTY(TARGET rail-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(rail-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/librail-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rail-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rail-client "${_IMPORT_PREFIX}/lib/freerdp2/librail-client.a" )

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

# Import target "drive-client" for configuration "Release"
SET_PROPERTY(TARGET drive-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(drive-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libdrive-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drive-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drive-client "${_IMPORT_PREFIX}/lib/freerdp2/libdrive-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.
IF(NOT TARGET "freerdp" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"freerdp\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"freerdp\" not found ! ")
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

# Import target "audin-client" for configuration "Release"
SET_PROPERTY(TARGET audin-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(audin-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "freerdp;winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libaudin-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS audin-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_audin-client "${_IMPORT_PREFIX}/lib/freerdp2/libaudin-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.
IF(NOT TARGET "freerdp" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"freerdp\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"freerdp\" not found ! ")
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

# Import target "audin-client-oss" for configuration "Release"
SET_PROPERTY(TARGET audin-client-oss APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(audin-client-oss PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "freerdp;winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libaudin-client-oss.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS audin-client-oss )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_audin-client-oss "${_IMPORT_PREFIX}/lib/freerdp2/libaudin-client-oss.a" )

# Make sure the targets which have been exported in some other 
# export set exist.
IF(NOT TARGET "freerdp" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"freerdp\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"freerdp\" not found ! ")
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

# Import target "audin-client-alsa" for configuration "Release"
SET_PROPERTY(TARGET audin-client-alsa APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(audin-client-alsa PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "freerdp;winpr;/usr/lib/libasound.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libaudin-client-alsa.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS audin-client-alsa )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_audin-client-alsa "${_IMPORT_PREFIX}/lib/freerdp2/libaudin-client-alsa.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drdynvc-client" for configuration "Release"
SET_PROPERTY(TARGET drdynvc-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(drdynvc-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libdrdynvc-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drdynvc-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drdynvc-client "${_IMPORT_PREFIX}/lib/freerdp2/libdrdynvc-client.a" )

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

# Import target "rdpsnd-client" for configuration "Release"
SET_PROPERTY(TARGET rdpsnd-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(rdpsnd-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp;-lpthread"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/librdpsnd-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpsnd-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpsnd-client "${_IMPORT_PREFIX}/lib/freerdp2/librdpsnd-client.a" )

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

# Import target "rdpsnd-client-oss" for configuration "Release"
SET_PROPERTY(TARGET rdpsnd-client-oss APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(rdpsnd-client-oss PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/librdpsnd-client-oss.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpsnd-client-oss )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpsnd-client-oss "${_IMPORT_PREFIX}/lib/freerdp2/librdpsnd-client-oss.a" )

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

# Import target "rdpsnd-client-alsa" for configuration "Release"
SET_PROPERTY(TARGET rdpsnd-client-alsa APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(rdpsnd-client-alsa PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp;/usr/lib/libasound.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/librdpsnd-client-alsa.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpsnd-client-alsa )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpsnd-client-alsa "${_IMPORT_PREFIX}/lib/freerdp2/librdpsnd-client-alsa.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "encomsp-client" for configuration "Release"
SET_PROPERTY(TARGET encomsp-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(encomsp-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libencomsp-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS encomsp-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_encomsp-client "${_IMPORT_PREFIX}/lib/freerdp2/libencomsp-client.a" )

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

# Import target "rdpei-client" for configuration "Release"
SET_PROPERTY(TARGET rdpei-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(rdpei-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/librdpei-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpei-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpei-client "${_IMPORT_PREFIX}/lib/freerdp2/librdpei-client.a" )

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

# Import target "rdpdr-client" for configuration "Release"
SET_PROPERTY(TARGET rdpdr-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(rdpdr-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/librdpdr-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS rdpdr-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_rdpdr-client "${_IMPORT_PREFIX}/lib/freerdp2/librdpdr-client.a" )

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

# Import target "smartcard-client" for configuration "Release"
SET_PROPERTY(TARGET smartcard-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(smartcard-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libsmartcard-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS smartcard-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_smartcard-client "${_IMPORT_PREFIX}/lib/freerdp2/libsmartcard-client.a" )

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

# Import target "disp-client" for configuration "Release"
SET_PROPERTY(TARGET disp-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(disp-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libdisp-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS disp-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_disp-client "${_IMPORT_PREFIX}/lib/freerdp2/libdisp-client.a" )

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

# Import target "serial-client" for configuration "Release"
SET_PROPERTY(TARGET serial-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(serial-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr;freerdp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libserial-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS serial-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_serial-client "${_IMPORT_PREFIX}/lib/freerdp2/libserial-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "remdesk-client" for configuration "Release"
SET_PROPERTY(TARGET remdesk-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(remdesk-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libremdesk-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS remdesk-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_remdesk-client "${_IMPORT_PREFIX}/lib/freerdp2/libremdesk-client.a" )

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

# Import target "echo-client" for configuration "Release"
SET_PROPERTY(TARGET echo-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(echo-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/freerdp2/libecho-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS echo-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_echo-client "${_IMPORT_PREFIX}/lib/freerdp2/libecho-client.a" )

# Make sure the targets which have been exported in some other 
# export set exist.
IF(NOT TARGET "freerdp" )
  IF(CMAKE_FIND_PACKAGE_NAME)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    SET( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "Required imported target \"freerdp\" not found ! ")
  ELSE()
    MESSAGE(FATAL_ERROR "Required imported target \"freerdp\" not found ! ")
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

# Import target "freerdp-client" for configuration "Release"
SET_PROPERTY(TARGET freerdp-client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(freerdp-client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "parallel-client;drive-client;smartcard-client;serial-client;tsmf-client;rdpgfx-client;audin-client;rdpei-client;disp-client;echo-client;cliprdr-client;rail-client;drdynvc-client;rdpsnd-client;encomsp-client;rdpdr-client;remdesk-client;tsmf-client-oss-audio;tsmf-client-alsa-audio;audin-client-oss;audin-client-alsa;rdpsnd-client-oss;rdpsnd-client-alsa;freerdp;winpr;winpr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfreerdp-client.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS freerdp-client )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_freerdp-client "${_IMPORT_PREFIX}/lib/libfreerdp-client.a" )

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

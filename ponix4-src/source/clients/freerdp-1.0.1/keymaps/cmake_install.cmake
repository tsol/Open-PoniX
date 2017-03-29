# Install script for directory: /source/clients/freerdp-1.0.1/keymaps

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/freerdp/keymaps" TYPE FILE FILES
    "/source/clients/freerdp-1.0.1/keymaps/aliases"
    "/source/clients/freerdp-1.0.1/keymaps/amiga"
    "/source/clients/freerdp-1.0.1/keymaps/ataritt"
    "/source/clients/freerdp-1.0.1/keymaps/empty"
    "/source/clients/freerdp-1.0.1/keymaps/evdev"
    "/source/clients/freerdp-1.0.1/keymaps/fujitsu"
    "/source/clients/freerdp-1.0.1/keymaps/hp"
    "/source/clients/freerdp-1.0.1/keymaps/ibm"
    "/source/clients/freerdp-1.0.1/keymaps/macintosh"
    "/source/clients/freerdp-1.0.1/keymaps/macosx"
    "/source/clients/freerdp-1.0.1/keymaps/sony"
    "/source/clients/freerdp-1.0.1/keymaps/sun"
    "/source/clients/freerdp-1.0.1/keymaps/xfree86"
    "/source/clients/freerdp-1.0.1/keymaps/xfree98"
    "/source/clients/freerdp-1.0.1/keymaps/xkb.pl"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/freerdp/keymaps" TYPE DIRECTORY FILES "/source/clients/freerdp-1.0.1/keymaps/digital_vndr" FILES_MATCHING REGEX "/[^/]*$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/freerdp/keymaps" TYPE DIRECTORY FILES "/source/clients/freerdp-1.0.1/keymaps/sgi_vndr" FILES_MATCHING REGEX "/[^/]*$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")


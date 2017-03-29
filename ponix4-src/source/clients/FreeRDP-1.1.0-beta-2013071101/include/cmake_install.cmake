# Install script for directory: /source/clients/FreeRDP-1.1.0-beta-2013071101/include

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE FILE FILES
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/primary.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/freerdp.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/message.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/event.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/extension.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/listener.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/dvc.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/rail.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/input.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/pointer.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/error.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/scancode.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/client.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/peer.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/graphics.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/svc.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/primitives.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/update.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/altsec.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/settings.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/types.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/window.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/constants.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/secondary.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/addin.h"
    "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/api.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/cache" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/codec" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/crypto" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/gdi" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/locale" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/rail" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/utils" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/client" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/server" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/FreeRDP-1.1.0-beta-2013071101/include/freerdp/channels" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")


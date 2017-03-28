# Install script for directory: /source/clients/freerdp-git-17102014/include

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE FILE FILES
    "/source/clients/freerdp-git-17102014/include/freerdp/primary.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/freerdp.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/metrics.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/message.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/event.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/extension.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/codecs.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/listener.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/version.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/dvc.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/rail.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/input.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/pointer.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/error.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/scancode.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/client.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/peer.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/graphics.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/assistance.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/svc.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/primitives.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/update.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/altsec.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/settings.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/types.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/window.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/constants.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/secondary.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/log.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/addin.h"
    "/source/clients/freerdp-git-17102014/include/freerdp/api.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE FILE FILES "/source/clients/freerdp-git-17102014/include/freerdp/version.h")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/cache" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/codec" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/crypto" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/gdi" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/locale" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/rail" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/utils" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/client" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/server" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-17102014/include/freerdp/channels" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")


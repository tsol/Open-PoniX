# Install script for directory: /source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include

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
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/primary.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/freerdp.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/extension.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/listener.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/dvc.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/rail.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/input.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/pointer.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/scancode.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/peer.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/graphics.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/svc.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/update.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/altsec.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/settings.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/types.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/errorcodes.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/window.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/constants.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/secondary.h"
    "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/api.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/cache" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/codec" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/crypto" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/gdi" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/locale" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/rail" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/utils" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/client" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/server" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freerdp" TYPE DIRECTORY FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/include/freerdp/channels" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")


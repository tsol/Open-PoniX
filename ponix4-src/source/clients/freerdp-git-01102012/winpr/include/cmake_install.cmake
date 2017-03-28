# Install script for directory: /f/winpr/include

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/freerdp")
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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/winpr" TYPE FILE FILES
    "/f/winpr/include/winpr/sam.h"
    "/f/winpr/include/winpr/file.h"
    "/f/winpr/include/winpr/memory.h"
    "/f/winpr/include/winpr/thread.h"
    "/f/winpr/include/winpr/winpr.h"
    "/f/winpr/include/winpr/environment.h"
    "/f/winpr/include/winpr/print.h"
    "/f/winpr/include/winpr/sspicli.h"
    "/f/winpr/include/winpr/ndr.h"
    "/f/winpr/include/winpr/synch.h"
    "/f/winpr/include/winpr/interlocked.h"
    "/f/winpr/include/winpr/timezone.h"
    "/f/winpr/include/winpr/bcrypt.h"
    "/f/winpr/include/winpr/error.h"
    "/f/winpr/include/winpr/rpc.h"
    "/f/winpr/include/winpr/windows.h"
    "/f/winpr/include/winpr/handle.h"
    "/f/winpr/include/winpr/wtypes.h"
    "/f/winpr/include/winpr/sysinfo.h"
    "/f/winpr/include/winpr/asn1.h"
    "/f/winpr/include/winpr/security.h"
    "/f/winpr/include/winpr/ntlm.h"
    "/f/winpr/include/winpr/registry.h"
    "/f/winpr/include/winpr/winsock.h"
    "/f/winpr/include/winpr/config.h"
    "/f/winpr/include/winpr/spec.h"
    "/f/winpr/include/winpr/string.h"
    "/f/winpr/include/winpr/library.h"
    "/f/winpr/include/winpr/io.h"
    "/f/winpr/include/winpr/heap.h"
    "/f/winpr/include/winpr/crt.h"
    "/f/winpr/include/winpr/path.h"
    "/f/winpr/include/winpr/midl.h"
    "/f/winpr/include/winpr/tchar.h"
    "/f/winpr/include/winpr/winhttp.h"
    "/f/winpr/include/winpr/dsparse.h"
    "/f/winpr/include/winpr/sspi.h"
    "/f/winpr/include/winpr/stream.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")


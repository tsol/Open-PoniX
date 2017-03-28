# Install script for directory: /source/clients/freerdp-git-07062016/winpr/include

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/winpr2/winpr" TYPE FILE FILES
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/platform.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/sam.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/file.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/memory.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/thread.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/winpr.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/environment.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/smartcard.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/locale.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/print.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/endian.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/intrin.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/sspicli.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/ssl.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/ndr.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/synch.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/version.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/schannel.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/interlocked.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/nt.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/user.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/crypto.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/timezone.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/pipe.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/debug.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/bcrypt.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/input.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/error.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/rpc.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/windows.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/pool.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/handle.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/wtypes.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/wtsapi.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/bitstream.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/cmdline.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/sysinfo.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/pack.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/asn1.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/security.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/credui.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/ntlm.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/image.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/registry.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/winsock.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/ini.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/credentials.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/comm.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/spec.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/string.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/wnd.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/collections.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/library.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/io.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/clipboard.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/heap.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/crt.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/path.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/shell.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/midl.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/tchar.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/winhttp.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/dsparse.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/sspi.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/stream.h"
    "/source/clients/freerdp-git-07062016/winpr/include/winpr/wlog.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/winpr2/winpr" TYPE FILE FILES "/source/clients/freerdp-git-07062016/winpr/include/winpr/version.h")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/winpr2/winpr" TYPE FILE FILES "/source/clients/freerdp-git-07062016/winpr/include/winpr/wtypes.h")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/winpr2/winpr" TYPE DIRECTORY FILES "/source/clients/freerdp-git-07062016/winpr/include/winpr/tools" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")


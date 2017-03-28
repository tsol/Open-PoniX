# Install script for directory: /source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/client/X11

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
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/xfreerdp" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/xfreerdp")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/xfreerdp"
         RPATH "$ORIGIN/../lib:/usr/xorg/lib")
  ENDIF()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/client/X11/xfreerdp")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/xfreerdp" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/xfreerdp")
    FILE(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/xfreerdp"
         OLD_RPATH "/usr/xorg/lib:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/client/common:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/libfreerdp/core:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/libfreerdp/gdi:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/libfreerdp/locale:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/libfreerdp/rail:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/libfreerdp/utils:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/interlocked:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/libfreerdp/cache:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/libfreerdp/crypto:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/library:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/libfreerdp/codec:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/dsparse:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/sspi:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/registry:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/utils:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/sysinfo:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/crt:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/synch:/source/clients/freerdp-git-11032013-bmiklautz-numpadpatch/winpr/libwinpr/handle:"
         NEW_RPATH "$ORIGIN/../lib:/usr/xorg/lib")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/toolchain/i486-TSOL-linux-gnu/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/xfreerdp")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")


# Install script for directory: /source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr

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

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/timezone/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/winsock/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/sspi/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/credui/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/handle/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/asn1/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/crypto/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/pipe/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/dsparse/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/input/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/crt/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/pool/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/path/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/io/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/synch/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/credentials/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/utils/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/sysinfo/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/file/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/error/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/interlocked/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/rpc/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/registry/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/thread/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/bcrypt/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/environment/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/library/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/sspicli/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/heap/cmake_install.cmake")
  INCLUDE("/source/clients/FreeRDP-1.1.0-beta-2013071101/winpr/libwinpr/winhttp/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)


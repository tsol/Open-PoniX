# Install script for directory: /f/winpr/libwinpr

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

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/f/winpr/libwinpr/crt/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/utils/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/heap/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/path/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/io/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/file/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/error/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/environment/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/interlocked/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/handle/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/synch/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/thread/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/sysinfo/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/bcrypt/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/dsparse/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/asn1/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/rpc/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/sspicli/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/sspi/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/registry/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/library/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/timezone/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/winsock/cmake_install.cmake")
  INCLUDE("/f/winpr/libwinpr/winhttp/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)


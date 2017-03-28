# Install script for directory: /source/clients/freerdp-git-25102013/channels

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
  INCLUDE("/source/clients/freerdp-git-25102013/channels/parallel/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/sample/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/tsmf/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/cliprdr/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/rail/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/drive/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/audin/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/drdynvc/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/rdpsnd/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/rdpei/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/rdpdr/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/disp/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/serial/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/echo/cmake_install.cmake")
  INCLUDE("/source/clients/freerdp-git-25102013/channels/client/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)


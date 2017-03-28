#!/bin/sh

alias ll='ls -l'

export LANGUAGE="en_US.UTF-8"
export LANG="en_US.UTF-8"
export LC_ALL="en_US.UTF-8"
export LC_CTYPE="en_US.UTF-8"
export LC_MESSAGES="en_US.UTF-8"
export MM_CHARSET="UTF-8"

export CC="i486-TSOL-linux-gnu-gcc"
export CXX="i486-TSOL-linux-gnu-g++"

export LD="i486-TSOL-linux-gnu-ld"

#export LD="i486-TSOL-linux-gnu-gcc"
export CMAKE_PREFIX_PATH="/usr/xorg"
export PKG_CONFIG_PATH=/usr/xorg/lib/pkgconfig:/usr/xorg/share/pkgconfig:${PKG_CONFIG_PATH}

export LDFLAGS=-static-libgcc
export CFLAGS=-Os
export CXXFLAGS=-Os



echo -e "\nWelcome to the open-poniX 4.0 Build Enviroment\n" 

bash

echo -e "\nYou have exited the open-poniX 4.0 Build Environment\n"

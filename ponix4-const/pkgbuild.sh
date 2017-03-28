#! /bin/bash

BASEDIR=/TSOL/PoniX-4/
COPYTO=/tftpboot/ponix4/pkg/

USER=$PONIX_GOD
if [ -z "$USER" ]; then
	USER=kolyan
fi

if [ -z "$1" ]; then 
	echo "Usage: $0 [packagename]";
	exit 1;
fi

if test ! -d packages/$1 ; then
	echo "No package '$1' exist!"
	exit
fi

# [version control] ###############################################################
        
BASEVER=`cat ${BASEDIR}versions/$1.base`
CURBUILD=`cat ${BASEDIR}versions/$1.build`

if [ -z $CURBUILD ]; then
	echo WARNING, build version is empty! Setting it = 1!
	CURBUILD=1
fi

if [ -z $BASEVER ]; then
	echo WARNING, base version is empty! Setting it = 1!
	BASEVER=1
fi

VERSION=${BASEVER}.${CURBUILD}

echo
echo ==============================================================================
echo Package: $1, Upload user: $USER, Building version: $VERSION
echo ==============================================================================
echo
echo "type a few words describing change and hit enter:"

read Z

echo `date +"%F %R"` - $VERSION - $Z >> "versions/$1_history.txt"
echo $VERSION > "${BASEDIR}versions/$1_version.txt"

NEWVER=`expr $CURBUILD + 1`
echo $NEWVER > "${BASEDIR}versions/$1.build"
echo $BASEVER > "${BASEDIR}versions/$1.base"

# clean up old package in dest. folder

rm -r -f ${COPYTO}$1.px
cd ${BASEDIR}packages/$1

# store history and current package version inside of the package itself.

if [ ! -d "var" ] ; then mkdir var ; fi
cd var
if [ ! -d "packages" ] ; then mkdir packages ; fi
cd packages
cat ${BASEDIR}versions/$1_version.txt > $1.loaded

# archive all into px

cd ${BASEDIR}packages/$1
tar -cJf ${COPYTO}$1.px *

echo
echo "Done. New package is here '${COPYTO}$1.px'"
echo


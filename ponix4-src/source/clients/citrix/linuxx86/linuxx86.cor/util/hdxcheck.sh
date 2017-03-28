#!/bin/bash
#############################################################
# Citrix HDX Pre-requisite Check for the Receiver for Linux #
#############################################################
# Copyright 2011-2013 Citrix Systems, Inc.  All Rights Reserved.

# Usage: ./hdxcheck.sh 
#
# How to use: 1.) Get the Latest receiver Tar.gz archive from Citrix.com
#             2.) Extract the archive to /tmp/
#             3.) Give this script execute permissions
#             4.) Run the script
#
# Note: If you wish to extract the client to a different folder, just change the 
# 	$ClientPath variable below to wherever the client is.
#
# Output: Results are shown on stdout and a log file is produced
#
# Script dependencies(any version): 
# bash
# awk
# curl
# ldd
#
# This script checks for the following requirenments
# Kernel: 2.6.29, ALSA(libasound2), Speex and Vorbis codec libraries
# Gstreamer 10.25, Flash plugin, LibCurl 7.18.2, LibPCSCLite 1.5.6 and Gtk 2.12
# As well as the associated binary dependencies of these libraries
# USB support is also checked. 
# It also checks for multimonitor and seamless window support

logfile=/tmp/logfile.txt
rm -rf /tmp/logfile.txt
clear

DefaultRootClientPath=/opt/Citrix/ICAClient
DefaultUserClientPath=`echo $HOME/ICAClient/linux*`

#if ICAROOT is set use it, otherwise use a default client location
if [ ! "$ICAROOT" ]
then
    if [ -d "$DefaultRootClientPath" ]
    then
        ClientPath="$DefaultRootClientPath"
    elif [ -d "$DefaultUserClientPath" ]
    then
        ClientPath="$DefaultUserClientPath"
    else
        echo "Please export ICAROOT no default installation could be found." | tee -a $logfile
        exit 1      
    fi
else
    if [ ! -d "$ICAROOT" ]
    then
        echo "Invalid ICAROOT - $ICAROOT: No such file or directory" | tee -a $logfile
        exit 1
    else
        ClientPath="$ICAROOT"
    fi
fi

LibPath="/usr/lib"
IncludePath="/usr/include"

initializeANSI()
{
  esc=""

  whitef="${esc}[37m"   blueb="${esc}[44m"; 
  boldon="${esc}[1m";    boldoff="${esc}[22m"
  italicson="${esc}[3m"; italicsoff="${esc}[23m"
  reset="${esc}[0m"
}

initializeANSI
versionCompare() {
  if [[ $1 == $2 ]]
    then
        return '0'
    fi
    local IFS=.
    local i ver1=($1) ver2=($2)
    # fill empty fields in ver1 with zeros
    for ((i=${#ver1[@]}; i<${#ver2[@]}; i++))
    do
        ver1[i]=0
    done
    for ((i=0; i<${#ver1[@]}; i++))
    do
        if [[ -z ${ver2[i]} ]]
        then
            # fill empty fields in ver2 with zeros
            ver2[i]=0
        fi
        if ((${ver1[i]} > ${ver2[i]}))
        then
            return '1'
        fi
        if ((${ver1[i]} < ${ver2[i]}))
        then
            return '0' #0 if ver2 is bigger
        fi
    done
    return '0'
}

check_lib()
{
    local libname="$1" goodmsg="$2" badmsg="$3"
    local libs=$( (find /usr/lib -name '*'$libname'*' -a -type f; find /lib -name '*'$libname'*' -a -type f) | xargs file 2>/dev/null | grep 'ELF 32-bit LSB shared object' | cut -d: -f1 )
    echo $libs >> $logfile
    if [ -n "$libs" ]
    then
        if [ "$goodmsg" ]; then echo "$goodmsg" | tee -a $logfile; fi
        LibFound="1"
    else
        if [ "$badmsg"  ]; then echo "$badmsg"  | tee -a $logfile; fi
        Problem="1"
        LibFound="0"
    fi
}

echo "${blueb}${boldon}${whitef}=========================================================" 
echo "= Pre-requisite check for the Citrix Receiver for Linux =" | tee -a $logfile
echo "=                 Version 1.3 (June 2013)               =" | tee -a $logfile
echo "=========================================================" | tee -a $logfile
echo "${reset}"

echo "-------------------------------" | tee -a $logfile
echo "-- Client Information...      -" | tee -a $logfile
echo "-------------------------------" | tee -a $logfile

echo "Client version:"                 | tee -a $logfile
archVer=`uname -m | grep arm`
if [ $? != "0" ]
then
    #must be intel
    echo `awk -F = '/^ID_VERSION/ { print $2 }' "$ClientPath/pkginf/Ver.core.linuxx86"` | tee -a $logfile
else
    #it's ARM, but is it hf or el?
    if [ -f /proc/self/maps ]; then
        if grep "gnueabihf\|armhf" /proc/self/maps 2>&1 > /dev/null ;then
            PLATFORM=linuxarmhf
        elif ls /lib/ld-linux* | grep armhf 2>&1 > /dev/null ;then
            PLATFORM=linuxarmhf
        else
            PLATFORM=linuxarm
        fi
    else
        # Assume armel for the moment
        PLATFORM=linuxarm
    fi
    echo `awk -F = '/^ID_VERSION/ { print $2 }' "$ClientPath/pkginf/Ver.core.$PLATFORM"` | tee -a $logfile
fi

echo "Install location:"               | tee -a $logfile
echo "$ClientPath"                     | tee -a $logfile
echo ""                                | tee -a $logfile

echo "-------------------------------" | tee -a $logfile
echo "-- Checking Kernel Version... -" | tee -a $logfile
echo "-------------------------------" | tee -a $logfile
KernelMinVersion="2.6.29"

KernelVersion=`uname -r | sed "s/^\([[:digit:]]*\.[[:digit:]]*\.[[:digit:]]*\).*/\1/"`
versionCompare $KernelMinVersion $KernelVersion
if [ $? -eq "0" ]; then
    echo "Success, a compatible kernel version($KernelVersion) has been found." | tee -a $logfile
else
    echo "Warning! - you are using Kernel version $KernelVersion" | tee -a $logfile
    echo "Versions prior to 2.6.29 contain a bug in the isochronous USB code that may cause"
    echo "your computer to freeze when using USB media devices like webcams or audio equipment"
fi
###################################################################

###################################################################

echo "${blueb}${boldon}${whitef}--------------------------------------------" 
echo "-- Checking Audio support dependencies..   -" | tee -a $logfile
echo "--------------------------------------------" | tee -a $logfile
echo "${reset}"
###################################################################

echo "------------------------------------------------------------------" | tee -a $logfile
echo "-- Checking that ALSA Support is installed..." | tee -a $logfile
echo "------------------------------------------------------------------" | tee -a $logfile
AlsaVersion=`ldd $ClientPath/wfica | grep asound | grep "not found"`
if [ $? != "0" ] 
then
	echo "Success! - ALSA is installed!" | tee -a $logfile
else
	echo "Warning! - A compatible version of ALSA could not be located! Client Audio will not function correctly without ALSA  installed." | tee -a $logfile
fi
echo ""

###################################################################

###################################################################

echo "-----------------------------------------------------" | tee -a $logfile
echo "-- Checking for SPEEX and VORBIS Audio Support..." | tee -a $logfile
echo "-----------------------------------------------------" | tee -a $logfile
Problem=0


ldd "$ClientPath/SPEEX.DLL" | grep "not found"
if [ $? -eq "0" ] ; then echo "Error! - SPEEX dependencies missing!" | tee -a $logfile ; Problem="1" ; fi
ldd "$ClientPath/VORBIS.DLL" | grep "not found"
if [ $? -eq "0" ] ; then echo "Error! - VORBIS dependencies missing!" | tee -a $logfile ; Problem="1" ; fi
if [ $Problem -eq "0" ] ; then echo "Success! All SPEEX and VORBIS dependencies found!" | tee -a $logfile ; fi
echo ""

###################################################################

echo "${blueb}${boldon}${whitef}--------------------------------------------" 
echo "-- Checking HDX MediaStream dependencies.. -" | tee -a $logfile
echo "--------------------------------------------" | tee -a $logfile
echo "${reset}"
GstreamerMinVersion="10.25"
GstreamerVersion=`$ClientPath/util/gst_play --gst-version | sed -n "s/^.*\.\(.*\..*\)$/\1/p"` 
echo "The minimum required version of GStreamer is $GstreamerMinVersion" | tee -a $logfile
echo "You are using version $GstreamerVersion of GStreamer" | tee -a $logfile
versionCompare $GstreamerMinVersion $GstreamerVersion
if [ $? -eq "0" ] 
then 
	echo "Success! - A compatible version of GStreamer is installed!" | tee -a $logfile
else
	echo "Warning! - A compatible version of GStreamer could not be located!" | tee -a $logfile
fi
echo ""

echo "------------------------------------------------------------------" | tee -a $logfile
echo "-- Checking FLASH Support ..." | tee -a $logfile
echo "------------------------------------------------------------------" | tee -a $logfile

#check flash plugin is present
if [ -f "/usr/lib/browser-plugins/libflashplayer.so" -o -f "/usr/lib/flashplugin-installer/libflashplayer.so" -o \
    -f "/usr/lib/adobe-flashplugin/libflashplayer.so" -o -f "/usr/lib/mozilla/plugins/libflashplayer.so" -o \
    -f "/usr/lib/opera/plugins/libflashplayer.so" -o -f "/usr/lib/flash-plugin/libflashplayer.so" -o\
    -f "/usr/lib/firefox/plugins/libflashplayer.so" -o -f "$ClientPath/libflashplayer.so" ] 
then

	echo "Success! - Adobe Flash plugin found!" | tee -a $logfile
        echo "Make sure your plugin's version is 10.0 or later."
        echo "You can check this by going into Firefox and typing: about:plugins in the address bar."
else
        echo "Warning! - Adobe Flash plugin is missing." | tee -a $logfile
fi
echo ""

if [ -f $ClientPath/FlashContainer.bin ]
then
    FlashVersion=`ldd $ClientPath/FlashContainer.bin | grep "not found"`
    if [ $? != "0" ] 
    then
	    echo "Success! - Flash dependencies installed!" | tee -a $logfile
    else
	    echo "Warning! - Some dependencies required for Flash are missing." | tee -a $logfile
            ldd $ClientPath/FlashContainer.bin | grep "not found" | tee -a $logfile
    fi
else
    echo "$ClientPath/FlashContainer.bin not found" | tee -a $logfile
    echo "Warning! - Flash dependencies not available. " | tee -a $logfile
fi

echo ""

curl -V > /dev/null 2>&1
if [ $? != 0 ] 
then
    #curl not found
    echo "The curl utility is not installed in your system." | tee -a $logfile 
    echo "It's impossible to check if you have the required version of libcurl."| tee -a $logfile 
    echo "This should be at least version 7.18.2" | tee -a $logfile  
else
               
    libCurlVer=`curl --version | awk 'NR==1 {print substr($4, 9)}'`

        echo "Your version of libcurl is: $libCurlVer" | tee -a $logfile
        versionCompare "7.18.2" $libCurlVer
        if [ $? -eq 0 ]; then
            echo "Sucesss! Correct libCurl version found"
        else
            echo "Warning! - Your version of libcurl is lower than the required 7.18.2 for Flash to work"
        fi

fi
echo ""


###################################################################

###################################################################

echo "------------------------------------------------------------------" | tee -a $logfile
echo "-- Checking GStreamer webcam Support ..." | tee -a $logfile
echo "------------------------------------------------------------------" | tee -a $logfile
Problem="0"
GstPlay=`ldd $ClientPath/util/gst_play | grep "not found"`
if [ $? != "0" ]; then
    Problem="1"
fi
GstPlay=`ldd $ClientPath/util/gst_read| grep "not found"`
if [ $? != "0" ]; then
    Problem="1"
fi

if [ $Problem -eq "1" ]
then
	echo "Success! - Gstreamer webcam dependencies installed!" | tee -a $logfile
else
	echo "Warning! - some dependencies required for webcam support are missing." | tee -a $logfile
        ldd $ClientPath/util/gst_play| grep "not found" | tee -a $logfile
        ldd $ClientPath/util/gst_read| grep "not found" | tee -a $logfile
fi

echo ""
###################################################################
echo "${blueb}${boldon}${whitef}--------------------------------------${reset}" 
echo "${blueb}${boldon}${whitef}-- Checking for printing support ... -${reset}" 
echo "${blueb}${boldon}${whitef}--------------------------------------${reset}" 

if  which lpstat > /dev/null 2>&1 
then
    echo "Success! - Printing system found" | tee -a $logfile
    echo ""
    echo "The following printers are installed in your system: "
    lpstat -a
else
    ls /etc/printcap > /dev/null 2>&1
    
    if [ -f /etc/printcap ]
        then
            echo "Success! - Printing system found" | tee -a $logfile
            echo ""
            echo "The following printers are installed in your system: "
            cat /etc/printcap | awk ' substr($1,1,1) !="#" {print $1}'
        else
            echo "Warning! - No printing system found" | tee -a $logfile
    fi

fi

echo ""
####################################################################
echo "${blueb}${boldon}${whitef}-----------------------------------${reset}" 
echo "${blueb}${boldon}${whitef}-- Checking Smartcard Support ... -${reset}" 
echo "${blueb}${boldon}${whitef}-----------------------------------${reset}" 

#need to check version of pcsclite
check_lib "libpcsclite.so" "Success! - Libpcsclite.so installed. Smartcard support enabled." "Warning! - libpcsclite.so missing, check that the file exists."
if [ $LibFound -eq "1" ]
then
    if [ -f "$IncludePath/PCSC/pcsclite.h" ]  #pcsc compile from source
    then 
        pcscVer=`cat $IncludePath/PCSC/pcsclite.h | grep VERSION | awk ' {print substr($3,2,5)} '`
        echo "Your version of libpcsclite is: $pcscVer" | tee -a $logfile
        versionCompare "1.5.6" $pcscVer
        if [ $? -eq 0 ]; then
            echo "Sucesss! Correct pcsclite version found"
        else
            echo "Warning! - Your version of pcsclite is lower than the required 1.5.6"
        fi
    else #pcsc not compiled from source
       echo "libpcsclite was found on your system but it's version could not be determined."
       echo "Make sure you have version 1.5.6 or later"
    fi
fi
echo ""

echo "${blueb}${boldon}${whitef}------------------------------------${reset}" 
echo "${blueb}${boldon}${whitef}--- Checking other dependencies .. -${reset}" 
echo "${blueb}${boldon}${whitef}------------------------------------${reset}" 
echo ""

########################################################

Problem=0
echo "-----------------------------------------------------" | tee -a $logfile
echo "-- Checking Client Library dependencies..." | tee -a $logfile
echo "-----------------------------------------------------" | tee -a $logfile
files=`find $ClientPath | xargs file 2>/dev/null | grep "LSB shared object" | sort | awk -F ':' {'print $1'}`  | tee -a $logfile
for file in $files ; do
    if  [ "$file" == "./util/gst_play" -o "$file" == "./util/gst_read" ]; then
        continue
    fi
    
    Lib=`ldd $file | grep "not found"` | tee -a $logfile
    Lib=`ldd $file | grep "not found"`
    if [ $? -eq "0" ] ; then echo "Error! - $file dependencies missing!" | tee -a $logfile ; Problem="1" ; fi
done

if [ $Problem -eq "0" ] ; then echo "Success! All client dependencies found!" | tee -a $logfile ; fi
echo ""

########################################################
echo "-----------------------------------------------------" | tee -a $logfile
echo "-- Checking OS Library dependencies..." | tee -a $logfile
echo "-----------------------------------------------------" | tee -a $logfile
Problem=0

check_lib "libcrypto.so"  "" "Warning! - libcrypto.so is not installed. This is required if you use NTLM proxies."
check_lib "libjpeg.so"    "" "Warning! - libjpeg.so is not installed! This is needed for Speedscreen Image and Browser Acceleration"
check_lib "libldapsdk.so" "" "Warning! - libldapsdk.so is not installed! This is only needed if you use Novell Netware Services."

if [ $Problem -eq "0" ] ; then echo "Success! All OS dependencies found!" | tee -a $logfile ; fi

echo ""
###################################################################
echo "-----------------------------" | tee -a $logfile
echo "-- Checking For USB Support.." | tee -a $logfile
echo "-----------------------------" | tee -a $logfile
Problem="0"

echo ""

echo "--------------------------------------------------------------" | tee -a $logfile
echo "-- Checking that a compatible version of udev is installed..." | tee -a $logfile
echo "--------------------------------------------------------------" | tee -a $logfile
ps ax | awk '{print $5}' | grep udevd >> $logfile
if [ $? -eq "0" ]
then 
	echo "A compatible version of udev is installed!" | tee -a $logfile
else
	echo "A compatible version of udev could not be located!" | tee -a $logfile
	Problem="1"
fi
echo ""

echo "---------------------------------------------------------------" | tee -a $logfile
echo "-- Checking that a compatible version of glibc is installed..." | tee -a $logfile
echo "---------------------------------------------------------------" | tee -a $logfile

glibc_min_version="2.7"

if [ -d "/lib/i386-linux-gnu" ]
then
    cd /lib/i386-linux-gnu
elif [ -d "/lib/arm-linux-gnueabihf" ]
then
    cd /lib/arm-linux-gnueabihf
elif [ -d "/lib/arm-linux-gnueabi" ]
then
    cd /lib/arm-linux-gnueabi
else
    cd /lib
fi

glibc_version=`ls *libc-2* | sed "s/.*libc-// ; s/\.so//"`
echo "The minimum required version is $glibc_min_version" | tee -a $logfile
echo "You are using version $glibc_version of glibc" | tee -a $logfile

versionCompare $glibc_min_version $glibc_version
if [ $? -eq "0" ]
then 
	echo "A compatible version of glibc is installed!" | tee -a $logfile
else
	echo "A compatible version of glibc could not be located!" | tee -a $logfile
	Problem="1"
fi

echo ""

echo "----------------------------------------------------------------" | tee -a $logfile
echo "-- Checking that a compatible version of libcap is installed..." | tee -a $logfile
echo "----------------------------------------------------------------" | tee -a $logfile

check_lib "libcap.so" "A compatible version of libcap is installed!" "A compatible version of libcap could not be located!"

if [ $Problem -eq "0" ] ; then echo "Success! All USB dependencies found!" | tee -a $logfile 
else echo "Warning - Some USB dependencies are missing!" | tee -a $logfile ; fi

###################################################################
echo ""
echo "${blueb}${boldon}${whitef}-----------------------------------${reset}" 
echo "${blueb}${boldon}${whitef}- Checking X-window dependencies  -${reset}"
echo "${blueb}${boldon}${whitef}-----------------------------------${reset}"

echo "-----------------------------------------------------------------" | tee -a $logfile
echo "-- Checking that your system supports the minimum color depth..." | tee -a $logfile
echo "-----------------------------------------------------------------" | tee -a $logfile
mincolordepth="8"
sufficientdepth="0"
bestdepth="0"

if which xdpyinfo > /dev/null 2>&1 
then

    for line in `xdpyinfo | grep depth: | awk '{print "" $2}'` ; do
	    if [ $line -ge $mincolordepth ]
	    then 
		    sufficientdepth="1"
		    if [ $line -ge $bestdepth ]
		    then
			    bestdepth="$line"
		    fi
	    fi
    done

    echo "The minimum required color depth is $mincolordepth bit" | tee -a $logfile
    echo "Your system supports up to $bestdepth bit" | tee -a $logfile
    if [ $sufficientdepth -eq "1" ]
    then 
	    echo "Success! - A compatible color depth is available!" | tee -a $logfile
    else
	    echo "Error! - A compatible color depth is NOT available!" | tee -a $logfile
    fi

else 
    echo "Warning! - xdpyinfo is missing from your system." | tee -a $logfile
    echo "Cannot determine if you system supports the minimum color depth." | tee -a $logfile

fi #xdpyinfo check if ends

###################################################################
echo ""
echo "------------------------------------------------------------" | tee -a $logfile
echo "-- Checking that your XServer supports Seamless Windows..." | tee -a $logfile
echo "------------------------------------------------------------" | tee -a $logfile

if which xprop > /dev/null 2>&1
then

    xprop -root | grep _NET_SUPPORTED > /tmp/xprop.txt

    NotFound="0"

    stdout=`grep '_NET_WM_STATE_MAXIMIZED_HORZ' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WM_STATE_MAXIMIZED_HORZ' /tmp/xprop.txt`
    if [ $? -ne "0" ] ; then NotFound="1" ; fi
    stdout=`grep '_NET_WM_STATE_MAXIMIZED_VERT' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WM_STATE_MAXIMIZED_VERT' /tmp/xprop.txt`
    if [ $? -ne "0" ] ; then NotFound="1" ; fi
    stdout=`grep '_NET_WORKAREA' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WORKAREA' /tmp/xprop.txt`
    if [ $? -ne "0" ] ; then NotFound="1" ; fi
    stdout=`grep '_NET_WM_STATE_SKIP_TASKBAR' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WM_STATE_SKIP_TASKBAR' /tmp/xprop.txt` 
    if [ $? -ne "0" ] ; then NotFound="1" ; fi
    stdout=`grep '_NET_WM_STATE_SKIP_PAGER' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WM_STATE_SKIP_PAGER' /tmp/xprop.txt`
    if [ $? -ne "0" ] ; then NotFound="1" ; fi
    stdout=`grep '_NET_WM_STATE_ABOVE' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WM_STATE_ABOVE' /tmp/xprop.txt`
    if [ $? -ne "0" ] ; then NotFound="1" ; fi
    stdout=`grep '_NET_WM_USER_TIME' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WM_USER_TIME' /tmp/xprop.txt`
    if [ $? -ne "0" ] ; then NotFound="1" ; fi
    stdout=`grep '_NET_WM_MOVERESIZE' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WM_MOVERESIZE' /tmp/xprop.txt`
    if [ $? -ne "0" ] ; then NotFound="1" ; fi
    stdout=`grep '_NET_WORKAREA' /tmp/xprop.txt` | tee -a $logfile
    stdout=`grep '_NET_WORKAREA' /tmp/xprop.txt` 
    if [ $? -ne "0" ] ; then NotFound="1" ; fi


    if [ $NotFound -eq "0" ]
    then 
	    echo "Success! - Seamless windows are supported!" | tee -a $logfile
    else
	    echo "Warning! - Seamless windows are NOT supported!" | tee -a $logfile
    fi

    echo "-------------------------------------------------------" | tee -a $logfile
    echo "-- Checking that your XServer supports Multi-Monitor..." | tee -a $logfile
    echo "-------------------------------------------------------" | tee -a $logfile
    NotFound="0"

    stdout=`grep '_NET_WORKAREA' /tmp/xprop.txt`
    echo $stdout >> $logfile

    if [ "$stdout" ]
    then 
	    
        stdout=`grep '_NET_WM_FULLSCREEN_MONITORS' /tmp/xprop.txt`
        echo $stdout >> $logfile

        if [ "$stdout" ]
        then
            echo "Success! - Multi-monitor is supported!" | tee -a $logfile
        else
            echo "Warning! - Your window manager does not support"  | tee -a $logifle
            echo "multi-monitor fullscreen windows." | tee -a $logfile
            echo "This will cause difficulties for multi-monitor"
            echo "fullscreen sessions which are not seamless."
            echo ""
        fi
        echo "Found the following Monitors:" | tee -a $logfile
	    xdpyinfo -ext XINERAMA | grep "head #" | tee -a $logfile    
    else
	    echo "Warning! - Multimonitor is not supported!" | tee -a $logfile
    fi
    rm -rf xprop.txt
    echo ""

else
    echo "Warning! - xprop is missing from your system." | tee -a $logfile
    echo "Cannot determine if your system support seamless windows or multimonitor." | tee -a $logfile

fi # from if xprop exists 

###################################################################
echo ""
echo "${blueb}${boldon}${whitef}===================================================================${reset}" 
echo "${blueb}${boldon}${whitef}= Pre-requisite check for the Citrix Receiver for Linux Completed =${reset}"
echo "${blueb}${boldon}${whitef}===================================================================${reset}"
echo "Logfile: $logfile"




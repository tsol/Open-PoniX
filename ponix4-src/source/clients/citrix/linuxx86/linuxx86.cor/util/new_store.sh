#!/bin/sh
# Used by the plugin to handle CR files
# Pass in the FULL path to the CR file as the parameter
#
# Copyright 2013 Citrix Systems, Inc.  All Rights Reserved.

BASEDIR=$(dirname $0)
STORERESULT=`$BASEDIR/storebrowse -C "$*"`
STORERETCODE=$?

# Check for successful return code
if [ "$STORERETCODE" = "0" ]
then
  # and if we have added stores, then launch selfservice
    STORECOUNT=`echo $STORERESULT | grep -c "'"`
    if [ "$STORECOUNT" != "0" ]
    then
	$BASEDIR/../selfservice &
    fi
fi

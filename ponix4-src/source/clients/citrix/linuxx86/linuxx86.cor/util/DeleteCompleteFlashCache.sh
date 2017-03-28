#!/bin/bash
# Flash container related Cache files are kept at $ICARoot/CtxFlashCache
# and they are used by container process i.e. FlashContainer.bin. In order to clean up
# the cache please make sure there are no process which is using cache, else the behaviour
# of the user container processes are undefined

ps -ef | grep 'FlashContainer.bin /tmp/Ctx' | grep -v grep > /dev/null
retVal=$?

if [ $retVal -eq 0 ]
then
    echo " WARNING : Some of FlashContainer.bin processes are still using cache."
	echo "	   Please terminate them before removing cache."
else
    rm -rf $HOME/.ICAClient/CtxFlashCache
	echo "$HOME/.ICAClient/CtxFlashCache has been deleted."
fi

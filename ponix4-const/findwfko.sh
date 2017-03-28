#!/bin/sh

# FINDS ALL WIFI KERNEL MODULES AND COLLECTS THEM TO ONE FOLDER
#
# (prepares them for wireless.pkg)
#

#BASEPATH=/build/lib/modules/3.2.12-poniX-2.2/updates/
#DEPPATH=/build/lib/modules/3.2.12-poniX-2.2/updates/
#DEPPATH1=/const/kernel/modules/kernel/

BASEPATH=/const/kernel/modules/kernel/
DEPPATH=/const/kernel/modules/kernel/

TMP=/tmp/findwfko
RES_MODULES=/const/packages/wireless/lib/modules/
RES_LIST=/const/packages/wireless/etc/wireless.modules

rm -rf $RES_MODULES
mkdir $RES_MODULES
rm $RES_LIST

find_and_add_module ()
{
	DEPDIR=$2
	if [ -z "$DEPDIR" ] ; then
		DEPDIR=$DEPPATH
	fi
	
	find $DEPDIR -name "$1.ko" -exec cp {} $RES_MODULES \;
}

copy_deps_for_module ()
{
	(modinfo $1 | grep "depends:" | sed 's/^depends:[[:space:]]*//g' | sed s/,/\\n/g) |
	while read mod
	do
		if [ -n "$mod" ] ; then

			if [ -e "$RES_MODULES/$mod.ko" ] ; then
				echo "ALLREADY THERE: $mod"			
				continue
			fi

			echo "ADDING DEP MODULE: $mod"
			find_and_add_module $mod
			
			if [ ! -e "$RES_MODULES$mod.ko" ] ; then
				echo "WARNING! Wasn't found DEP module '$mod' in '$DEPPATH'"
				
				if [ -n "$DEPPATH1" ] ; then
					find_and_add_module $mod $DEPPATH1
					if [ ! -e "$RES_MODULES$mod.ko" ] ; then
						echo "ERROR! Wasn't found DEP module '$mod' in '$DEPPATH1'"
					fi
				else
					echo "And thats an ERROR, because DEPPATH1 is undefined :("
				fi
				
			fi
			
		fi
	done
}


process_dependencies ()
{
	cd $RES_MODULES

	(ls *.ko) |
	while read mod
	do
		echo " SEARCHING DEPS FOR '$mod'"
		copy_deps_for_module $mod
	done
}

process_folder ()
{
	rm -rf $TMP
	mkdir $TMP

	find $1 -name "*.*" -exec cp {} $TMP/ \;
	
	cd $TMP
	ls * | sed 's/\.ko//g' >> $RES_LIST
	mv * $RES_MODULES
	
}


process_folder $BASEPATH/net/wireless
process_folder $BASEPATH/net/mac80211
process_folder $BASEPATH/drivers/net/wireless
process_folder $BASEPATH/drivers/staging/rtl8187se
process_folder $BASEPATH/drivers/staging/rtl8192e
process_folder $BASEPATH/drivers/staging/rtl8192u
#process_folder $BASEPATH/drivers/staging/rtl8712
process_folder $BASEPATH/drivers/staging/wlags49_h2
process_folder $BASEPATH/drivers/staging/wlags49_h25
process_folder $BASEPATH/drivers/staging/wlan-ng
process_folder /build/lib/modules/3.2.12-poniX-3.0/mymodules/realtek-wifi
process_folder /build/lib/modules/3.2.12-poniX-3.0/mymodules/ralink-wifi

#process_folder /build/lib/modules/3.2.12-poniX-2.2/updates/

#process_folder compat
#process_folder net
#process_folder drivers


process_dependencies
echo "SECOND DEP RUN..."
process_dependencies
echo "THIRD DEP RUN..."
process_dependencies

#!/bin/sh

. /etc/ponix.env
. $TS_GLOBAL

trap '' 1 2 15 

echo "Ponix Simple Shell 1.0"
echo "Ponix verion: $TS_VERSION, IP: $CLIENT_IP, MAC: $CLIENT_MAC"

do_help() {
	echo "Ponix Simple Shell commands:

show info		some basic summary
show top		run process monitor
show lspci		show connected pci devices
show lsusb		show connected usb devices
show mount		show mounted external disks

log network		view ponix.network
log runtime		view runtime settings
log boot		view boot.log
log messages		view messages
savelog			save logs archive to server or attached flash

edit			edit user settings

net show		show network settings
net restart		restart network
net ping <host>		ping a host

pkg show		show currently loaded packages
pkg load <pkg>		load a package
pkg force <pkg>		force reload of a package

video show		show current video mode
video apply		apply resolution changes
video restart		restart video subsystem (avoid this)

sound edit		edit sound volume levels using alsamixer
sound store		save to tftp sound mixer settings for this terminal
sound store pxeall	save to tftp sound mixer settings for this and similar clients
sound store locally	save sound mixer settings to local flash drive

reboot			reboot thin client
shutdown		shutdown thin client

msg <text>		send user a popup messsage
help			commands reference
quit			exit from pss
"
}


edit_file() {
	FILE=$1
	
	if [ -e /bin/joe ] ; then
		/bin/joe $FILE
		return
	fi

	vi $FILE
}


do_msg() {
	export XD_USE_CONSOLE=""
	/bin/xdmenu_msgbox "$1" &
	export XD_USE_CONSOLE="ON"
	echo "Message sent"
}

do_info(){
echo "PONIX_VERSION = $TS_VERSION"
echo ""

echo "CLIENT_NAME   = $CLIENT_NAME"
echo "CLIENT_MAC    = $CLIENT_MAC"
echo "CLIENT_IP     = $CLIENT_IP"

echo ""

KERNEL=`cat /proc/version | cut -d "(" -f1`
echo "KERNEL        = $KERNEL"

echo "ALSA          = `cat /proc/asound/version`"
echo "FREERDP       = `xfreerdp --version`"

echo ""

CPU=`cat /proc/cpuinfo | grep "model name" | cut -d " " -f4-`
echo "CPU           = $CPU"
echo "CPU_FREQ      = "`/bin/dmidecode -s processor-frequency`

MEMTOTAL=`cat /proc/meminfo | grep MemTotal | awk '{print $2}'`
echo "MEM_TOTAL     = $MEMTOTAL kB"

MEMFREE=`cat /proc/meminfo | grep MemFree | awk '{print $2}'`
echo "MEM_FREE      = $MEMFREE kB"

echo ""
echo "BIOS_VENDOR   = "`/bin/dmidecode -s bios-vendor`
echo "BIOS_VERSION  = "`/bin/dmidecode -s bios-version`
echo "BIOS_DATE     = "`/bin/dmidecode -s bios-release-date`

echo ""
echo "BOARD_MADE_BY = "`/bin/dmidecode -s baseboard-manufacturer`
echo "BOARD_NAME    = "`/bin/dmidecode -s baseboard-product-name`

echo ""
echo "BOARD_VERSION = "`/bin/dmidecode -s baseboard-version`
echo "BOARD_SERIAL  = "`/bin/dmidecode -s baseboard-serial-number`

}

do_video() {

	case "$1" in
	    apply)
		/bin/apply_xrandr
	    ;;
	    restart)
		store_variable XORG_DEATH_COUNT 2 RUNTIME
		kill `pidof Xorg`
	    ;;
	    show)
	    	xrandr
	    ;;
	    *)
		echo "Unknown command 'video $1'. See help."
	    ;;	
	esac

}


do_show() {

	case "$1" in
	    top)
		trap 'kill `pidof top`' 1 2 15
		/bin/sh -c top
		echo ""
		echo ""
	    ;;
	    lspci)
		lspci
	    ;;
	    lsusb)
	    	lsusb
	    ;;
	    mount)
		echo -e "External mounts:\n"
	    	mount | grep -v -E "(rootfs|tmpfs|none|proc|devpts|oldroot)"
	    	echo -e "\nDisk usage:\n"
	    	df
	    ;;
	    info)
	    	do_info
	    ;;
	    *)
		echo "Unknown command 'show $1'. See help."
	    ;;	
	esac

}


do_log() {

	case "$1" in
	    network)
		edit_file /etc/ponix.network
	    ;;
	    runtime)
		edit_file /etc/ponix.runtime
	    ;;
	    boot)
	    	edit_file /var/log/boot.log
	    ;;
	    messages)
	    	edit_file /var/log/messages
	    ;;
	    *)
		echo "Unknown log '$1'. See help."
	    ;;	
	esac

}


do_sound() {

	case "$1" in
	    edit)
		alsamixer
		alsactl store
		echo 'Use "sound store" to permanently save settings.'
	    ;;
	    store)
		MODE=pxemac
		if [ -n "$2" ] ; then MODE=$2 ; fi
		${XDMENU_SCRIPTS}/soundcfg_store $MODE
	    ;;
	    
	    *)
		echo "Allowed sound commands are: edit, store"
	    ;;	
	esac

}


do_pkg() {
	echo "Current PKG_PREFIX='$SERVER_IP/$PKG_PREFIX'"
	case "$1" in
	    load)
		pkg require $2
	    ;;
	    force)
		rm /var/packages/$2.loaded
		pkg require $2
	    ;;
	    
	    show)
		echo -e "Loaded packages:\n"
		cd /var/packages
		ls *.loaded | sed 's/\.loaded//g'
	    ;;
	    
	    *)
		echo "Allowed pkg commands are: load"
	    ;;	
	esac

}

do_net() {

	case "$1" in
	    show)
		ifconfig
	    ;;
	    restart)
		service network restart
	    ;;
	    ping)
		ping $2
	    ;;
	    
	    *)
		echo "Allowed net commands are: show, restart"
	    ;;	
	esac

}

do_quit()
{
	echo "BYE"
	export XD_USE_CONSOLE=""
	exit 0
}

export XD_USE_CONSOLE="ON"

while true ; do
    echo -n "pss> "
    read line

    CMD=`echo $line | awk '{print $1}'`
    ARG0=`echo $line | awk '{print $2}'`
    ARG1=`echo $line | awk '{print $3}'`

    ALLARGS=`echo $line | cut -d " " -f2-`
    
#    echo "CMD='$CMD' ARG0='$ARG0' ARG1='$ARG1'"
    
    case "$CMD" in  
	quit)
		do_quit
    	;;
	exit)
		do_quit
    	;;
	help)
		do_help
    	;;
	net)
		do_net $ARG0 $ARG1
	;;

	pkg)
		do_pkg $ARG0 $ARG1
	;;
	savelog)
		/bin/loger
	;;
	edit)
		edit_file $TS_USER
	;;
	sound)
		do_sound $ARG0 $ARG1
	;;
	log)
		do_log $ARG0
	;;
	reboot)
		reboot
	;;
	shutdown)
		shutdown
	;;
	msg)
		do_msg "$ALLARGS"
	;;
	show)
		do_show $ARG0
	;;
	video)
		do_video $ARG0
	;;
	ping)
		do_net ping $ARG0
	;;
	ls)
		do_help
	;;
	lspci)
		do_show lspci
	;;
	lsusb)
		do_show lsusb
	;;
	info)
		do_show info
	;;
	top)
		do_show top
	;;
  	*)
		echo "Unknown command '$CMD'"
	;;
    esac

done


	



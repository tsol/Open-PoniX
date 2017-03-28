echo "Script must be run as root"
sleep 1
#if [ -e squashfs-root ] ; then
#	rm -Rf squashfs-root
#fi
../../utils/tools/unsquashfs.def initrd.devices
rm initrd.devices
../../utils/tools/mksquashfs squashfs-root/ initrd.devices -b 1Mbyte -all-root -comp xz
chmod 644 initrd.devices
rm -Rf squashfs-root

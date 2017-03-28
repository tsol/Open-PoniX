# ! /bin/sh
clear
echo
echo " Setting up rotation option for ELO Touchscreen"
echo " =============================================="
echo 
echo
echo " 1. This script will run elova [calibration utility] in normal base mode of operation." 
echo "    Please ensure that an accurate calibration is performed in the normal video mode  "
echo "    without any video rotation."
echo
echo " 2. It will save the base touchscreen calibration for normal mode [0 Degree rotation] "
echo "    to configuration file in /etc/opt/elo-ser/rotate/ folder."
echo
echo
echo
echo
echo " Press [ ENTER ] to continue or [ CTRL+C ] to quit the script now."
echo
echo

read

/etc/opt/elo-ser/elova

cp /etc/opt/elo-ser/SerialConfigData /etc/opt/elo-ser/rotate/SerialConfigData_rotate_normal

echo
echo
echo " Copied touchscreen configuration file to /etc/opt/elo-ser/rotate/SerialConfigData_rotate_normal."
echo
echo " Please run /etc/opt/elo-ser/rotate/elorotate to adapt touchscreen calibration to current "
echo " video rotation mode."
echo
echo
echo


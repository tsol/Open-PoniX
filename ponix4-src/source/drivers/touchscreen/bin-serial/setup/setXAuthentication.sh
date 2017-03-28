#! /bin/sh

echo "$0: Starting Elo Serial X authentication script" > /var/log/ser-auth.log

if [ x"$ELOSERIAL_X_STARTUP" != x ]; then
  echo "$0: System startup detected ["$ELOSERIAL_X_STARTUP"]" >> /var/log/ser-auth.log
  echo "$0: Wait 10 seconds before proceeding, to allow X to load completely." >> /var/log/ser-auth.log
  sleep 10
fi

xauth_command=
if [ -x /usr/X11R6/bin/xauth ]; then
xauth_command=/usr/X11R6/bin/xauth
elif [ -x /usr/bin/xauth ]; then
xauth_command=/usr/bin/xauth
else
echo "$0: Can't find the xauth command. Aborting authentication script." >> /var/log/ser.log
echo "$0: Can't find the xauth command. Aborting authentication script." >> /var/log/ser-auth.log
exit 1
fi

#xauth_command=`which xauth` // Try using this to get xauth if not found in the above locations 
#sudo_command=`which sudo`   // Use the sudo command if necessary

current_display=:0
magic_cookie=$1

echo "$0: Current XAUTHORITY env path is:" $XAUTHORITY >> /var/log/ser-auth.log

if [ x"$XAUTHORITY" = x ]; then
 XAUTHORITY=$HOME/.Xauthority
 export XAUTHORITY
 echo "$0: Modified XAUTHORITY env path is:" $XAUTHORITY >> /var/log/ser-auth.log
fi

user_xauth_full_file=$XAUTHORITY

echo "$0: Current Display (Default):" $current_display >> /var/log/ser-auth.log
echo "$0: Xauth command location :" $xauth_command >> /var/log/ser-auth.log

echo "$0: User XAUTHORIZATION File with full path:" $user_xauth_full_file >> /var/log/ser-auth.log

echo "$0: Current Xwindow session's magic cookie:" $magic_cookie >> /var/log/ser-auth.log

if [ -w "$user_xauth_full_file" ]; then 
  echo "$0: User xauth file exists and is writable" >> /var/log/ser-auth.log
else
  echo "$0: User xauth file does not exist. Creating new file." >> /var/log/ser-auth.log
  echo -n >$user_xauth_full_file
fi
 
echo "$0: Updating xauth file:" >> /var/log/ser-auth.log
$xauth_command -f $user_xauth_full_file add $current_display . $magic_cookie

echo "$0: Xauth File Info:" >> /var/log/ser-auth.log
$xauth_command -f $user_xauth_full_file info >> /var/log/ser-auth.log
echo "$0: Xauth File Listing:" >> /var/log/ser-auth.log
$xauth_command -f $user_xauth_full_file list >> /var/log/ser-auth.log

echo "$0: Completed Elo Serial X authentication script" >> /var/log/ser-auth.log

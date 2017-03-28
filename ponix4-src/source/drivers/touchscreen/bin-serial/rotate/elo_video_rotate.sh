# ! /bin/sh
# 
# Sample script to perform both video rotation and touchscreen rotation together
#
# Script usage: /etc/opt/elo-ser/rotate/elo_video_rotate.sh <rotation mode>
#
# Available rotation modes: normal, left, inverted and right. 
#
# example: /etc/opt/elo-ser/rotate/elo_video_rotate.sh left
#
# Important: Video card driver may or may not support all the above modes.  
#                 

# Print the command line option provided for xrandr
echo The rotation mode provided for xrandr is:
echo $1

# Adjust the command line option to suit the elorotate requirement 
clo="--"$1

# Print the command line option provided for elorotate
echo The rotation mode provided for elorotate is:
echo $clo


# Call the xrandr extension for the respective video output and rotation mode 
# Edit the output name (default) to suit your system. Use "xrandr -q" to view all output names.
xrandr --output default --rotate $1


# Call the elorotate utility with the respective rotation mode 
/etc/opt/elo-ser/rotate/elorotate $clo 

#
# Regular cron jobs for the lunchbox package
#
0 4	* * *	root	[ -x /usr/bin/lunchbox_maintenance ] && /usr/bin/lunchbox_maintenance

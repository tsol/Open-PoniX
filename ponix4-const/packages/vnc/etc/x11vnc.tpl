service x11vnc
{	
	type		= UNLISTED
	disable 	= no
	flags		= REUSE
	socket_type	= stream        
	wait		= no
	user		= root
	port		= 5900
	server		= /bin/x11vnc
	server_args	= -rfbauth /etc/vnc_password -q -shared -inetd
	log_on_failure	+= USERID
	only_from	= localhost $VNC_REMOTE_ACCESS_FROM
}

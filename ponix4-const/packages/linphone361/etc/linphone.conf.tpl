[sip]
sip_port=$LINPHONE_PORT
default_proxy=0

[proxy_0]
reg_proxy=sip:$LINPHONE_SERVER
reg_identity=sip:$LINPHONE_LOGIN@$LINPHONE_SERVER
reg_sendregister=1

[auth_info_0]
username=$LINPHONE_LOGIN
passwd=$LINPHONE_PASSWORD
realm="$LINPHONE_REALM"

[sound]
local_ring=/share/linphone/sounds/oldphone.wav
remote_ring=/share/linphone/sounds/ringback.wav


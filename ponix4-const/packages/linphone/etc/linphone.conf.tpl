[sound]
playback_dev_id=$LINPHONE_PLAYBACK_DEV
ringer_dev_id=$LINPHONE_RINGER_DEV
capture_dev_id=$LINPHONE_CAPTURE_DEV
local_ring=/share/linphone/sounds/oldphone.wav
remote_ring=/share/linphone/sounds/ringback.wav
echocancellation=$LINPHONE_ECHOCANCEL

[video]
size=cif
enabled=0
display=0
capture=0
show_local=0
self_view=0

[GtkUi]
advanced_ui=1

[net]
download_bw=0
upload_bw=0
firewall_policy=0
mtu=0

[rtp]
audio_rtp_port=7078
video_rtp_port=9078
audio_jitt_comp=60
video_jitt_comp=60
nortp_timeout=30

[audio_codec_0]
mime=speex
rate=32000
enabled=1

[audio_codec_1]
mime=speex
rate=16000
enabled=1

[audio_codec_2]
mime=speex
rate=8000
enabled=1

[audio_codec_3]
mime=PCMU
rate=8000
enabled=1

[audio_codec_4]
mime=PCMA
rate=8000
enabled=1

[sip]
sip_port=$LINPHONE_PORT
sip_tcp_port=0
guess_hostname=1
contact=<sip:$LINPHONE_LOGIN@$LINPHONE_SERVER>
inc_timeout=15
use_info=0
use_rfc2833=1
use_ipv6=0
register_only_when_network_is_up=1
default_proxy=-1

[proxy_0]
reg_proxy=sip:$LINPHONE_SERVER
reg_identity=sip:$LINPHONE_LOGIN@$LINPHONE_SERVER
reg_sendregister=1

[auth_info_0]
username=$LINPHONE_LOGIN
passwd=$LINPHONE_PASSWORD
realm="$LINPHONE_REALM"



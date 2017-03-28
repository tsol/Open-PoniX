#! /bin/sh

REPLIMENU="/etc/replimenu"

if [ ! -e "$REPLIMENU/snd.menu" ] && [ ! -e "$REPLIMENU/config.menu" ]; then
   . /etc/ponix.global
   echo s/\$TS_VERSION/$TS_VERSION/g > /tmp/configmenu
   echo s/\$CLIENT_IP/$CLIENT_IP/g >> /tmp/configmenu
   echo s/\$CLIENT_NAME/$CLIENT_NAME/g >> /tmp/configmenu
   echo s/\$CLIENT_MAC/$CLIENT_MAC/g >> /tmp/configmenu
   echo s/\$SERVER_IP/$SERVER_IP/g >> /tmp/configmenu
                                     
   cat $REPLIMENU/config.tpl | sed -f /tmp/configmenu  > $REPLIMENU/config.menu
   cat $REPLIMENU/snd.tpl | sed -f /tmp/configmenu  > $REPLIMENU/snd.menu
   rm /tmp/configmenu
fi

replimenu -c 4 -f /etc/replimenu/config.menu
                                                
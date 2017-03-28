caption = Terminal Solutions Saint-Petersburg 2011 http://www.t-sol.ru +7 (812) 458-7499
colorscheme = 4

br -----------SOUND------------
br

begin item
   name = sndcfg
   label = Sound configuration
   bullet =
   command = /bin/tsol_soundcfg.sh
end

br
br In alsamixer use arrow-keys and
br TAB key to setup your sound card
br
br ---------LOG FILES----------
br

begin item
   name boot.log
   label = Boot log
   bullet =
   command = vi /var/log/boot.log
end  

begin item
   name Xorg1
   label = Xorg log
   bullet =
   command = vi /var/log/Xorg.1.log
end

begin item
   name processes
   label = Running processes
   bullet =
   command = /etc/scripts/ps.sh
end

br
br For exit please enter ":q"
br
br ----------------------------

begin item
   name console
   label Developer console
   bullet =
   command = chvt 12
end

br ----------------------------
br
begin item
    name = back
    label = Go to first open session
    bullet =
    command = /etc/scripts/opensession.sh
end
begin item
    name = QUIT
    label = Shutdown
    bullet =
    command = /bin/shutdown
end
begin item
    name = QUIT
    label = Reboot
    bullet
    command = /bin/reboot -f
end
br
br
br poniX version $TS_VERSION
br ----------------------------
br IP address: $CLIENT_IP
br MAC address: $CLIENT_MAC
br Hostname: $CLIENT_NAME 
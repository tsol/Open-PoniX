caption = Terminal Solutions Saint-Petersburg 2011 http://www.t-sol.ru +7(812)458-7499
colorscheme = 4
br
br TFTP Server: $SERVER_IP
br
br Upload sound settings to server?
br
br REMEMBER: Tftp server must allow uploads of new files
br (-w param for standart tftpd). Folder ASOUND must
br exist on the server and have global write permissions set.
br
br
begin item
    name = sound
    label = For this terminal only
    bullet =
    type = radiobutton | selected
    default = sndcfg_mac
end

begin item
   name = sound
   label = For all terminals
   bullet =
   type = radiobutton
   default = sndcfg_all
end

br
br -----------------------------
br
begin item
   name = QUIT
   label = Apply settings
   bullet =
   command = /bin/tsol_soundcfg.sh
end

begin item
   name = QUIT
   label = Quit
   bullet =
end

########## Start section from template ##################
Section "ServerFlags"
    Option      "blank time"    "$SCREEN_BLANK_TIME"
    Option      "standby time"  "$SCREEN_BLANK_TIME"
    Option      "suspend time"  "$SCREEN_BLANK_TIME"
    Option      "off time"      "$SCREEN_BLANK_TIME"
    Option      "AllowEmptyInput" "Off"
    Option	"AutoAddDevices" "False"
EndSection
Section "InputDevice"
        Option   "AllowEmptyInput" "FALSE"
        Identifier  "Keyboard0"
        Driver      "kbd"
        Option      "AutoRepeat" "400 30"
        Option  "XkbModel"        "pc105"
        Option  "XkbLayout"       "us,ru(winkeys)"
        Option  "XkbOptions"      "grp:alt_shift_toggle"

EndSection
Section "InputDevice"                                      
        Identifier  "Mouse0"
        Driver      "mouse"                                
        Option      "Protocol" "auto"
        Option      "Device" "/dev/input/mice"
        Option      "ZAxisMapping" "4 5 6 7"
	Option      "Sensitivity" "$MOUSE_SPEED"
EndSection
########## End section from template ####################

##### Section autoconfigured at "Xorg -configure" #######

#!/bin/bash
# This script is triggered by the "power" configuration file.  This is
# described in the acpid man page.

# A much more comprehensive power button handler is provided in the
# powerbtn directory in this samples directory.  If you run gnome or KDE,
# it's better to use that one instead of this one.

/sbin/shutdown -h now "Power button pressed"


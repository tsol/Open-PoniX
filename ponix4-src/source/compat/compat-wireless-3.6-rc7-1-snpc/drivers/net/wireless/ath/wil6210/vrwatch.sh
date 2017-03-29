#!/bin/bash
# Watch VRING's
### where is wil6210 debugfs?
D=$(find /sys/kernel/debug/ieee80211/ -name wil6210)
exec watch -n 0.2 -d cat $D/vrings


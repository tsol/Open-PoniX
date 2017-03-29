#!/bin/bash
# Display Tx descriptor content
# Descriptor index
I=$1
### where is wil6210 debugfs?
D=$(find /sys/kernel/debug/ieee80211/ -name wil6210)
echo $I > $D/txdesc_index
cat $D/txdesc

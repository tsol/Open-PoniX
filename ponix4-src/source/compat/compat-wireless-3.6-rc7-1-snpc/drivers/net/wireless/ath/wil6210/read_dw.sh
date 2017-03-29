#!/bin/bash
### parameter - FW address to read
A=$1
### where is wil6210 debugfs?
D=$(find /sys/kernel/debug/ieee80211/ -name wil6210)
echo $A > $D/mem_addr
cat $D/mem_val

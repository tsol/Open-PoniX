#!/bin/bash
### where is wil6210 debugfs?
D=$(find /sys/kernel/debug/ieee80211/ -name wil6210)
exec tools/trace $D/blob_fw_peri

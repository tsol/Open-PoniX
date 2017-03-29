#!/bin/bash
### parameter - memdump prefix
P=$1
### where is wil6210 debugfs?
D=$(find /sys/kernel/debug/ieee80211/ -name wil6210)

for f in fw_data fw_peri uc_data; do {
  cat $D/blob_${f} > ${P}${f}
} done

#!/bin/sh

dd conv=notrunc bs=440 count=1 if=mbr.bin of=/dev/sda1
parted /dev/sda1 set 1 boot on

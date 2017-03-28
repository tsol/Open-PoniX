#!/bin/sh

# Copyright (C) 1999-2004 Hallvar Helleseth (hallvar@ii.uib.no)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies of the Software, its documentation and marketing & publicity
# materials, and acknowledgment shall be given in the documentation, materials
# and software packages that this Software was used.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Author: Hallvar Helleseth (hallvar@ii.uib.no) - Not that I'm proud ;)
# Instructions: move the top window and see what happends to the lower one.
#

# Open dialog 1
eesh -e "dialog_ok Move this window"
usleep 100000
eesh -e "wop Message title Dlg1"
usleep 100000

# Open dialog 2
eesh -e "dialog_ok Watch me follow the above window"
usleep 100000
eesh -e "wop Message title Dlg2"
usleep 100000

# In one endless loop, get window position of the first window, then move the
# second one accordingly...
while true
do
	# Get position
	pos=`eesh wop Dlg1 move "?"`
	a=0
	for i in $pos;do
		a=$(($a + 1))
		if [ $a = 3 ];then
			xpos=$i
		fi
		if [ $a = 4 ];then
			ypos=$i
		fi
	done
		
	# Move the second window to the new position
	eesh wop Dlg2 move $xpos $(($ypos + 74))
done

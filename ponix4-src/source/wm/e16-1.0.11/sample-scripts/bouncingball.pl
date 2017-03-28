#!/usr/bin/perl

# Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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

# This is a little script that will create a window that says
# "Follow the Bouncing Ball" and then drops it to the bottom of the
# screen and slowly bounces it around.
# then when it's done bouncing it gets rid of it.


$s = `eesh screen size`;
chomp($s);

($crap,$width,$height) = split(/\s+/,$s);
if ($s =~ /.*size\s+(\d+)x(\d+)/) {
  $width = $1; $height = $2;
}

`eesh dialog_ok \"Follow the Bouncing Ball\"`;
$ball = "Message";

$s = `eesh win_op $ball move "?"`;
if ($s =~ /.*:\s+(\d+)\s+(\d+)/) {
  $ballx = $1; $bally = $2;
}
$s = `eesh win_op $ball size "??"`;
if ($s =~ /.*:\s+(\d+)\s+(\d+)/) {
  $ballw = $1; $ballh = $2;
}
#print "x,y=$ballx,$bally wxh=$ballw x $ballh\n";

# now for the fun part.  make that baby bounce up and down.
# we're going to open a big pipe for this one and just shove data
# to it.

open IPCPIPE,"| eesh";

@fallspeed = (30,25,20,15,10,5,4,3,2);
$i = 0;
foreach(@fallspeed) {
	$originalbally = $bally;
	$fallspeed = $fallspeed[i];
	while($bally < ($height - $ballh)) {
		if(($bally + $fallspeed + $ballh) < $height) {
			$bally += $fallspeed;
		} else {
			$bally = $height - $ballh;
		}
		print IPCPIPE "win_op $ball move $ballx $bally\n";
		system("usleep 20000");
	}

	if($fallspeed[i+1]) {
		$fallspeed = $fallspeed[i+1];
	} else {
		$fallspeed = 1;
	}

	while($bally > ($originalbally + int($originalbally * (1/$#fallspeed)))) {
		if(($bally - $fallspeed) > 
				($originalbally + int($originalbally * (1/$#fallspeed)))) {
			$bally -= $fallspeed;
		} else {
			$bally = $originalbally + int($originalbally * (1/$#fallspeed));
		}
		print IPCPIPE "win_op $ball move $ballx $bally\n";
		system("usleep 20000");
	}
	$i++;
}

print IPCPIPE "win_op $ball close\n";
close IPCPIPE;

# that's all folks.

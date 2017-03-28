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

# This script is still VERY VERY VERY early on in development
# but I wanted to give you folks an idea of why the IPC was being
# set up the way it was.  this is just a quick hack.  expect better
# samples in the near future.
# This app will take the parameters "on" and "off", and will basically
# shade all the windows on the current desktop and area.  (or unshade)
#
# --Mandrake

# here we're going to test to see whether we are shading or unshading
# the window.

if($ARGV[0] eq "on") {
	$shade = 1;
} else {
	$shade = 0;
}

# here we'll retreive the current desk we're on

$_ = `eesh desk`;
@stuff = split(/\s*:\s*/);
@stuff = split(/\s*\/\s*/, $stuff[1]);
$current_desk = $stuff[0];

# here we'll retreive the current area we're on

$_ = `eesh area`;
@stuff = split(/\s*\n\s*/);
@stuff = split(/\s*:\s*/, $stuff[0]);
$current_area = $stuff[1];
$current_area =~ s/\n//g;


# get the old shadespeed so that we can set it back later
# because we want this to happen fairly quickly, we'll set
# the speed to something really high

$_ = `eesh show misc.shading.speed`;
@stuff = split(/\s*\n\s*/);
@stuff = split(/\s*=\s*/, $stuff[0]);
$shadespeed = $stuff[1];

open IPCPIPE,"| eesh";
print IPCPIPE "set misc.shading.speed 10000000\n";

# now we're going to walk through each of these windows and
# shade them

@winlist = `eesh window_list a`;
foreach (@winlist) {
	if (/\s*(\w+)\s* : .* :: \s*(-*\d+)\s* : (.*) : (.*)$/) {
		$window = $1;
		$desk = $2;
		$area = $3;
		$name = $4;

		# Skip pagers, iconboxes, systrays, and epplets
		next if ($name =~ /^Pager-|Iconbox|Systray|E-/);
#		next unless (($desk == -1) and ($desk eq $current_desk));
		next unless ($desk eq $current_desk);
		next unless ($area eq $current_area);
		if ($shade) {
			print IPCPIPE "win_op $window shade on\n";
		} else {
			print IPCPIPE "win_op $window shade off\n";
		}
	}
}

# now we're going to set the shade speed back to what it was originally

print IPCPIPE "set misc.shading.speed $shadespeed\n";
close IPCPIPE;

# that's it!

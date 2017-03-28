#!/usr/bin/perl
use strict;

#
# This script helps to transfer folders with files from /build/usr to /const/package/[package]
# preserving the directory tree and symlinks
#
# Files are copied based on skeleton folder. Usage scenario is as follows:
#
# 1. first you build some program using prefix '/tmp/program', it places all files there
# 2. then you build program using prefix='/usr/' where all files get mixed with 
# 3. then you use THIS TOOL to copy only neccessary files from /usr based on /tmp/program tree
#

#my $skeldir	= '/build/tmp/alsa-lib-1025';
my $skeldir	= '/build/tmp/alsa-utils-1025';
my $srcdir	= '/build/usr';
my $dstdir	= '/const/packages/alsa1025full';


my $find = qx { find $skeldir };

#print $find;

LINE: foreach my $line (split(/[\n\r]+/,$find))
{
  my ($relative) = $line =~ /^$skeldir(.+)/;
  my $dest = $dstdir . $relative;
  my $source = $srcdir . $relative;

  print "LINE: $line\n";
    
  if ( -d $line ) {

      if ( ! -d $source ) {
          print "[!] SOURCE FOLDER: '$source' does'nt exist!\n";
          exit;
      }

      if ( ! -d $dest) {  
        if ( ! mkdir ($dest) ) {
          print "[!] UNABLE TO CREATE DEST FOLDER: '$dest'!\n";
          exit;
        }
        else
        {
          print "DEST FOLDER CREATED: '$dest'\n";        
        }
      }
      else
      {
          print "DEST FOLDER EXISTED: '$dest'\n";      
      }
      
      next LINE;
  };

  if ( -f $line) {

      if ( ! -e $source) {
          print "[!] SOURCE FILE '$source' DOES NOT EXIST!\n";
          exit;
      }
            
      if ( -e $dest) {
          print "DEST FILE EXISTED: '$dest', removing...\n";
          if ( ! unlink($dest) ) {
              print "[!] EXISTED DEST FILE COULD NOT BE DELETED!\n";
              exit;
          }
      }

      qx{ cp -a $source $dest };
      
      if (! -e $dest) {
        print "[!] DEST FILE WAS NOT COPIED!\n";
        exit;      
      }
      else
      {
        print "FILE COPIED OK: '$dest'\n";
      }

  };

}

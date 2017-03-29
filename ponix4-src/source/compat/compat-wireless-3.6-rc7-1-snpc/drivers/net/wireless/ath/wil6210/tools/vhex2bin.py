#!/usr/bin/python

'''
Convert .vhex file into binary representation
'''

import sys
import fileinput
import re
import struct

for line in fileinput.input():
  if not re.match("[0-9a-f]+", line):
    continue
  x = int(line,16)
  s = struct.pack("<I", x)
  sys.stdout.write(s)
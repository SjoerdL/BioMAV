#!/usr/bin/python

import sys
import re

if len(sys.argv) < 2:
  sys.stderr.write("No filename specified\n")
  sys.exit(1)

OPERATOR_REGEX="self->operator(\\+|-|\\*|/|<|>=|==|\\[\\])\\s*\\(([^\\)]*)\\)"
I_OPERATOR = 0
I_PARAMETER = 1

try:
  rfile = open(sys.argv[1], "rb")
except IOError:
  sys.stderr.write("File " + sys.argv[1] + " cannot be opened\n")
  sys.exit(2)

try:
  for line in rfile:
    m = re.search(OPERATOR_REGEX, line)
    if m == None:
      sys.stdout.write(line)
    else:
      replacementString = "(*self)"
      if m.groups()[I_OPERATOR] == "[]":
        replacementString += "[(" + m.groups()[I_PARAMETER] + ")]"
      else:
        replacementString += " " + m.groups()[I_OPERATOR] + " (" + m.groups()[I_PARAMETER] + ")"
      replacementString = "(" + replacementString + ")"
      sys.stdout.write(line.replace(m.group(), replacementString))

finally:
  rfile.close()
  

#!/bin/bash

if [ -z $1 ]; then
  echo No header file specified
  exit 1
fi

HEADERFILE=$1

TMPFILE=/tmp/tmpheaderinsertionfile

FILES=$(find . -name '*.java')

for f in $FILES; do
  echo Adding to $f
  cat $HEADERFILE $f > $TMPFILE
  cp $TMPFILE $f  
done

exit 0

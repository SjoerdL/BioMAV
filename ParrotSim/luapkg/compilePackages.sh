#!/bin/bash

TMP_FILE=/tmp/cPkgTmp

files=`ls *.pkg`
tolua_executeable=tolua++5.1
which $tolua_executeable > /dev/null
if [ "$?" -ne "0" ]; then
  tolua_executeable=tolua++
  which $tolua_executeable > /dev/null

  if [ "$?" -ne "0" ]; then
    tolua_executeable=../win/libs/tolua++/bin/tolua++5.1
    which $tolua_executeable > /dev/null

    if [ "$?" -ne "0" ]; then
      tolua_executeable=../tk/libs/tolua++/bin/tolua++
      which $tolua_executeable > /dev/null

      if [ "$?" -ne "0" ]; then
        echo "tolua++ not found!"
        exit 1
      fi
    fi
  fi
fi

echo "tolua++ found: " $tolua_executeable

python_executable=python
which $python_executable > /dev/null
if [ "$?" -ne "0" ]; then
  python_executable=python2
  which $python_executable > /dev/null
  if [ "$?" -ne "0" ]; then
    echo "Python (Version 2.x) not found"
    exit 1
  fi
fi

python_version=$($python_executable --version 2>&1 | sed -r 's/Python ([0-9]).*/\1/')
if [ "$python_version" -ne "2" ]; then
  # Fix for ARCH(?)
  python_executable=python2
  #                  ^ ^ ^
  #       Set to correct executeable

  which $python_executable > /dev/null
  if [ "$?" -ne "0" ]; then
    echo "Python (Version 2.x) not found"
    exit 1
  fi

  python_version=$($python_executable --version 2>&1 | sed -r 's/Python ([0-9]).*/\1/')
  if [ "$python_version" -ne "2" ]; then
    echo "Python has wrong version - needs to be 2.x"
    exit 1
  fi
fi

echo "Python 2.x found: " $python_executable

for file in $files; do
  echo "Compiling package $file"
  basefn=`echo $file |  cut -d'.' -f1`
  BASEFN=`echo $basefn | tr '[:lower:]' '[:upper:]'`

  $tolua_executeable -o ${TMP_FILE}2 -H ${TMP_FILE} $file
  $python_executable convertOperators.py ${TMP_FILE}2 > $basefn".cpp"

  headerfile=${basefn}.hpp

  rm $headerfile 2> /dev/null
  echo "#ifndef _LUABINDING_"${BASEFN}"_H_" > $headerfile
  echo "#define _LUABINDING_"${BASEFN}"_H_" >> $headerfile
  echo '#include <tolua++.h>' >> $headerfile
  cat $TMP_FILE >> $headerfile 2> /dev/null
  echo "#endif // _LUABINDING_"${BASEFN}"_H_" >> $headerfile

  rm $TMP_FILE 2> /dev/null
  rm ${TMP_FILE}2 2> /dev/null
  rm ${TMP_FILE}3 2> /dev/null
done

exit 0

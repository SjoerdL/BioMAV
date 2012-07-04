#!/bin/bash
# clean previous compilation
make clean

function printOptions {
  echo "1. OpenCV 2.1"
  echo "2. OpenCV 2.2"
  echo "3. JNI"

  echo -n "Please choose input support [1,2 or 3]? "
}

 
# Declare variable choice and assign value 4
choice=6
# Print to stdout
printOptions

# Loop while the variable choice is equal 6
# bash while loop
while [ $choice -eq 6 ]; do
  read choice
	if [ $choice -eq 1 ] ; then
		# give confirmation
		echo "OpenCV 2.1"
		# copy makefile
		cp Makefiles/openCV21 Makefile
		# rearrange source files
		#cp src/type/openCV/* src/
		# compile code
		make
		# run programme (convert webcam into correct data stream)
		#LD_PRELOAD=/usr/lib/libv4l/v4l2convert.so ./vision_testing
		run/runOpenCV.sh
	else
	if [ $choice -eq 2 ] ; then
		# give confirmation
		echo "OpenCV 2.2"
		# copy makefile
		cp Makefiles/openCV22 Makefile
		# rearrange source files
		# compile code		
		
		make
	else
	if [ $choice -eq 3 ] ; then
		# give confirmation
		echo "JNI"
		
		# copy makefile
		cp Makefiles/jni Makefile

		# compile code		
		make
	else
		# illegal character input, repeat choices
		printOptions
		choice=6
	fi
	fi
	fi
done 

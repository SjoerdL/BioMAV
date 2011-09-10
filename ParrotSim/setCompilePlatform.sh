#!/bin/bash

srcMakefile=

if [ "$1" == "win" ]; then
	echo "Setting windows (MSys + MinGW) makefiles"
	srcMakefile=Makefile.win
fi

if [ "$1" == "linux" ]; then
	echo "Setting linux makefiles"
	srcMakefile=Makefile.lin
fi

if [ "$1" == "arch" ]; then
	echo "Setting arch-linux makefiles (don't forget to change compilePackages.sh (tolua++, python2))"
	srcMakefile=Makefile.arc
fi

if [ "$1" == "tk" ]; then
	echo "Setting TK-linux makefiles"
	srcMakefile=Makefile.tklin
fi

if [ "$srcMakefile" == "" ]; then
	echo "Please specify a target platform (win, linux or arch). Use tk for TK-linux Makefiles."
	exit 1
else
	for makefile in `find . -maxdepth 2 -name $srcMakefile`; do
		targetMakefile=$(echo $makefile | sed -E 's/\..{1,5}$//';)
		echo cp $makefile $targetMakefile
		cp $makefile $targetMakefile
	done
fi

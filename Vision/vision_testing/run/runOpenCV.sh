#!/bin/bash

## -- INPUT -- ##
# dimensions of the input
width=320
height=240
# index of the webcam (default=0)
webcamIndex=0

INPUT="$width $height $webcamIndex"

## -- PREPROCESSING -- ##
## conversion ##
# pixels/hexels (bool)
hexel=1
# diameter of the hexels
hexelDiameter=3.75
## activation ##
# colorspace/photoreceptor (bool)
colorspace=1
# colorspace rewrite type
# 0=RED, 1=GREEN, 2=BLUE, 3=RED/GREEN 4=GREEN/BLUE, 5=BLUE/RED, 6=RED/GREEN/BLUE
###		(TEMP: ONLY USE ONE CHANNEL AND 0/1 AS INPUT)
convert=1
# ratio long/medium/short 
###		(TEMP: ONLY USE ONE CHANNEL AND 0/1 AS INPUT)
long=1
medium=0
short=0
## general ##
# normalisation (bool)
normalise=1
# lateral inhibition (bool)
lateral=1


PREPROCESSING="$hexel $hexelDiameter $colorspace $convert $long $medium $short $normalise $lateral"

## -- PROCESSING -- ##



## -- OUTPUT -- ##

LD_PRELOAD=/usr/lib/libv4l/v4l2convert.so ./vision_testing $INPUT $PREPROCESSING

# v4l2ucp


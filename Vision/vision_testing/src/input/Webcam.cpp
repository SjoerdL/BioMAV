/*
    This file is part of the BioMAV project.

    The BioMAV project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The BioMAV project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The BioMAV project. If not, see <http://www.gnu.org/licenses/>.
*/
/* 
 * File:   Webcam.cpp
 * Author: jelangevoort
 * 
 * Created on August 14, 2011, 11:15 AM
 */

#include "Webcam.h"

#include <cstdio>

/**
 * Constructor
 *  initializes capture device and set custom resolution
 */
Webcam::Webcam(int captureDevice, int width, int height)
{
    // open webcam
    capture = new cv::VideoCapture (captureDevice);

    // set resolution webcam
    capture->set(CV_CAP_PROP_FRAME_WIDTH,width);
    capture->set(CV_CAP_PROP_FRAME_HEIGHT,height);

    if (!capture->isOpened())
    {
        printf("Failed to open video capture device %d\n", captureDevice);
    }
    
}

/**
 * Destructor.
 */
Webcam::~Webcam()
{
    capture->release();
    delete capture;
}

/**
 * Grabs frame and returns the image data
 *
 * uint8_t* RGB [width*height*3]
 */
uint8_t* Webcam::getData()
{
    *capture >> frame;
    cvtColor(frame,frame,CV_BGR2RGB);
    return frame.data;
}

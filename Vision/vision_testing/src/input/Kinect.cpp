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
 * File:   Kinect.cpp
 * Author: jelangevoort
 * 
 * Created on August 14, 2011, 11:29 AM
 */

#include "Kinect.h"

/**
 * Constructor
 *  Starts kinect device with standard parameters
 */
Kinect::Kinect()
{
    // Get Kinect Device
    device = &freenect.createDevice<MyFreenectDevice>(0);
    // Start Kinect Device
    device->setTiltDegrees(10);
    device->startVideo();
    //device->startDepth();
    //handle Kinect Device Data
    device->setLed(LED_RED);

    rgb = new uint8_t [640*480*3];
}

/**
 * Destructor
 */
Kinect::~Kinect()
{
    // Stop Kinect Device
    device->stopVideo();
    //device->stopDepth();
    device->setLed(LED_OFF);
}

/**
 * Gets RGB data from kinect device and returns it
 *
 * @return image (RGB) data
 */
uint8_t* Kinect::getData()
{
    device->getRGB(rgb);
    return rgb;
}

/**
 * Gets depth data from kinect device and returns it
 *
 * @return depth data
 */
uint8_t* Kinect::getDepth()
{
    //device->getDepth();
    return 0;
}
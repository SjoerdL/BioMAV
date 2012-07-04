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
 * File:   Input.cpp
 * Author: jlangevoort
 * 
 * Created on August 12, 2011, 8:48 AM
 */

#include "Input.h"

/**
 * Constructor
 *  initializes input device and parameters
 */
Input::Input(int width, int height, int index)
{
    // set input parameters to standard
    this->width = width;
    this->height = height;
    this->numberOfChannels = 3;	// standard RGB colorspace
    
    // initialize data stream
    webcam = new Webcam(index, width, height);
}

/**
 * Destructor
 */
Input::~Input()
{
    delete[] data;
    delete[] webcam;
}

/**
 * Gets the input data and converts it into useable activation
 * 
 * @return activation based on the input and the chosen parameters
 */
uint8_t* Input::getInput()
{
    // get data stream
    data = webcam->getData();
    // return calculated activation
    return data;
}

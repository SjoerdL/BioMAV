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
 * File:   Input.h
 * Author: jlangevoort
 *
 * Created on August 12, 2011, 8:48 AM
 *
 * Opens the data stream
 *  pointer to an RGB image
 *  uint8_t* [width*height*nchannels]
 *  [0-255] ... red green blue red green blue ...
 *
 * Converts data stream into usable information
 *  square or hexagonal matrix
 *  float* [width*height]
 *   qvadriangvlvs: default width & height
 *   sexangvlvs: alternative width & height based on conversion
 *  [0-1] ... activation activation activation ...
 */

#ifndef INPUT_H
#define	INPUT_H

#include <stdint.h>             // exact-width integer types

// included for testing purposes
#include "input/Webcam.h"             // webcam device

class Input 
{
public:
    Input(int width, int height, int index);            // constructor
    ~Input();   // destructor
    // get data
    uint8_t* getInput();
    // return data for usage as parameters for processing
    int getWidth(){return width;};
    int getHeight(){return height;};
    int getNumberOfChannels(){return numberOfChannels;};
private:
    // input parameters
    int width;
    int height;
    int numberOfChannels;
    // classes used in testing
    Webcam* webcam;
    // datastructures
    uint8_t* data;     // input image
};

#endif	/* INPUT_H */

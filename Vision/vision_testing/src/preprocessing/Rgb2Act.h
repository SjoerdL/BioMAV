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
 * File:   Rgb2Act.h
 * Author: jlangevoort
 *
 * Created on August 12, 2011, 11:00 AM
 *
 * Converts an RGB image to activation
 *
 * Input
 *  RGB image
 *  uint8_t* [width*height*nchannels]
 *  [0-255] ... red green blue red green blue ...
 *
 * Output
 *  activation
 *  float* [width*height]
 *  [0-1] ... activation activation activation ...
 * 
 * NYI: 
 *  alternate conversion based on color or luminance
 *  CMYK, YUV, YPbPr, HSV, HSL, ...
 */

#ifndef RGB2ACT_H
#define	RGB2ACT_H

#include <stdint.h>             // exact-width integer types

class Rgb2Act
{
public:
    Rgb2Act(int width, int height, int numberOfChannels);
    ~Rgb2Act();

    // get activation
    float* getActivation(uint8_t* input, int methodType);
private:
    // image properties
    int length;
    int numberOfChannels;
    // data structures
    float* output;
    
    // conversion functions
    float* getRed(uint8_t* input);
    float* getGreen(uint8_t* input);
    float* getBlue(uint8_t* input);
    float* getRedGreen(uint8_t* input);
    float* getGreenBlue(uint8_t* input);
    float* getBlueRed(uint8_t* input);
    float* getRedGreenBlue(uint8_t* input);
};

#endif	/* RGB2ACT_H */


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
 * File:   Rgb2Lms.h
 * Author: jlangevoort
 *
 * Created on August 12, 2011, 11:10 AM
 * 
 * Converts an RGB image to cone cell like activation
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
 *  alternate conversion based on insect rhabdom
 *  will be implemented if necessary data is ever found
 *  not a priority, since it is not necessarily task related (orange pole)
 */

#ifndef RGB2LMS_H
#define	RGB2LMS_H

#include <stdint.h>             // exact-width integer types
#include <omp.h>                // parallel processing OpenMP

class Rgb2Lms 
{
public:
    Rgb2Lms(int width, int height, int numberOfChannels);
    ~Rgb2Lms();

    // set cell type
    void setCellType(int l, int m, int s);
    // conversion functions
    float* getLms(uint8_t* input);
private:
    // image properties
    int width;
    int height;
    int numberOfChannels;
    int* types;
    // parameters
    float* gamma_fcn;
    float lr, lg, lb;
    float mr, mg, mb;
    float sr, sg, sb;
    // output
    float* activation;

    // functions used in the conversion
    float convertRgbToL (int r, int g, int b);
    float convertRgbToM (int r, int g, int b);
    float convertRgbToS (int r, int g, int b);
    float convertRgbToR (int r, int g, int b);
};


#endif	/* RGB2LMS_H */

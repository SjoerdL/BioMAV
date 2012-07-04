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
 * File:   Qvadri2Sex.h
 * Author: jlangevoort
 *
 * Created on August 12, 2011, 9:28 AM
 *
 * Converts a square to a hexagonal matrix
 *
 * Input
 *  RGB image
 *  uint8_t* [width*height*nchannels]
 *  [0-255] ... red green blue red green blue ...
 *
 * Output
 *  RGB image, hexagonal representation
 *  uint8_t* [width*height*nchannels]
 *  [0-255] ... red green blue red green blue ...
 */

#ifndef QVADRI2SEX_H
#define	QUADRI2SEX_H

#include <math.h>               // generic mathematical functions
#include <stdint.h>             // exact-width integer types
#include <omp.h>                // parallel processing

class Qvadri2Sex
{
public:
    Qvadri2Sex(int width, int height, int numberOfChannels, float hexelDiameter);
    ~Qvadri2Sex();

    // conversion
    uint8_t* convert(uint8_t* qvadri);

    // public return functions
    int getWidth(){return width;}
    int getHeight(){return height;}
    int getLength(){return length;}
private:
    // input properties
    int inputNumberOfChannels;
    int inputLength;
    // output properties
    int width;
    int height;
    int length;
    float area;
    float t;
    // data structures
    int* number;
    int* sum;
    int* connection;
    float* weight;
    uint8_t* output;

    // functions used in the conversion process
    float calcPolygonArea(float* x, float* y, int length);
    float calcIntersectionX(int x, float x1, float y1, float x2, float y2);
    float calcIntersectionY(int y, float x1, float y1, float x2, float y2);
    float calcOverlap(int x, int y, float* x_hex, float *y_hex);
};

#endif	/* QVADRI2SEX_H */

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
 * File:   Preprocessing.h
 * Author: jlangevoort
 *
 * Created on September 08, 2011, 8:48 AM
 *
 * Preprocesses the 'raw' data (i.e. RGB arrays)
 *
 * Converts data stream into usable information
 *  square or hexagonal matrix
 *  float* [width*height]
 *   qvadriangvlvs: default width & height
 *   sexangvlvs: alternative width & height based on conversion
 *  [0-1] ... activation activation activation ...
 */

#ifndef PREPROCESSING_H
#define	PREPROCESSING_H

#include <stdint.h>             			// exact-width integer types

#include "preprocessing/Qvadri2Sex.h"       // conversion from pixel to hexel
#include "preprocessing/Rgb2Act.h"          // conversion from RGB to activation
#include "preprocessing/Rgb2Lms.h"          // conversion from RGB to LMS
#include "miscellaneous/LatInh.h"          	// lateral inhibition


class Preprocessing 
{
public:
    Preprocessing(int width, int height, int hexel, float hexelDiameter, int colorspace, int convert, int l, int m, int s, int normalize, int lateral);            // constructor
    ~Preprocessing();   // destructor
    // get data
    float* process(uint8_t* input);
    // return data for usage as parameters for processing
    int getWidth(){return outputWidth;};
    int getHeight(){return outputHeight;};
    bool estSexangvlatvm(){if(outputType==1) return true; return false;};
    // COLORTESTING
    float* getActivation(){return activation;};
    float* getActivation2(){return activation2;};
private:
    // choice parameters
    int inputType;
    int conversionType;
    int methodType;
    int outputType;
    bool isLatInh;
    // input parameters
    int inputWidth;
    int inputHeight;
    int inputNumberOfChannels;
    // output parameters
    int outputWidth;
    int outputHeight;
    int outputLength;
    // classes used in conversion
    Qvadri2Sex* qvadri2sex;
    Rgb2Act* rgb2act;
    Rgb2Lms* rgb2lms;
    LatInh* latInh;
    // TEST color decoding
    Rgb2Act* rgb2act2;
    Rgb2Lms* rgb2lms2;
	LatInh* latInh2;
    // datastructures
    float* activation;  // output activation
    float* activation2;  // COLOR TEST
};

#endif	/* PREPROCESSING_H */

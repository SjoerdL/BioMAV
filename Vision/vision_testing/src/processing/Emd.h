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
 * File:   Emd.h
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 6:24 PM
 *
 *
 * Input
 *  activation
 *  float* [width*height]
 *  [0-1] ... activation activation activation ...
 *
 * Output
 *  activation
 *  float* [width*height]
 *  [0-1] ... activation activation activation ...
 *
 * 
 * Based on original code by Guido de Croon (2010)
 *
 * function elementaryMotionDetection(im_dir, im_type, scales, tH, tL)
 * Uses the enhanced Elementary Motion Detector (EMD) from the article:
 * "An FPGA Implementation of Insect-Inspired Motion Detector for High-Speed Vision Systems"
 *  by Zhang et al., ICRA 2008.
 * We apply the EMD at several scales.
 * The goal is to make an instructive implementation, not a computationally
 *  efficient one.
 * ...
 *
 *  (c) 2010, Guido de Croon.
 * 
 * TODO: qvadriangvlar / sexangvlar
 * TODO: width/height/length, rewrite matrix to array
 */

#ifndef EMD_H
#define	EMD_H

#include<math.h>                // basic math include
#include<omp.h>                 // openMP for parallel processing

class Emd
{
public:
    Emd(int width, int height, float tHigh, float tLow, bool estSexangvlatvm);
    ~Emd();

    // 
    void process(float* activation);

    // activation for each direction
    float* getMotionNegX(){return motionNegX;};
    float* getMotionNegY(){return motionNegY;};
    float* getMotionNegZ(){return motionNegZ;};
    float* getMotionX(){return motionX;};
    float* getMotionY(){return motionY;};
    float* getMotionZ(){return motionZ;};
    // for testing purposes
    float* getHighPass(){return highPass;};
    float* getPassed(){return passed;};
    float* getLowPass(){return lowPass;};
    // global activation
    float* getMotionOrientation(){return motionOrientation;};
    float* getMotionStrength(){return motionStrength;};
    
private:
    // EMD parameters
    int width;
    int height;
    int length;
    float tHigh;
    float tLow;
    bool estSexangvlatvm;
    // data structures
    //  pre-processing
    float* highPass;
    float* passed;
    float* lowPass;
    //  output cells (specific directions)
    float* motionX;
    float* motionY;
    float* motionZ;
    float* motionNegX;
    float* motionNegY;
    float* motionNegZ;
    //  combination
    float* motionOrientation;
    float* motionStrength;
    
    // TEST //
	float* highPassOff;
    float* passedOff;
    float* lowPassOff;
    
    bool isEven(int i){if ((i%2) == 1)return false; else return true;};
};

#endif	/* EMD_H */

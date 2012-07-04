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
 * File:   Recognition.h
 * Author: jelangevoort
 *
 * Created on September 7, 2011, 6:25 PM
 *
 * Looks for poles
 *
 * Input
 *  activation
 *  float* [width*height]
 *  [0-1] ... activation activation activation ...
 *
 * Output
 *  ???
 *
 * NYI:
 *
 */

#ifndef RECOGNITION_H
#define	RECOGNITION_H

#include<omp.h>                 // openMP for parallel processing

#include <vector>
#include <boost/shared_array.hpp>

#include "recognition/VerticalActivationCounter.hpp"
#include "recognition/SimplePolePositionDetector.hpp"

typedef std::vector<VerticalActivationCounter> ActivationCounterVector;
typedef boost::shared_array<float> SharedFloatArray;

class Recognition
{
public:
    Recognition(int width, int height, bool estSexangvlatvm);
    ~Recognition();

    void process(float* activation, float* blobs);
    
    float* getVerticalScores();
    
    float* getDetectionProbabilities();
    
    int getPoleCount();
    float getPolePosition(int i);
    float getPoleDistance(int i);
private:
    // feature parameters
    int width;
    int height;
    int length;
    bool estSexangvlatvm;
    
    // data structures
    bool initialized;
    ActivationCounterVector activationCounters;
    
    SharedFloatArray rawVerticalActivationValues;

    PolePositionDetector ppDetector;
    
    SharedFloatArray poleDetectionProbabilities;
    
    float rawDistance;

    void firstInit(float* activation, float* blobs);
};

#endif	/* RECOGNITION_H */



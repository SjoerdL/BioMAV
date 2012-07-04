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
 * File:   Recognition.cpp
 * Author: jlangevoort
 * 
 * Created on August 16, 2011, 10:03 AM
 */

#include "Recognition.h"

#include <cmath>

#include <iostream>

using namespace std;

/**
 * Constructor
 *  initializes input parameters and data structures
 *
 * @param width;                width of the input activation
 * @param height;               height of the input activation
 * @param estSexangvlatvm;      whether or not it is a hexagonal representation
 */
Recognition::Recognition(int width, int height, bool estSexangvlatvm) : ppDetector(width)
{
  // initialize input parameters (not necessary)
  this->width = width;
  this->height = height;
  this->estSexangvlatvm = estSexangvlatvm;
  
  initialized = false;
}

/**
 * Destructor
 */
Recognition::~Recognition()
{
	
}

float* Recognition :: getVerticalScores() {
  return rawVerticalActivationValues.get();
}

float* Recognition :: getDetectionProbabilities() {
  return poleDetectionProbabilities.get();
}

int Recognition :: getPoleCount() {
  return ppDetector.isVisible() ? 1 : 0;
}

float Recognition :: getPolePosition(int i) {
  return ppDetector.getAngle();
}

float Recognition :: getPoleDistance(int i) {
  return rawDistance;
}


/**
 * Processes the recognition
 *  runs each of the recognition modules and returns activation for output
 *
 * @param activation;		pole-like objects seperated from the background
 * @param blobs;			color coded (orange)
 */
void Recognition::process(float* activation, float* blobs)
{
  firstInit(activation, blobs);
  for (int x = 0; x < width; x++) {
    rawVerticalActivationValues[x] = activationCounters[x].calculateActivation();
  }
  
  ppDetector.processData(getVerticalScores());

  for (int x = 0; x < width; x++) {
    poleDetectionProbabilities[x] = 1.0 / sqrt(2.0 * M_PI) / ppDetector.getStdDev() * exp(- ((x - ppDetector.getMean()) * (x - ppDetector.getMean())) / (2.0 * ppDetector.getStdDev() * ppDetector.getStdDev()));
  }

  // Evaluate closeness
  if (ppDetector.isVisible()) {
    int istddev = (int) ceil(ppDetector.getStdDev());
    int icenter = (int) round(ppDetector.getMean());
    
    int cutCount = 0;
    
    int lowBound = icenter - istddev;
    if (lowBound < 0) {
      cutCount += -lowBound;
      lowBound = 0;
    }
    
    int highBound = icenter + istddev;
    if (highBound > width - 1) {
      cutCount += highBound - (width - 1);
      highBound = width - 1;
    }
    
    float activationSum = 0;
    for (int x = lowBound; x <= highBound; x++) {
      for (int y = 0; y < height; y++) {
        activationSum += blobs[x + y * width];
      }
    }
    activationSum /= height;
    activationSum *= 2.0 * istddev / (2.0 * istddev - cutCount);

    rawDistance = 1.0 / activationSum;    
  }
}

void Recognition :: firstInit(float* activation, float* blobs) {
  if (!initialized) {
    for (int x = 0; x < width; x++) {
      activationCounters.push_back(VerticalActivationCounter(activation, width, height, x, -1));
    }
    rawVerticalActivationValues = SharedFloatArray(new float[width]);
    poleDetectionProbabilities = SharedFloatArray(new float[width]);
    
    initialized = true;
  }
}

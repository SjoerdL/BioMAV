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
#include "SimplePolePositionDetector.hpp"

#include <cmath>

#include <iostream>

// TODO: Make this framerate independent

#define FORGETTINESS 0.666
#define NEW_STDDEV_MIXING_FACTOR 0.4
#define MIN_STDDEV 2
#define MIN_WEIGHT 0.05

#define STDDEV_THRESHOLD 10

using namespace std;

float probabilityFunction(float x, float sigma) {
  return 1.0 / sqrt(2.0 * M_PI) / sigma * exp(- (x * x) / (2.0 * sigma * sigma));
}

void PolePositionDetector :: processData(float* activations) {
  stddev += FORGETTINESS;

  float weightSum = 0;
  float dataMean = 0;
  
  for (int x = 0; x < width; x++) {
    if (activations[x] > POLE_DETECTION_THRESHOLD) {
      float weight = probabilityFunction(x - mean, stddev) + MIN_WEIGHT;
      weightSum += weight;
      dataMean += weight * x;
    }
  }
  
  if (weightSum > 0) {
    dataMean /= weightSum;
    
    float dataStddev = 0;
    for (int x = 0; x < width; x++) {
      if (activations[x] > POLE_DETECTION_THRESHOLD) {
        float weight = probabilityFunction(x - mean, stddev) + MIN_WEIGHT;
        dataStddev += weight * (x - dataMean) * (x - dataMean);
      }
    }
    dataStddev = sqrt(dataStddev / weightSum);
    
    mean = mean * (1 - NEW_STDDEV_MIXING_FACTOR) + NEW_STDDEV_MIXING_FACTOR * dataMean;
    stddev = stddev * (1 - NEW_STDDEV_MIXING_FACTOR) + NEW_STDDEV_MIXING_FACTOR * dataStddev;
    if (stddev < MIN_STDDEV) {
      stddev = MIN_STDDEV;
    }
  }
  
  visible = stddev < STDDEV_THRESHOLD;
  if (visible) {
    angle = (mean - width / 2) / (width / 2) * M_PI / 4;
  }
  
//  cout << visible << " " << angle << endl;
}

bool PolePositionDetector :: isVisible() {
  return visible;
}

float PolePositionDetector :: getAngle() {
  return angle;
}

float PolePositionDetector :: getMean() {
  return mean;
}

float PolePositionDetector :: getStdDev() {
  return stddev;
}

PolePositionDetector :: PolePositionDetector(int width) {
  visible = false;
  this->width = width;
  mean = 0;
  stddev = 1000;
}

PolePositionDetector :: ~PolePositionDetector() {
}

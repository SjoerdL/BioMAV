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
#include "VerticalActivationCounter.hpp"

#include <cmath>

#include <iostream>

using namespace std;

#define OPENING_ANGLE (15.0 / 180.0 * M_PI / 2)
#define WEIGHT_FUNCTION_WIDTH (3.0 / 2.0)

#define ACTIVATION_GRADIENT_PENALTY 0.5

namespace VAC {
  float weightFunction(float x) {
    return exp(-(x * x) / (WEIGHT_FUNCTION_WIDTH * WEIGHT_FUNCTION_WIDTH));
  }
  
  float calculateHorizontalActivation(int x, float scanWidth, int dataWidth, float* scanlineData) {
    float weightSum = 0;
    float activationSum = 0;
    
    for (int windex = 0; windex <= ((int) ceil(scanWidth)); windex++) {
      float this_weight = weightFunction(windex);

      if (windex > 0) {
        if (x - windex >= 0) {
          activationSum += this_weight * scanlineData[x - windex];
          weightSum += this_weight;
        }
      } 

      if (x + windex < dataWidth) {
        activationSum += this_weight * scanlineData[x + windex];
        weightSum += this_weight;
      }
    }

    return activationSum / weightSum;
  }
}

using namespace VAC;

float VerticalActivationCounter :: calculateActivation() {
  float result = 0;
  
  int counter = 0;
  float openingGradient = tan(OPENING_ANGLE);
  
  float lastActivation = -1;
  
  for (int y = (direction > 0 ? 0 : height - 1); (y >= 0) && (y < height); y += direction) {
    float activation = VAC::calculateHorizontalActivation(startX, counter * openingGradient, width, buffer + width * y);

    result += activation;
    if (counter > 0) {
      result -= ACTIVATION_GRADIENT_PENALTY * lastActivation;
    }

    counter++;
    
    lastActivation = activation;
  }
  
  return result;
}

VerticalActivationCounter& VerticalActivationCounter :: operator=(const VerticalActivationCounter& other) {
  buffer = other.buffer;
  width = other.width;
  height = other.height;
  startX = other.startX;
  direction = other.direction;

  return *this;
}

VerticalActivationCounter :: VerticalActivationCounter(float* buffer, int width, int height, int startX, int direction) {
  this->buffer = buffer;
  this->width = width;
  this->height = height;
  this->startX = startX;
  this->direction = direction < 0 ? -1 : 1;
}

VerticalActivationCounter :: ~VerticalActivationCounter() {
}

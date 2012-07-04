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
 * File:   Kinect.h
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 11:29 AM
 *
 * Creates kinect device,
 *  class used to handle all kinect related activities used by the rest of the program
 */

#ifndef KINECT_H_
#define KINECT_H_

#include <libfreenect/libfreenect.hpp>
#include "Mutex.h"
#include "MyFreenectDevice.h"

using namespace std;

class Kinect
{
public:
    Kinect();
    ~Kinect();

    uint8_t* getData();
    uint8_t* getDepth();

    void updateState(){device->updateState();};
private:
    Freenect::Freenect freenect;
    MyFreenectDevice* device;

    uint8_t* rgb;
    uint8_t* depth;
};

#endif /* KINECT_H_ */
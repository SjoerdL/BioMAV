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
 * File:   MyFreenectDevice.h
 * Author: jelangevoort
 * Based on: OpenKinect "C++OpenGLExample"
 *
 * Created on August 14, 2011, 11:27 AM
 *
 * Kinect device
 */

#ifndef MYFREENECTDEVICE_H_
#define MYFREENECTDEVICE_H_

#include <vector>           // vector class template
#include <math.h>           // generic math functions
#include <pthread.h>        // POSIX threads
#include <stdint.h>         // exact-width integer types

#include <libfreenect/libfreenect.hpp>
#include "Mutex.h"

using namespace std;

class MyFreenectDevice : public Freenect::FreenectDevice
{
public:
    MyFreenectDevice(freenect_context *_ctx, int _index);

    void VideoCallback(void* _rgb, uint32_t timestamp);
    void DepthCallback(void* _depth, uint32_t timestamp);
    bool getRGB(uint8_t *buffer);
    bool getDepth(uint8_t &buffer);

private:
    uint8_t* m_buffer_depth;
    uint8_t* m_buffer_video;
    uint16_t* m_gamma;
    Mutex m_rgb_mutex;
    Mutex m_depth_mutex;
    bool m_new_rgb_frame;
    bool m_new_depth_frame;
};

#endif /* MYFREENECTDEVICE_H_ */
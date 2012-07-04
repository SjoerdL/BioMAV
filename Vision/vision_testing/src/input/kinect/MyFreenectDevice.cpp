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
 * File:   MyFreenectDevice.cpp
 * Author: jelangevoort
 * Based on: OpenKinect "C++OpenGLExample"
 * 
 * Created on August 14, 2011, 11:27 AM
 */

#include "MyFreenectDevice.h"
#include <stdint.h>
#include <stdio.h>

/**
 * Constructor
 */
MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index) : Freenect::FreenectDevice(_ctx, _index)
{
    m_buffer_depth = new uint8_t [640*480*3];
    m_buffer_video = new uint8_t [640*480*3],
    m_gamma = new uint16_t [2048];
    m_new_rgb_frame = false;
    m_new_depth_frame = false;

    for(unsigned int i = 0 ; i < 2048 ; i++)
    {
        float v = i/2048.0;
        v = pow(v, 3)* 6;
        m_gamma[i] = v*6*256;
    }
}

/**
 * Gets video from the Kinect device
 */
void MyFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp)
{
    m_rgb_mutex.lock();
    uint8_t* rgb = static_cast<uint8_t*>(_rgb);
    copy(rgb, rgb+640*480*3, m_buffer_video);
    m_new_rgb_frame = true;
    m_rgb_mutex.unlock();
}

/**
 * Gets depth information from the Kinect device
 */
void MyFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp)
{
    m_depth_mutex.lock();
    uint16_t* depth = static_cast<uint16_t*>(_depth);
    for( unsigned int i = 0 ; i < 640*480 ; i++)
    {
        int pval = m_gamma[depth[i]];
        int lb = pval & 0xff;
        switch (pval>>8)
        {
            case 0:
                m_buffer_depth[3*i+0] = 255;
                m_buffer_depth[3*i+1] = 255-lb;
                m_buffer_depth[3*i+2] = 255-lb;
                break;
            case 1:
                m_buffer_depth[3*i+0] = 255;
                m_buffer_depth[3*i+1] = lb;
                m_buffer_depth[3*i+2] = 0;
                break;
            case 2:
                m_buffer_depth[3*i+0] = 255-lb;
                m_buffer_depth[3*i+1] = 255;
                m_buffer_depth[3*i+2] = 0;
                break;
            case 3:
                m_buffer_depth[3*i+0] = 0;
                m_buffer_depth[3*i+1] = 255;
                m_buffer_depth[3*i+2] = lb;
                break;
            case 4:
                m_buffer_depth[3*i+0] = 0;
                m_buffer_depth[3*i+1] = 255-lb;
                m_buffer_depth[3*i+2] = 255;
                break;
            case 5:
                m_buffer_depth[3*i+0] = 0;
                m_buffer_depth[3*i+1] = 0;
                m_buffer_depth[3*i+2] = 255-lb;
                break;
            default:
                m_buffer_depth[3*i+0] = 0;
                m_buffer_depth[3*i+1] = 0;
                m_buffer_depth[3*i+2] = 0;
                break;
        }
    }
    m_new_depth_frame = true;
    m_depth_mutex.unlock();
}

/**
 * Checks whether the Kinect device has given a new RGB-frame
 *      If so, the information is loaded into the buffer
 */
bool MyFreenectDevice::getRGB(uint8_t *buffer)
{
    m_rgb_mutex.lock();
    if(m_new_rgb_frame)
    {
        copy(m_buffer_video, m_buffer_video+640*480*3, buffer);
        m_new_rgb_frame = false;
        m_rgb_mutex.unlock();
        return true;
    }
    else
    {
        m_rgb_mutex.unlock();
        return false;
    }
}

/**
 * Checks whether the Kinect device has given new depth information
 *      If so, the information is loaded into the buffer
 */
bool MyFreenectDevice::getDepth(uint8_t &buffer)
{
    m_depth_mutex.lock();
    if(m_new_depth_frame)
    {
        buffer = *m_buffer_depth;
        m_new_depth_frame = false;
        m_depth_mutex.unlock();
        return true;
    }
    else
    {
        m_depth_mutex.unlock();
        return false;
    }
}
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
 * File:   Mutex.h
 * Author: jelangevoort
 * Based on: OpenKinect "C++OpenGLExample"
 *
 * Created on August 14, 2011, 11:25 AM
 * 
 * Mutual exclusion for the kinect functions
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include <pthread.h>

class Mutex
{
public:
    Mutex();
    ~Mutex();
    void lock();
    void unlock();
private:
    pthread_mutex_t m_mutex;
};

#endif /* MUTEX_H_ */
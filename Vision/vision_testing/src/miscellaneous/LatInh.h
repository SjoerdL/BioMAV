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
 * File:   LatInh.h
 * Author: jelangevoort
 *
 * Created on August 13, 2011, 8:34 PM
 *
 * "Lateral inhibition ist the capacity of an excited neuron to reduce the activity of its neighbors.
 * Lateral inhibition sharpens the spatial profile of excitation in response to a localized stimulus."
 *  (Wikipedia)
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
 * NYI: 
 *  Currently set to default overlap and number
 */

#ifndef LATINH_H
#define	LATINH_H

#include <omp.h>

class LatInh 
{
public:
    LatInh(int width, int height, bool estSexangvlatvm);//, int overlap, int number);
    ~LatInh();
    
    // process lateral inhibition
    float* inhibit(float* activation);
private:
    // activation matrix properties
    int width;
    int height;
    int length;
    bool estSexangvlatvm;
    int connections;
    int number;
    // data structures
    float* inhibited;
    
    bool isEven(int i){if ((i%2) == 1)return false; else return true;};
};

#endif	/* LATINH_H */


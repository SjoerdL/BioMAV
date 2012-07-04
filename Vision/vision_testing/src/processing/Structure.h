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
 * File:   Structure.h
 * Author: jelangevoort
 *
 * Created on September 7, 2011, 6:25 PM
 *
 * Looks for vertical structures in the input stream
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
 *
 */

#ifndef STRUCTURE_H
#define	STRUCTURE_H

#include<omp.h>                 // openMP for parallel processing

class Structure
{
public:
    Structure(int width, int height, bool estSexangvlatvm);
    ~Structure();

    void process(float* blobs, float* contours);

    float* getStructures(){return structures;};
private:
    // feature parameters
    int width;
    int height;
    int length;
    bool estSexangvlatvm;
    // data structures
    float* structures;

    bool isEven(int i){if ((i%2) == 1)return false; else return true;};
};

#endif	/* STRUCTURE_H */



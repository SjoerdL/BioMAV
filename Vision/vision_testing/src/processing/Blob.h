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
 * File:   Blob.h
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 6:25 PM
 *
 * Extracts information on specific colors by looking at the ratio between different input cells
 *  thus negating any intensity in the activation
 *
 * Input
 *  activation, activation (of a different color channel)
 *  float* [width*height], idem.
 *  [0-1] ... activation activation activation ..., idem.
 *
 * Output
 *  activation
 *  float* [width*height]
 *  [0-1] ... activation activation activation ...
 *
 * NYI:
 *  Colors other than 'orange'
 */

#ifndef BLOB_H
#define	BLOB_H

#include<omp.h>                 // openMP for parallel processing

class Blob
{
public:
    Blob(int width, int height, bool estSexangvlatvm);
    ~Blob();

    void process(float* colorRed, float* colorGreen);

    float* getBlobs(){return blobs;};
private:
    // feature parameters
    int width;
    int height;
    int length;
    bool estSexangvlatvm;
    // data structures
    float* blobs;

    bool isEven(int i){if ((i%2) == 1)return false; else return true;};
};

#endif	/* BLOB_H */

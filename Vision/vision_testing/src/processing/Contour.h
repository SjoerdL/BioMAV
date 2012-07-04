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
 * File:   Contour.h
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 6:25 PM
 * 
 * Combines ON/OFF edge activation with elementary motion detector activation
 * With little to no movement, edge detection will be good, but no EMD
 *  with movement, edge detection will be worse, yet EMD will be better
 *
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
 *  EMD information combined with the edge information
 *  Perhaps merge with feature
 */

#ifndef CONTOUR_H
#define	CONTOUR_H

#include<omp.h>                 // openMP for parallel processing

class Contour 
{
public:
    Contour(int width, int height, bool estSexangvlatvm);
    ~Contour();

    void process(float* linesX, float* linesY, float* linesZ, float* emdNegX, float* emdNegY, float* emdNegZ, float* emdX, float* emdY, float* emdZ);

    float* getContours(){return contours;};
    float* getHough(){return hough;};
private:
    // activation parameters
    int height;
    int width;
    int length;
    bool estSexangvlatvm;
    // data structures
    float* contours;
    float* anglesX;
    float* anglesY;
    float** distances;

    int houghLength;
    float* hough;

    bool isEven(int i){if ((i%2) == 1)return false; else return true;};
};

#endif	/* CONTOUR_H */


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
 * File:   Line.h
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 6:25 PM
 *
 * Extracts edge information in one of the 2/3 major directions, (X,Y(,Z))
 * The boundaries between ON and OFF local edge detectors signify the presence
 *  of an edge with a certain direction/orientation
 * These cells are structured such that the neighboring cells need to have a
 *  specific activation in certain direction, similar to the ON center OFF
 *  surround structure of the local edge detectors, but with a small alteration
 *  to detect lines rather than mere edges.
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
 *  As of yet the difference between an edge and a lines is ignored, this could have consequences
 *   for the constriuction of contours, yet it does not seem unreasonable to assume that there
 *   are no edges that are a part of a larger object, could also be solved in the construction of
 *   the contours itself.
 *
 */

#ifndef LINE_H
#define	LINE_H

#include<omp.h>                 // openMP for parallel processing

class Line
{
public:
    Line(int width, int height, bool estSexangvlatvm);
    ~Line();

    void process(float* edgeOn, float* edgeOff);

    float* getLines(){return lines;};

    float* getLinesX(){return linesX;};
    float* getLinesY(){return linesY;};
    float* getLinesZ(){return linesZ;};
private:
    // feature parameters
    int width;
    int height;
    int length;
    bool estSexangvlatvm;
    // data structures
    float* lines;

    float* linesX;
    float* linesY;
    float* linesZ;

    bool isEven(int i){if ((i%2) == 1)return false; else return true;};
};

#endif	/* LINE_H */


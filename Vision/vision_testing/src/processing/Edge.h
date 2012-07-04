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
 * File:   Edge.h
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 6:24 PM
 * 
 * Calculates the edge information given a certain activation as input
 *  It takes pixels and use either the closest 4, or 8 pixels,
 *   or it takes hexels and uses the closest 6 hexels to calculate the edge information
 *  The edge information is calculated both in ON (center) and OFF (center)
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
 *  scale variants, currently it always has maximal overlap,
 *   i.e. each pixel/hexel gives rise to one local edge detector+
 *  combination of ON and OFF channels, an edge is on the boundary between ON
 *   and OFF activation
 */

#ifndef EDGE_H
#define	EDGE_H

#include<omp.h>                 // openMP for parallel processing

class Edge 
{
public:
    Edge(int width, int height, int conversionType);
    ~Edge();
    
    // process edge detection
    void process(float* activation);
    // return data
    float* getEdgeOn(){return edgeOn;};
    float* getEdgeOff(){return edgeOff;};
private:
    // edge detection parameters
    int width;
    int height;
    int length;
    int conversionType;
    // data structures
    float* edgeOn;
    float* edgeOff;
    
    bool isEven(int i){if ((i%2) == 1)return false; else return true;};
};

#endif	/* EDGE_H */


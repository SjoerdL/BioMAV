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
 * File:   Processing.cpp
 * Author: jlangevoort
 * 
 * Created on August 16, 2011, 10:03 AM
 */

#include "Processing.h"

// define conversion types
enum conversion
{
    quattuor,
    sex,
    octo
};

/**
 * Constructor
 *  initializes input parameters and data structures
 *
 * @param width;                width of the input activation
 * @param height;               height of the input activation
 * @param estSexangvlatvm;      whether or not it is a hexagonal representation
 */
Processing::Processing(int width, int height, bool estSexangvlatvm)
{
    // initialize input parameters (not necessary)
    this->width = width;
    this->height = height;
    this->estSexangvlatvm = estSexangvlatvm;

    // parameters
    //  EMD
    float tHigh = 1.5;
    float tLow = 5;
    //  LED
    int edgeType = sex;

    // initialize different processing classes
    blob = new Blob (width, height, estSexangvlatvm);
    edge = new Edge (width, height, edgeType);
    emd = new Emd (width, height, tHigh, tLow, estSexangvlatvm);
	line = new Line (width, height, estSexangvlatvm);
    contour = new Contour (width, height, estSexangvlatvm);
	structure = new Structure (width, height, estSexangvlatvm);

    // initialize additional classes
    latInh = new LatInh (width, height, estSexangvlatvm);
}

/**
 * Destructor
 */
Processing::~Processing()
{
	delete[] blob;
  delete[] edge;
  delete[] emd;
  delete[] line;
  delete[] contour;
  delete[] structure;
  delete[] latInh;
}

float* Processing::combine(float* a, float* b, float* c)
{
	int i;
	for(i=0; i< width*height; i++)
	{
		a[i] = (a[i] + b[i] + c[i])/3;
	}
	
	return a;
}

/**
 * Processes the processing
 *  runs each of the processing modules and returns activation for output
 *
 * @param input;            input activation, i.e. photoreceptor cells
 * @return activation for testing purposes
 */
float* Processing::process(float* input, float* input2)
{
	blob->process(input2, input);
    emd->process(blob->getBlobs());
    edge->process(blob->getBlobs());

    line->process(edge->getEdgeOn(), edge->getEdgeOff());
    
    contour->process(line->getLinesX(), line->getLinesY(), line->getLinesZ(), 
                     emd->getMotionNegX(), emd->getMotionNegY(), emd->getMotionNegZ(),
                     emd->getMotionX(), emd->getMotionY(), emd->getMotionZ());
    
    structure->process(blob->getBlobs(), contour->getContours());
    
    // -- INPUT -- //
    //return input;
    // -- BLOBS -- //
    //return blob->getBlobs();
    // -- EDGES -- //
    //return edge->getEdgeOn();
    //return edge->getEdgeOff();
    // -- EMDS -- //
    //return emd->getMotionX();
    //return emd->getMotionY();
    //return emd->getMotionZ();
    //return emd->getMotionNegX();
    //return emd->getMotionNegY();
    //return emd->getMotionNegZ();
    //return emd->getMotionStrength();
    // -- LINES -- //
    //return combine(line->getLinesX(), line->getLinesY(), line->getLinesZ());
    //return line->getLinesX();
    //return line->getLinesY();
    //return line->getLinesZ();
    // -- CONTOURS -- //
    //return contour->getContours();
    //return contour->getHough();
    // -- STRUCTURES -- //
    return structure->getStructures();
}

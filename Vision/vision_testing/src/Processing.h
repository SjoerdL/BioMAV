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
 * File:   Processing.h
 * Author: jlangevoort
 *
 * Created on August 16, 2011, 10:03 AM
 *
 * Class governing processing tasks
 */

#ifndef PROCESSING_H
#define	PROCESSING_H

#include "processing/Blob.h"           	// color coding class
#include "processing/Contour.h"     	// contour processing class
#include "processing/Edge.h"        	// local edge detecting class
#include "processing/Emd.h"         	// elementary motion detection class
#include "processing/Line.h"        	// feature extraction class
#include "processing/Structure.h"		// structure construction class

#include "miscellaneous/LatInh.h"       // lateral inhibition class

class Processing
{
public:
    Processing(int width, int height, bool isSexAngvlvs);
    ~Processing();

    float* process(float* input, float* input2);
    float* getBlobData() {return blob->getBlobs();};
private:
    // input parameters
    int width;
    int height;
    bool estSexangvlatvm;
    // processing classes
    Blob* blob;
    Contour* contour;
    Edge* edge;
    Emd* emd;
    Line* line;
    Structure* structure;
    // additional classes
    LatInh* latInh;
    
    // testing
    float* combine(float* a, float* b, float* c);
};

#endif	/* PROCESSING_H */


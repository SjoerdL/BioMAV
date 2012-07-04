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
 * File:   Blob.cpp
 * Author: jelangevoort
 *
 * Created on September 08, 2011, 9:05 AM
 */

#include "Blob.h"

#include <stdio.h>

/**
 * Constructor
 *  initializes input parameters and data structures
 *
 * @param width;                width of the input activation
 * @param height;               height of the input activation
 * @param estSexangvlatvm;      whether or not it is a hexagonal representation
 */
Blob::Blob(int width, int height, bool estSexangvlatvm)
{
    int i;
    // initialize blob parameters
    this->width = width;
    this->height = height;
    length = width * height;
    this->estSexangvlatvm = estSexangvlatvm;

    // allocate memory for data structures
    blobs = new float [length];

    // initialize data structures
    for (i = 0; i < length; i++)
    {
        blobs[i] = 0;
    }
}

/**
 * Destructor
 */
Blob::~Blob()
{
    delete[] blobs;
}

/**
 * Processes different color channels to determine color components
 *
 * @param colorRed;				first color channel
 * @param colorGreen;          	second color channel
 */
void Blob::process(float* colorRed, float* colorGreen)
{
    int i;
    float max = 0;
    
    if ( estSexangvlatvm )
    {
		float activation;

		#pragma omp parallel for private(i, activation)
        for (i = 0; i < length; i++)
        {
			colorRed[i] += 0.0001;
			colorGreen[i] += 0.0001;
			
			blobs[i] = colorRed[i] / colorGreen[i];
			
			/*
			if (colorRed[i] > colorGreen[i])
			{
				blobs[i] = colorRed[i] / colorGreen[i];
			}
			else
			{
				blobs[i] = colorGreen[i] / colorRed[i];
			}
			*/
			
			if (blobs[i] > max)
			{
				max = blobs[i];
			}
		}
		
		/*
        bool indent = false;

        // TODO: resolve segmentation faults more intelligently
        #pragma omp parallel for private(i, indent)
        for (i = 0; i < length; i++)
        {
            // when the end of a line (width) is reached
            if (isEven(i/width))
            {
                indent = false;
            }
            else
            {
                indent = true;
            }

            // process local edge detectors -> to lines
            // TODO: RATIO BETWEEN THE EDGES OR TOTAL ACTIVATION?
            if (indent)
            {

            }
            else
            {

            }
        }
        */
    }
    else // if ( estQvadriangvlatvm )
    {
        #pragma omp parallel for private(i)
        for (i = 0; i < length; i++)
        {

        }
    }
    
    // NO NORMALIZATION
    // factorize instead
    // temp variable
    float BLOBFACTORIZATION = 0.5;
	#pragma omp parallel for private(i)
	for (i = 0; i < length; i++)
	{
		//blobs[i] = blobs[i] / max;
		blobs[i] = blobs[i] * BLOBFACTORIZATION;
		
		if (blobs[i] > 1)
		{
			blobs[i] = 1;
//			printf("BLOBS %f\n", blobs[i]);
		}
	}
	
}

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
 * File:   Structure.cpp
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 6:25 PM
 */

#include "Structure.h"

#include <stdio.h>

/**
 * Constructor
 *  initializes input parameters and data structures
 *
 * @param width;                width of the input activation
 * @param height;               height of the input activation
 * @param estSexangvlatvm;      whether or not it is a hexagonal representation
 */
Structure::Structure(int width, int height, bool estSexangvlatvm)
{
    int i;
    // initialize blob parameters
    this->width = width;
    this->height = height;
    length = width * height;
    this->estSexangvlatvm = estSexangvlatvm;

    // allocate memory for data structures
    structures = new float [length];

    // initialize data structures
    for (i = 0; i < length; i++)
    {
        structures[i] = 0;
    }
}

/**
 * Destructor
 */
Structure::~Structure()
{
    delete[] structures;
}

/**
 * Processes contour activation and extracts structures, e.g. lines in a specific direction
 *
 * @param contours;   		contour activation
 */
void Structure::process(float* blobs, float* contours)
{
    int i, j;
    float max = 0;
    float thBlob = 0.65;
    float thContours = 0.025;
    float thStructures = 0.50;
    float thStrucs = 0.50;
    
    if ( estSexangvlatvm )
    {
		#pragma omp parallel for private(i)
		for (i = 0; i < length; i++)
		{
			if (contours[i] > thContours)
			{
				if (blobs[i] > thBlob)
				{
					structures[i] = blobs[i];
				}
				else if (blobs[i-1] > thBlob || blobs[i+1] > thBlob)
				{
					structures[i] = blobs[i];
				}
				else
				{
					structures[i] = 0;
				}
			}
			else
			{
				structures[i] = 0;
			}
		}
		
		// color in poles
		//#pragma omp parallel for private(i)
		for (i = 0; i < width; i++)
		{
			float activation = 0;
			for (j = 0; j < height; j++)
			{
				if (structures[i + j*width] > 0 || structures[i + (j-1)*width] > 0 || structures[i + (j+1)*width] > 0)
				{
					activation += structures[i + j*width];
				}
			}
			
			activation = activation / height;
			for (j = 0; j < height; j++)
			{
				if (structures[i + j*width] > thStructures)
					structures[i + j*width] = activation;
			}
		}
		
		max = 0;
		// check max
		for (i = 0; i < length; i++)
		{
			if (structures[i] > max)
			{
				max = structures[i];
			}
		}

		/*
        // TODO: resolve segmentation faults more intelligently
        // for each horizontal line
        bool indent = false;

        // TODO: resolve segmentation faults more intelligently
        // for each local edge detector
        #pragma omp parallel for private(i, indent)
        for (i = width+1; i < length-width-1; i++)
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
				structures[i] = (contours[i-width] + contours[i-width+1] + contours[i] + contours[i+width] + contours[i+width+1]) - (contours[i-1] + contours[i+1]);
				structures[i] += blobs[i];
			}
			else // no indent
			{
				structures[i] = (contours[i-width-1] + contours[i-width] + contours[i] + contours[i+width-1] + contours[i+width]) - (contours[i-1] + contours[i+1]);
				structures[i] += blobs[i];
			}
			
            if ( structures[i] < 0 )
            {
                structures[i] = 0;
            }

            if ( structures[i] > max )
            {
                max = structures[i];
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
    float STRUCTUREFACTORIZATION = 1;
    
    bool normalize = true;
    // normalize
    if (normalize)
    {
        if ( estSexangvlatvm )
        {
            #pragma omp parallel for private(i)
            for (i = 0; i < length; i++)
            {
				//structures[i] = structures[i] / max;
				structures[i] = structures[i] * STRUCTUREFACTORIZATION;
				
				if (structures[i] > 1)
				{
//					printf("STRUCTURES %f\n", max);
					structures[i] = 1;
				}
            }
        }
        else // if ( estQvadriangvlatvm )
        {
            #pragma omp parallel for private(i)
            for (i = 0; i < length; i++)
            {

            }
        }
    }
}


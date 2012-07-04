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
 * File:   Edge.cpp
 * Author: jelangevoort
 * 
 * Created on August 14, 2011, 6:24 PM
 */

#include "Edge.h"

#include <stdio.h>

// define conversion types, number of connections
enum conversion
{
    quattuor,
    sex,
    octo
};

/**
 * Constructor
 * 
 * @param width
 * @param height
 * @param conversionType
 */
Edge::Edge(int width, int height, int conversionType)
{
    int i;
    
    // initialize edge detection parameters
    this->width = width;
    this->height = height;
    length = width * height;
    this->conversionType = conversionType;
    
    // allocate memory for edge maps
    edgeOn = new float [length];
    edgeOff = new float [length];
    
    // initialize data structures
    for (i = 0; i < length; i++)
    {
        edgeOn[i] = 0;
        edgeOff[i] = 0;
    }
}

/**
 * Destructor
 */
Edge::~Edge() 
{
    delete[] edgeOn;
    delete[] edgeOff;
}

/**
 * Processes input with a standard edge detection algorithm (ON/OFF, center/surround)
 * 
 * @param activation;           activation of the photo receptors
 */
void Edge::process(float* activation)
{
    int i;
    float maxOn = 0, maxOff = 0;
    
    if(conversionType == quattuor)
    {
        // TODO: resolve segmentation faults more intelligently
        // for each local edge detector
        #pragma omp parallel for private(i)
        for (i = 0; i < length; i++)
        {
            edgeOn[i] = activation[i] - 
                (activation[i-width] + activation[i-1] + 
                activation[i+width] + activation[i+1]) / 4;
            edgeOff[i] = (1-activation[i]) - 
                (1-(activation[i-width] + activation[i-1] + 
                activation[i+width] + activation[i+1]) / 4);
            
            // if the activation is negative, set activation to 0
            if(edgeOn[i] < 0)
            {
                edgeOn[i] = 0;
            }
            if(edgeOff[i] < 0)
            {
                edgeOff[i] = 0;
            }
            
			if(i%width < 1 || i%width > width-2)
			{
				edgeOn[i] = 0;
				edgeOff[i] = 0;
			}
            
            if (edgeOn[i] > maxOn)
            {
                maxOn = edgeOn[i];
            }
            if (edgeOff[i] > maxOff)
            {
                maxOff = edgeOff[i];
            }
        }
    }
    else if (conversionType == octo)
    {
        // TODO: resolve segmentation faults more intelligently
        // for each local edge detector
        #pragma omp parallel for private(i)
        for (i = 0; i < length; i++)
        {
            edgeOn[i] = activation[i] - 
                ((activation[i-width] + activation[i-1] + activation[i+width] + activation[i+1]) / 6 +
                (activation[i-width-1] + activation[i-width+1] + activation[i+width-1] + activation[i+width+1]) / 12);
            edgeOff[i] = (1-activation[i]) - 
                (1-(activation[i-width] + activation[i-1] + activation[i+width] + activation[i+1]) / 6) -
                (1-(activation[i-width-1] + activation[i-width+1] + activation[i+width-1] + activation[i+width+1]) / 12);
                        
            // if the activation is negative, set activation to 0
            if(edgeOn[i] < 0)
            {
                edgeOn[i] = 0;
            }
            if(edgeOff[i] < 0)
            {
                edgeOff[i] = 0;
            }
            
			if(i%width < 1 || i%width > width-2)
			{
				edgeOn[i] = 0;
				edgeOff[i] = 0;
			}
            
            if (edgeOn[i] > maxOn)
            {
                maxOn = edgeOn[i];
            }
            if (edgeOff[i] > maxOff)
            {
                maxOff = edgeOff[i];
            }
        }
    }
    else // if (conversionType == sex)
    {
        bool indent = true;
        
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
            
            // process local edge detectors
            if (indent)
            {
                edgeOn[i] = activation[i] -
                    (activation[i-width] + activation[i-width+1] +
                    activation[i-1] + activation[i+1] + 
                    activation[i+width] + activation[i+width+1]) / 6;
                
                edgeOff[i] = (1-activation[i]) -
                    (1-(activation[i-width] + activation[i-width+1] +
                    activation[i-1] + activation[i+1] + 
                    activation[i+width] + activation[i+width+1]) / 6);
            }
            else
            {
                edgeOn[i] = activation[i] -
                    (activation[i-width-1] + activation[i-width] +
                    activation[i-1] + activation[i+1] + 
                    activation[i+width-1] + activation[i+width]) / 6;
                
                edgeOff[i] = (1-activation[i]) -
                    (1-(activation[i-width-1] + activation[i-width] +
                    activation[i-1] + activation[i+1] + 
                    activation[i+width-1] + activation[i+width]) / 6);
            }
            
            // if the activation is negative, set activation to 0
            if(edgeOn[i] < 0)
            {
                edgeOn[i] = 0;
            }
            if(edgeOff[i] < 0)
            {
                edgeOff[i] = 0;
            }
                    
			if(i%width < 1 || i%width > width-2)
			{
				edgeOn[i] = 0;
				edgeOff[i] = 0;
			}
            
            if (edgeOn[i] > maxOn)
            {
                maxOn = edgeOn[i];
            }

            if (edgeOff[i] > maxOff)
            {
                maxOff = edgeOff[i];
            }
        }
    }

    // NO NORMALIZATION
    // factorize instead
    // temp variable
    float EDGEONFACTORIZATION = 1.7;
    float EDGEOFFFACTORIZATION = 1.7;
    
    bool normalize = true;
    // normalize if necessary
    if ( normalize )
    {
        #pragma omp parallel for private(i)
        for (i = 0; i < length; i++)
        {
            //edgeOn[i] = edgeOn[i] / maxOn;
            //edgeOff[i] = edgeOff[i] / maxOff;
            
            edgeOn[i] = edgeOn[i] * EDGEONFACTORIZATION;
            edgeOff[i] = edgeOff[i] * EDGEOFFFACTORIZATION;
            
			if (edgeOn[i] > 1)
            {
//				printf("EDGEON %f\n", maxOn);
                edgeOn[i] = 1;
			}
			
			if (edgeOff[i] > 1)
            {
//				printf("EDGEOFF %f\n", maxOff);
                edgeOff[i] = 1;
			}
        }
    }
}

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
 * File:   Line.cpp
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 6:25 PM
 */

#include "Line.h"

#include <stdio.h>

/**
 * Constructor
 *  initializes input parameters and data structures
 *
 * @param width;                width of the input activation
 * @param height;               height of the input activation
 * @param estSexangvlatvm;      whether or not it is a hexagonal representation
 */
Line::Line(int width, int height, bool estSexangvlatvm)
{
    int i;
    // initialize blob parameters
    this->width = width;
    this->height = height;
    length = width * height;
    this->estSexangvlatvm = estSexangvlatvm;

    // allocate memory for data structures
    lines = new float [length];

    linesX = new float [length];
    linesY = new float [length];
    linesZ = new float [length];
    // initialize data structures
    for (i = 0; i < length; i++)
    {
        lines[i] = 0;

        linesX[i] = 0;
        linesY[i] = 0;
        linesZ[i] = 0;
    }
}

/**
 * Destructor
 */
Line::~Line()
{
    delete[] lines;

    delete[] linesX;
    delete[] linesY;
    delete[] linesZ;
}

/**
 * Processes edge activation and exrtacts features, e.g. lines in a specific direction
 *
 * @param edgeOn;           ON edge activation
 * @param edgeOff;          OFF edge activation
 */
void Line::process(float* edgeOn, float* edgeOff)
{
    int i;
    float negX, negY, negZ, x, y, z, center;
    float maxX = 0, maxY = 0, maxZ = 0;

    if ( estSexangvlatvm )
    {
        bool indent = false;

        // TODO: resolve segmentation faults more intelligently
        // for each local edge detector
        #pragma omp parallel for private(i, indent, negX, negY, negZ, x, y, z, center)
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
                //linesX[i] = edgeOn[i] + edgeOff[i-width] + edgeOff[i+width+1];
                //linesY[i] = edgeOn[i] + edgeOff[i-1] + edgeOff[i+1];
                //linesZ[i] = edgeOn[i] + edgeOff[i+width] + edgeOff[i+width+1];

                negX = edgeOff[i-width];
                negY = edgeOff[i-1];
                negZ = edgeOff[i+width];
                center = edgeOn[i];
                x = edgeOff[i+width+1];
                y = edgeOff[i+1];
                z = edgeOff[i-width+1];

                linesX[i] = center - (negX + x)/2 + (negY + y + negZ + z) / 4;
                linesY[i] = center - (negY + y)/2 + (negX + x + negZ + z) / 4;
                linesZ[i] = center - (negZ + z)/2 + (negX + x + negY + y) / 4;

                //linesX[i] = (center + negY + y + negZ + z) / 5 - (negX + x) / 2;
                //linesY[i] = (center + negX + x + negZ + z) / 5 - (negY + y) / 2;
                //linesZ[i] = (center + negX + x + negY + y) / 5 - (negZ + z) / 2;
            }
            else
            {
                //linesX[i] = edgeOn[i] + edgeOff[i-width-1] + edgeOff[i+width];
                //linesY[i] = edgeOn[i] + edgeOff[i-1] + edgeOff[i+1];
                //linesZ[i] = edgeOn[i] + edgeOff[i+width-1] + edgeOff[i+width];

                negX = edgeOff[i-width-1];
                negY = edgeOff[i-1];
                negZ = edgeOff[i+width-1];
                center = edgeOn[i];
                x = edgeOff[i+width];
                y = edgeOff[i+1];
                z = edgeOff[i-width];

                linesX[i] = center - (negX + x)/2 + (negY + y + negZ + z) / 4;
                linesY[i] = center - (negY + y)/2 + (negX + x + negZ + z) / 4;
                linesZ[i] = center - (negZ + z)/2 + (negX + x + negY + y) / 4;

                //linesX[i] = (center + negY + y + negZ + z) / 5 - (negX + x) / 2;
                //linesY[i] = (center + negX + x + negZ + z) / 5 - (negY + y) / 2;
                //linesZ[i] = (center + negX + x + negY + y) / 5 - (negZ + z) / 2;
            }

            // devide over the number of input, so [0-1]
            //linesX[i] = linesX[i] / 3;
            //linesY[i] = linesY[i] / 3;
            //linesZ[i] = linesZ[i] / 3;
            
            /*
            if (linesX[i] > linesY[i] && linesX[i] > linesZ[i])
            {
                linesY[i] = 0;
                linesZ[i] = 0;
            }
            else if (linesY[i] > linesZ[i])
            {
                linesX[i] = 0;
                linesZ[i] = 0;
            }
            else 
            {
                linesX[i] = 0;
                linesY[i] = 0;
            }
            */

            if ( linesX[i] < 0 )
            {
                linesX[i] = 0;
            }
            if ( linesY[i] < 0 )
            {
                linesY[i] = 0;
            }
            if ( linesZ[i] < 0 )
            {
                linesZ[i] = 0;
            }

            if ( linesX[i] > maxX )
            {
                maxX = linesX[i];
            }
            if ( linesY[i] > maxY )
            {
                maxY = linesY[i];
            }
            if ( linesZ[i] > maxY )
            {
                maxZ = linesZ[i];
            }
        }
    }
    else // if ( estQvadriangvlatvm )
    {
        #pragma omp parallel for private(i, negX, negY, center, x, y)
        for (i = 0; i < length; i++)
        {
            //linesX[i] = edgeOn[i] + edgeOff[i-1] + edgeOff[i+1];
            //linesY[i] = edgeOn[i] + edgeOff[i-width] + edgeOff[i+width];

            negX = edgeOff[i-width];
            negY = edgeOff[i-1];
            center = edgeOn[i];
            x = edgeOff[i+width];
            y = edgeOff[i+1];

            linesX[i] = center - (negX + x) + (negY + y) / 2;
            linesY[i] = center - (negY + y) + (negX + x) / 2;
           
            //linesX[i] = linesX[i] / 3;
            //linesY[i] = linesY[i] / 3;

            if ( linesX[i] < 0 )
            {
                linesX[i] = 0;
            }
            if ( linesY[i] < 0 )
            {
                linesY[i] = 0;
            }

            if ( linesX[i] > maxX )
            {
                maxX = linesX[i];
            }
            if ( linesY[i] > maxY )
            {
                maxY = linesY[i];
            }
        }
    }

    // NO NORMALIZATION
    // factorize instead
    // temp variable
    float LINEFACTORIZATION = 5;
    
    bool normalize = false;
    // normalize
    if (normalize)
    {
        if ( estSexangvlatvm )
        {
            #pragma omp parallel for private(i)
            for (i = 0; i < length; i++)
            {
                //linesX[i] = linesX[i] / maxX;
                //linesY[i] = linesY[i] / maxY;
                //linesZ[i] = linesZ[i] / maxZ;
                
                linesX[i] = linesX[i] * LINEFACTORIZATION;
                linesY[i] = linesY[i] * LINEFACTORIZATION;
                linesZ[i] = linesZ[i] * LINEFACTORIZATION;
                
                if (linesX[i] > 1)
                {
					printf("LINESX %f\n", maxX);
					linesX[i] = 1;
				}
				if (linesY[i] > 1)
                {
					printf("LINESY %f\n", maxY);
					linesY[i] = 1;
				}
				if (linesZ[i] > 1)
                {
					printf("LINESZ %f\n", maxZ);
					linesZ[i] = 1;
				}

            }
        }
        else // if ( estQvadriangvlatvm )
        {
            #pragma omp parallel for private(i)
            for (i = 0; i < length; i++)
            {
                //linesX[i] = linesX[i] / maxX;
                //linesY[i] = linesY[i] / maxY;
                
				linesX[i] = linesX[i] / LINEFACTORIZATION;
                linesY[i] = linesY[i] / LINEFACTORIZATION;
                
				if (linesX[i] > 1)
                {
//					printf("LINESX %f\n", maxX);
					linesX[i] = 1;
				}
				if (linesY[i] > 1)
                {
//					printf("LINESY %f\n", maxZ);
					linesY[i] = 1;
				}
            }
        }
    }
}

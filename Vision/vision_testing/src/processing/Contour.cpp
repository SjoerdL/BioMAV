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
 * File:   Contour.cpp
 * Author: jelangevoort
 * 
 * Created on August 14, 2011, 6:25 PM
 */

#include "Contour.h"
#include <stdio.h>
#include <math.h>

/**
 * Constructor
 *  initializes input parameters and data structures
 *
 * @param width;                width of the input activation
 * @param height;               height of the input activation
 * @param estSexangvlatvm;      whether or not it is a hexagonal representation
 */
Contour::Contour(int width, int height, bool estSexangvlatvm)
{
    int i, j;

    // initialize parameters
    this->width = width;
    this->height = height;
    length = width * height;
    this->estSexangvlatvm = estSexangvlatvm;

    // allocate memory for contour data
    contours = new float [length];
    anglesX = new float [12];
    anglesY = new float [12];
    distances = new float* [length];

    houghLength = 12*sqrt(width*width+height*height);
    hough = new float [houghLength];

    // initialize contours
    for (i = 0; i < length; i++)
    {
        contours[i] = 0;
        distances[i] = new float [12];

        for (j = 0; j < 12; j++)
        {
            distances[i][j] = 0;
        }
    }

    for (i = 0; i < houghLength; i++)
    {
        hough[i] = 0;
    }

    // set X angles
    anglesX[0] = cos(-90);
    anglesX[1] = cos(-75);
    anglesX[2] = cos(-60);
    anglesX[3] = cos(-45);
    anglesX[4] = cos(-30);
    anglesX[5] = cos(-15);
    anglesX[6] = cos(0);
    anglesX[7] = cos(15);
    anglesX[8] = cos(30);
    anglesX[9] = cos(45);
    anglesX[10] = cos(60);
    anglesX[11] = cos(75);
    // set Y angles
    anglesY[0] = sin(-90);
    anglesY[1] = sin(-75);
    anglesY[2] = sin(-60);
    anglesY[3] = sin(-45);
    anglesY[4] = sin(-30);
    anglesY[5] = sin(-15);
    anglesY[6] = sin(0);
    anglesY[7] = sin(15);
    anglesY[8] = sin(30);
    anglesY[9] = sin(45);
    anglesY[10] = sin(60);
    anglesY[11] = sin(75);
}

/**
 * Destructor
 */
Contour::~Contour()
{
    delete[] contours;
}

/**
 * Process contour information by combining the different activation of the edge and EMD activation
 *
 * @param linesX;           lines on the X axis 
 * @param linesY;           lines on the Y axis
 * @param linesZ;           lines on the Z axis (empty if qvadriangvlvs)
 * @param emdNegX;          emd activation direction negative X
 * @param emdNegY;          emd activation direction negative Y
 * @param emdNegZ;          emd activation direction negative Z (empty if qvadriangvlvs)
 * @param emdX;             emd activation direction X
 * @param emdY;             emd activation direction Y
 * @param emdZ;             emd activation direction Z (empty if qvadriangvlvs)
 */
void Contour::process(float* linesX, float* linesY, float* linesZ, float* emdNegX, float* emdNegY, float* emdNegZ, float* emdX, float* emdY, float* emdZ)
{
    int i;
    float max = 0;

    if ( estSexangvlatvm )
    {
        bool indent = false;

        // TODO: resolve segmentation faults more intelligently
        #pragma omp parallel for private (i, indent)
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

            // process motion filters
            if (indent)
            {
                // TODO implement better fit
                //contours[i] = (linesX[i] * 0.15 + linesX[i] * emdNegX[i+width+1] + linesX[i] * emdX[i-width] + emdNegX[i+width+1] * 0.15 + emdX[i-width] * 0.15) +
                //              (linesY[i] * 0.15 + linesY[i] * emdNegY[i+1] + linesY[i] * emdY[i-1] + emdNegY[i+1] * 0.15 + emdY[i-1] * 0.15) +
                //              (linesZ[i] * 0.15 + linesZ[i] * emdNegZ[i-width+1] + linesZ[i] * emdZ[i+width] + emdNegZ[i-width+1] * 0.15 + emdZ[i+width] * 0.15);
                
                contours[i] = (linesX[i] * 0.5 + linesX[i] * emdNegX[i+width+1] + linesX[i] * emdX[i-width]) +
                              (linesY[i] * 0.5 + linesY[i] * emdNegY[i+1] + linesY[i] * emdY[i-1]) +
                              (linesZ[i] * 0.5 + linesZ[i] * emdNegZ[i-width+1] + linesZ[i] * emdZ[i+width]);
                
//				contours[i] = (linesY[i]+linesZ[i]) * (emdNegX[i+width+1] + emdX[i-width]) +
//                              (linesX[i]+linesZ[i]) * (emdNegY[i+1] + emdNegY[i] + emdY[i-1]) +
//                              (linesX[i]+linesY[i]) * (emdNegZ[i-width+1] + emdZ[i+width]);
            }
            else
            {
                // TODO implement better fit
                //contours[i] = (linesX[i] * 0.15 + linesX[i] * emdNegX[i+width] + linesX[i] * emdX[i-width-1] + emdNegX[i+width] * 0.15 + emdX[i-width-1] * 0.15) +
                //              (linesY[i] * 0.15 + linesY[i] * emdNegY[i+1] + linesY[i] * emdY[i-1] + emdNegY[i+1] * 0.15 + emdY[i-1] * 0.15) +
                //              (linesZ[i] * 0.15 + linesZ[i] * emdNegZ[i-width] + linesZ[i] * emdZ[i+width-1] + emdNegZ[i-width] * 0.15 + emdZ[i+width-1] * 0.15);
                
                contours[i] = ((linesY[i]+linesZ[i]) * 0.25 + linesX[i] * emdNegX[i+width] + linesX[i] * emdX[i-width-1]) +
                              ((linesX[i]+linesZ[i]) * 0.25 + linesY[i] * emdNegY[i+1] + linesY[i] * emdY[i-1]) +
                              ((linesX[i]+linesY[i]) * 0.25 + linesZ[i] * emdNegZ[i-width] + linesZ[i] * emdZ[i+width-1]);
                              
//				contours[i] = (linesY[i]+linesZ[i]) * (emdNegX[i+width] + emdX[i-width-1]) +
//                              (linesX[i]+linesZ[i]) * (emdNegY[i+1] +  emdY[i-1]) +
//                              (linesX[i]+linesY[i]) * (emdNegZ[i-width] + emdZ[i+width-1]);
            }
            
            
            if ((i%width) > (width-3))
            {
                contours[i] = 0;
            }
            
            if ((i%width) < 2)
            {
                contours[i] = 0;
            }

            if ( contours[i] > max )
            {
                max = contours[i];
            }
        }
    }
    else // if ( estQvadriangvlatvm )
    {
        // TODO: resolve segmentation faults more intelligently
        #pragma omp parallel for private(i)
        for (i = 0; i < length; i++)
        {
            // TODO implement better fit
            contours[i] = (linesX[i] * 0.15 + linesX[i] * emdNegX[i+width] + linesX[i] * emdX[i-width] + emdNegX[i+width] * 0.15 + emdX[i-width] * 0.15) +
                          (linesY[i] * 0.15 + linesY[i] * emdNegY[i+1] + linesY[i] * emdY[i-1] + emdNegY[i+1] * 0.15 + emdY[i-1] * 0.15);

            if ( contours[i] > max )
            {
                max = contours[i];
            }
        }
    }
    
    // NO NORMALIZATION
    // factorize instead
    // temp variable
    float CONTOURFACTORIZATION = 0.5;
    
	bool normalize = true;
    // normalize
    if (normalize)
    {
        #pragma omp parallel for private(i)
        for (i = 0; i < length; i++)
        {
            //contours[i] = contours[i] / max;
            contours[i] = contours[i] * CONTOURFACTORIZATION;
            
            if (contours[i] > 1)
            {
//				printf("CONTOURS %f\n", max);
                contours[i] = 1;
			}
        }
    }
    
    /*
    float houghMax =0;
    // set hough datastructure to 0
	for (i = 0; i < houghLength; i++)
    {
        hough[i] = 0;
    }
    
    int j;
    if ( estSexangvlatvm )
    {
        int x, y;
        #pragma omp parallel for private (i, j, x, y)
        for (i = 0; i < length; i++)
        {
            if (contours[i] > 0.05)
            {
				// when the end of a line (width) is reached
				if (isEven(i/width))
				{
					// NO INDENT
					x = (int) i % width;
					y = (int) i / width;
				}
				else
				{
					// width is 1, since it uses indices to calculate
					x = (int) i % width + 0.5;
					y = (int) i / width;
				}
				
                // fill hough space
                for (j = 0; j < 12; j++)
                {
					distances[i][j] = x * anglesX[j] + y * anglesY[j];
					
                    if (distances[i][j] > 0)
                    {
                        hough[12*(int)distances[i][j]+j]+=1;
					}
					
                    if (hough[12*(int)distances[i][j]+j] > houghMax)
                    {
                        houghMax = hough[12*(int)distances[i][j]+j];
					}
                }
            }
            else
            {
				distances[i][0] = 0;
				distances[i][1] = 0;
				distances[i][2] = 0;
				distances[i][3] = 0;
				distances[i][4] = 0;
				distances[i][5] = 0;
				distances[i][6] = 0;
				distances[i][7] = 0;
				distances[i][8] = 0;
				distances[i][9] = 0;
				distances[i][10] = 0;
				distances[i][11] = 0;
			}
		}
    }
    
    normalize = true;
    // normalize
    if (normalize)
    {
        #pragma omp parallel for private(i)
        for (i = 0; i < houghLength; i++)
        {
            hough[i] = hough[i] / houghMax;
        }
    }
    */
}

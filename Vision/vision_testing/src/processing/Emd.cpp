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
 * File:   Emd.cpp
 * Author: jelangevoort
 * 
 * Created on August 14, 2011, 6:24 PM
 */

#include "Emd.h"

#include <stdio.h>
#include <math.h>

/**
 * Constructor
 *
 * @param width
 * @param height
 * @param tHigh
 * @param tLow
 * @param estSexangvlatvm
 * @param activation
 */
Emd::Emd(int width, int height, float tHigh, float tLow, bool estSexangvlatvm)
{
    int i;
    // initialize EMD parameters
    this->width = width;
    this->height = height;
    length = width * height;
    this->tHigh = tHigh;
    this->tLow = tLow;
    this->estSexangvlatvm = estSexangvlatvm;

    // allocate memory for data structures
    //  pre-processing
    highPass = new float [length];
    passed = new float [length];
    lowPass = new float [length];
    //  output cells (specific directions)
    motionNegX = new float [length];
    motionNegY = new float [length];
    motionNegZ = new float [length];
    motionX = new float [length];
    motionY = new float [length];
    motionZ = new float [length];
    //  combination
    motionOrientation = new float [length];
    motionStrength = new float [length];

	// TEST
	highPassOff = new float [length];
    passedOff = new float [length];
    lowPassOff = new float [length];
    
    // initialize data structures
    for (i = 0; i < length; i++)
    {
        // pre-processing
        highPass[i] = 0;
        passed[i] = 0;
        lowPass[i] = 0;
        // output cells (specific directions)
        motionNegX[i] = 0;
        motionNegY[i] = 0;
        motionNegZ[i] = 0;
        motionX[i] = 0;
        motionY[i] = 0;
        motionZ[i] = 0;
        // combination
        motionOrientation[i] = 0;
        motionStrength[i] = 0;
        
        // TEST //
		highPassOff[i] = 0;
        passedOff[i] = 0;
        lowPassOff[i] = 0;
    }
}

/**
 * Destructor
 */
Emd::~Emd()
{
    // pre-processing
    delete[] highPass;
    delete[] passed;
    delete[] lowPass;
    // output cells (specific directions)
    delete[] motionNegX;
    delete[] motionNegY;
    delete[] motionNegZ;
    delete[] motionX;
    delete[] motionY;
    delete[] motionZ;
    // combination
    delete[] motionOrientation;
    delete[] motionStrength;
    
    // TEST //
    delete[] highPassOff;
    delete[] passedOff;
    delete[] lowPassOff;
}


/**
 * 
 * @param i
 * @return 
 */
bool isEven(int i)
{
    if ((i%2) == 1)
        return false;
    else return true;
}

/**
 * Passes the filters and calculates the motion filters and orientation
 *
 * @param *image Receives the image file (1 dimensional)
 */
void Emd::process(float* activation)
{
    int i;
    float act;
    
    #pragma omp parallel for private(i, act)
    for (i = 0; i < length; i++)
    {
        // log-transform the input ** LEGACY **
        act = activation[i];//log(activation[i]);
        //if(act < -10)
        //    act = -10;
        // update the HighPass filter
        highPass[i] = act / tHigh + (1 - 1/tHigh) * highPass[i];
        // determine the part of the signal that passes the filter:
        passed[i] = act - highPass[i];
        // update the LowPass filter:
        lowPass[i] = passed[i] / tLow + (1 - 1/tLow) * lowPass[i];
        
        // TEST //
		// log-transform the input ** LEGACY **
        act = 1-activation[i];//log(1-activation[i]);
        //if(act < -10)
        //    act = -10;
        // update the HighPass filter
        highPassOff[i] = act / tHigh + (1 - 1/tHigh) * highPassOff[i];
        // determine the part of the signal that passes the filter:
        passedOff[i] = act - highPassOff[i];
        // update the LowPass filter:
        lowPassOff[i] = passedOff[i] / tLow + (1 - 1/tLow) * lowPassOff[i];
    }

    // process motion filters, determine orientation and strength
    //float max = 0;
    float maxNegX = 0, maxNegY = 0, maxNegZ = 0, maxX = 0, maxY = 0, maxZ = 0;
    if( estSexangvlatvm )
    {
        bool indent = false;
        
        // TODO: resolve segmentation faults more intelligently
        #pragma omp parallel for private(i, indent)
        for(i = width+1; i < length-width-1; i++)
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
				// ON CHANNEL
                motionNegX[i] 	= (passed[i-width] 		* lowPass[i]) - (passed[i] * lowPass[i-width]);
                motionNegY[i] 	= (passed[i-1] 			* lowPass[i]) - (passed[i] * lowPass[i-1]);
                motionNegZ[i] 	= (passed[i+width] 		* lowPass[i]) - (passed[i] * lowPass[i+width]);
                motionX[i] 		= (passed[i+width+1] 	* lowPass[i]) - (passed[i] * lowPass[i+width+1]);
                motionY[i] 		= (passed[i+1] 			* lowPass[i]) - (passed[i] * lowPass[i+1]);
                motionZ[i] 		= (passed[i-width+1] 	* lowPass[i]) - (passed[i] * lowPass[i-width+1]);
                // OFF CHANNEL
				motionNegX[i] 	+= (passedOff[i-width] 		* lowPassOff[i]) - (passedOff[i] * lowPassOff[i-width]);
                motionNegY[i] 	+= (passedOff[i-1] 			* lowPassOff[i]) - (passedOff[i] * lowPassOff[i-1]);
                motionNegZ[i] 	+= (passedOff[i+width] 		* lowPassOff[i]) - (passedOff[i] * lowPassOff[i+width]);
                motionX[i] 		+= (passedOff[i+width+1]	* lowPassOff[i]) - (passedOff[i] * lowPassOff[i+width+1]);
                motionY[i] 		+= (passedOff[i+1] 			* lowPassOff[i]) - (passedOff[i] * lowPassOff[i+1]);
                motionZ[i] 		+= (passedOff[i-width+1] 	* lowPassOff[i]) - (passedOff[i] * lowPassOff[i-width+1]);
            }
            else
            {
				// ON CHANNEL
                motionNegX[i] 	= (passed[i-width-1] 	* lowPass[i]) - (passed[i] * lowPass[i-width-1]);
                motionNegY[i] 	= (passed[i-1] 			* lowPass[i]) - (passed[i] * lowPass[i-1]);
                motionNegZ[i] 	= (passed[i+width-1] 	* lowPass[i]) - (passed[i] * lowPass[i+width-1]);
                motionX[i] 		= (passed[i+width] 		* lowPass[i]) - (passed[i] * lowPass[i+width]);
                motionY[i] 		= (passed[i+1] 			* lowPass[i]) - (passed[i] * lowPass[i+1]);
                motionZ[i] 		= (passed[i-width] 		* lowPass[i]) - (passed[i] * lowPass[i-width]);
				// OFF CHANNEL
                motionNegX[i] 	+= (passedOff[i-width-1] 	* lowPassOff[i]) - (passedOff[i] * lowPassOff[i-width-1]);
                motionNegY[i] 	+= (passedOff[i-1] 			* lowPassOff[i]) - (passedOff[i] * lowPassOff[i-1]);
                motionNegZ[i] 	+= (passedOff[i+width-1] 	* lowPassOff[i]) - (passedOff[i] * lowPassOff[i+width-1]);
                motionX[i] 		+= (passedOff[i+width] 		* lowPassOff[i]) - (passedOff[i] * lowPassOff[i+width]);
                motionY[i] 		+= (passedOff[i+1] 			* lowPassOff[i]) - (passedOff[i] * lowPassOff[i+1]);
                motionZ[i] 		+= (passedOff[i-width] 		* lowPassOff[i]) - (passedOff[i] * lowPassOff[i-width]);
            }

            if (motionNegX[i] < 0)
            {
                motionNegX[i] = 0;
            }
            if (motionNegY[i] < 0)
            {
                motionNegY[i] = 0;
            }
            if (motionNegZ[i] < 0)
            {
                motionNegZ[i] = 0;
            }
            if (motionX[i] < 0)
            {
                motionX[i] = 0;
            }
            if (motionY[i] < 0)
            {
                motionY[i] = 0;
            }
            if (motionZ[i] < 0)
            {
                motionZ[i] = 0;
            }
            
			if(i%width < 1 || i%width > width-2)
			{
                motionNegX[i] = 0;
                motionNegY[i] = 0;
                motionNegZ[i] = 0;
                motionX[i] = 0;
                motionY[i] = 0;
                motionZ[i] = 0;
			}
            
            if (motionNegX[i] > maxNegX)
            {
                maxNegX = motionNegX[i];
            }
            if (motionNegY[i] > maxNegY)
            {
                maxNegY = motionNegY[i];
            }
            if (motionNegZ[i] > maxNegZ)
            {
                maxNegZ = motionNegZ[i];
            }
            if (motionX[i] > maxX)
            {
                maxX = motionX[i];
            }
            if (motionY[i] > maxY)
            {
                maxY = motionY[i];
            }
            if (motionZ[i] > maxZ)
            {
                maxZ = motionZ[i];
            }

            //motionY[i] = motionY[i] * 100;
            //if (motionY[i] > 1 )
            //    motionY[i] = 1;
            
            // TODO: BOTH DO NOT WORK AS INTENDED, JUST USE AS VISUALIZATION!
            // determine orientation and strength
            //motionStrength[i]=mN[i]*mS[i]+mE[i]*mW[i];

            //motionStrength[i]=motionNegX[i]*motionX[i]+motionNegY[i]*motionY[i]+motionNegZ[i]*motionZ[i];
            
            //if (motionStrength[i] > max)
            //{
            //    max = motionStrength[i];
            //}

            //motionOrientation[i] = (motionNegX[i]+motionX[i]+motionNegY[i]+motionY[i]+motionNegY[i]+motionY[i]);
        } 
    }
    else // if ( estQuadriangvlatvm )
    {
        // TODO: resolve segmentation faults more intelligently
        for(i = width; i < length-width; i++)
        {
            // process motion filters
            motionNegX[i] = (passed[i-1] * lowPass[i]) - (passed[i] * lowPass[i-1]);
            motionNegY[i] = (passed[i-width] * lowPass[i]) - (passed[i] * lowPass[i-width]);
            motionX[i] = (passed[i+1] * lowPass[i]) - (passed[i] * lowPass[i+1]);
            motionY[i] = (passed[i+width] * lowPass[i]) - (passed[i] * lowPass[i+width]);

            if (motionNegX[i] < 0)
            {
                motionNegX[i] = 0;
            }
            if (motionNegY[i] < 0)
            {
                motionNegY[i] = 0;
            }
            if (motionX[i] < 0)
            {
                motionX[i] = 0;
            }
            if (motionY[i] < 0)
            {
                motionY[i] = 0;
            }
            // TODO: BOTH DO NOT WORK AS INTENDED, JUST USE AS VISUALIZATION!
            // determine orientation and strength
            //motionStrength[i]=mN[i]*mS[i]+mE[i]*mW[i];
            //motionStrength[i]=motionNegX[i]*motionX[i]+motionNegY[i]*motionY[i];

            //if (motionStrength[i] > max)
            //    max = motionStrength[i];

            //motionOrientation[i] = (motionNegX[i]+motionX[i]+motionNegY[i]+motionY[i]);
        }
    }

    // NO NORMALIZATION
    // factorize instead
    // temp variable
    float EMDFACTORIZATION = 50;
    
    bool normalize = true;
    // normalize motion strength
    if (normalize)
    {
        #pragma omp parallel for private(i)
        for (i = 0; i < length; i++)
        {
            //motionNegX[i] = motionNegX[i] / maxNegX;
            //motionNegY[i] = motionNegY[i] / maxNegY;
            //motionNegZ[i] = motionNegZ[i] / maxNegZ;
            //motionX[i] = motionX[i] / maxX;
            //motionY[i] = motionY[i] / maxY;
            //motionZ[i] = motionZ[i] / maxZ;
            
			motionNegX[i] = motionNegX[i] * EMDFACTORIZATION;
            motionNegY[i] = motionNegY[i] * EMDFACTORIZATION;
            motionNegZ[i] = motionNegZ[i] * EMDFACTORIZATION;
            motionX[i] = motionX[i] * EMDFACTORIZATION;
            motionY[i] = motionY[i] * EMDFACTORIZATION;
            motionZ[i] = motionZ[i] * EMDFACTORIZATION;
            
            //motionStrength[i]=(motionNegX[i]+motionX[i]+motionNegY[i]+motionY[i]+motionNegZ[i]+motionZ[i]);
            
			if (motionNegX[i] > 1)
			{
//				printf("MOTIONNEGX %f\n", maxNegX);
				motionNegX[i] = 1;
			}
			if (motionNegY[i] > 1)
			{
//				printf("MOTIONNEGY %f\n", maxNegY);
				motionNegY[i] = 1;
			}
			if (motionNegZ[i] > 1)
			{
//				printf("MOTIONNEGZ %f\n", maxNegZ);
				motionNegZ[i] = 1;
			}
			if (motionX[i] > 1)
			{
//				printf("MOTIONX %f\n", maxX);
				motionX[i] = 1;
			}
			if (motionY[i] > 1)
			{
//				printf("MOTIONY %f\n", maxY);
				motionY[i] = 1;
			}
			if (motionZ[i] > 1)
			{
//				printf("MOTIONZ %f\n", maxZ);
				motionZ[i] = 1;
			}
        }
    }

    /*
    float strengthThreshold = (float) (0.2 * max);
    for(i=0; i<length; i++)
    {
        // The orientations are all in [-pi, pi], with 0 either no motion
        // or movement to the right, 0.5*pi to the top, -0.5 * pi to the
        // bottom, and pi / -pi to the left.
        if (motionStrength[i] > strengthThreshold)
            motionOrientation[i] = atan2(mY[i],mX[i]);
        else motionOrientation[i] = 0;
    }

    double strengthThreshold = 0.2 * max;
    // The orientations are all in [-pi, pi], with 0 either no motion
    // or movement to the right, 0.5*pi to the top, -0.5 * pi to the
    // bottom, and pi / -pi to the left.
    for(j=1; j<HEIGHT; j++)
        for(i=1; i<WIDTH; i++)
            MotionOrientation[j][i]= atan2(MY[j][i],MX[j][i]) * (MotionStrength[j][i] > strengthThreshold);
    */
}

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
 * File:   LatInh.cpp
 * Author: jelangevoort
 * 
 * Created on August 13, 2011, 8:34 PM
 */

#include "LatInh.h"

/**
 * Constructor
 * 
 * @param width;                    width of the activation matrix
 * @param height;                   height of the activation matrix
 * @param estSexangvlatvm;          pixels or hexels (true if sexangvlvs)
 * @param connections;              number of photo receptors connected to each cell
 * @param number;                   number of cells
 */
LatInh::LatInh(int width, int height, bool estSexangvlatvm)//, int connections, int number)
{
    this->width = width;
    this->height = height;
    length = width * height;
    this->estSexangvlatvm = estSexangvlatvm;
    this->connections = connections;
    this->number = number;
    
    // allocate memory for inhibited activation
    inhibited = new float [width*height];
}

/**
 * Destructor
 */
LatInh::~LatInh() 
{   
    delete[] inhibited;
}

/**
 * Inhibits neighboring cells
 * 
 * @param activation;       activation prior to the lateral inhibition
 * @return rewritten activation with lateral inhibition
 */
float* LatInh::inhibit(float* activation)
{
    int i;
    float max = 0;
    float inhibit = 0;

    // initialize data structure
    for (i = 0; i < length; i++)
    {
        inhibited[i] = activation[i];
    }

    /*
    if ( estSexangvlatvm )
    {
        bool indent = false;
        int counter;

        // TODO: resolve segmentation faults more intelligently
        // for each local edge detector
        for (i = width+width+1, counter = 0; i < length-width-width-1; i++, counter++)
        {
            // when the end of a line (width) is reached
            if (counter == width)
            {
                // flip indentation
                if (indent)
                    indent = false;
                else
                    indent = true;

                // reset counter
                counter = 0;
            }

            // process neighboring data
            if (indent)
            {
                inhibit =
                    (activation[i-width-width-1] + activation[i-width-width] + activation[i-width-width+1] +
                    activation[i-width-1] + activation[i-width] + activation[i-width+1] + activation[i-width+2] +
                    activation[i-2] + activation[i-1] + activation[i] + activation[i+1] + activation[i+2] +
                    activation[i+width-1] + activation[i+width] + activation[i+width+1] + activation[i+width+2] +
                    activation[i+width+width-1] + activation[i+width+width] + activation[i+width+width+1]) / 38;

                inhibited[i-width-width-1] += inhibit;
                inhibited[i-width-width] += inhibit;
                inhibited[i-width-width+1] += inhibit;
                inhibited[i-width-1] += inhibit;
                inhibited[i-width] += inhibit;
                inhibited[i-width+1] += inhibit;
                inhibited[i-width+2] += inhibit;
                inhibited[i-2] += inhibit;
                inhibited[i-1] += inhibit;
                inhibited[i] += inhibit;
                inhibited[i+1] += inhibit;
                inhibited[i+2] += inhibit;
                inhibited[i+width-1] += inhibit;
                inhibited[i+width] += inhibit;
                inhibited[i+width+1] += inhibit;
                inhibited[i+width+2] += inhibit;
                inhibited[i+width+width-1] += inhibit;
                inhibited[i+width+width] += inhibit;
                inhibited[i+width+width+1] += inhibit;
            }
            else
            {
                inhibit =
                    (activation[i-width-width-1] + activation[i-width-width] + activation[i-width-width+1] +
                    activation[i-width-2] + activation[i-width-1] + activation[i-width] + activation[i-width+1] +
                    activation[i-2] + activation[i-1] + activation[i] + activation[i+1] + activation[i+2] +
                    activation[i+width-2] + activation[i+width-1] + activation[i+width] + activation[i+width+1] +
                    activation[i+width+width-1] + activation[i+width+width] + activation[i+width+width+1]) / 38;

                inhibited[i-width-width-1] += inhibit;
                inhibited[i-width-width] += inhibit;
                inhibited[i-width-width+1] += inhibit;
                inhibited[i-width-2] += inhibit;
                inhibited[i-width-1] += inhibit;
                inhibited[i-width] += inhibit;
                inhibited[i-width+1] += inhibit;
                inhibited[i-2] += inhibit;
                inhibited[i-1] += inhibit;
                inhibited[i] += inhibit;
                inhibited[i+1] += inhibit;
                inhibited[i+2] += inhibit;
                inhibited[i+width-2] += inhibit;
                inhibited[i+width-1] += inhibit;
                inhibited[i+width] += inhibit;
                inhibited[i+width+1] += inhibit;
                inhibited[i+width+width-1] += inhibit;
                inhibited[i+width+width] += inhibit;
                inhibited[i+width+width+1] += inhibit;
            }
        }
    }*/
    if ( estSexangvlatvm )
    {   
        bool indent = false;
        // TODO: resolve segmentation faults more intelligently
        // for each local edge detector
        
        #pragma omp parallel for private(i, indent, inhibit)
        for (i = width+width+1; i < length-width-width-1; i++)
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

            // process neighboring data
            if (indent)
            {
                inhibit =
                    (activation[i-width-width-1] + activation[i-width-width] + activation[i-width-width+1] +
                    activation[i-width-1] + activation[i-width] + activation[i-width+1] + activation[i-width+2] +
                    activation[i-2] + activation[i-1] + activation[i] + activation[i+1] + activation[i+2] +
                    activation[i+width-1] + activation[i+width] + activation[i+width+1] + activation[i+width+2] +
                    activation[i+width+width-1] + activation[i+width+width] + activation[i+width+width+1]) / 38;

                inhibited[i-width-width-1] += inhibit;
                inhibited[i-width-width] += inhibit;
                inhibited[i-width-width+1] += inhibit;
                inhibited[i-width-1] += inhibit;
                inhibited[i-width] += inhibit;
                inhibited[i-width+1] += inhibit;
                inhibited[i-width+2] += inhibit;
                inhibited[i-2] += inhibit;
                inhibited[i-1] += inhibit;
                inhibited[i] += inhibit;
                inhibited[i+1] += inhibit;
                inhibited[i+2] += inhibit;
                inhibited[i+width-1] += inhibit;
                inhibited[i+width] += inhibit;
                inhibited[i+width+1] += inhibit;
                inhibited[i+width+2] += inhibit;
                inhibited[i+width+width-1] += inhibit;
                inhibited[i+width+width] += inhibit;
                inhibited[i+width+width+1] += inhibit;
            }
            else
            {
                inhibit =
                    (activation[i-width-width-1] + activation[i-width-width] + activation[i-width-width+1] +
                    activation[i-width-2] + activation[i-width-1] + activation[i-width] + activation[i-width+1] +
                    activation[i-2] + activation[i-1] + activation[i] + activation[i+1] + activation[i+2] +
                    activation[i+width-2] + activation[i+width-1] + activation[i+width] + activation[i+width+1] +
                    activation[i+width+width-1] + activation[i+width+width] + activation[i+width+width+1]) / 38;

                inhibited[i-width-width-1] += inhibit;
                inhibited[i-width-width] += inhibit;
                inhibited[i-width-width+1] += inhibit;
                inhibited[i-width-2] += inhibit;
                inhibited[i-width-1] += inhibit;
                inhibited[i-width] += inhibit;
                inhibited[i-width+1] += inhibit;
                inhibited[i-2] += inhibit;
                inhibited[i-1] += inhibit;
                inhibited[i] += inhibit;
                inhibited[i+1] += inhibit;
                inhibited[i+2] += inhibit;
                inhibited[i+width-2] += inhibit;
                inhibited[i+width-1] += inhibit;
                inhibited[i+width] += inhibit;
                inhibited[i+width+1] += inhibit;
                inhibited[i+width+width-1] += inhibit;
                inhibited[i+width+width] += inhibit;
                inhibited[i+width+width+1] += inhibit;
            }
        }
    }
    else // if ( estQvadriangvlatvm )
    {
        for (i = 0; i < length; i++)
        {
            inhibit =
                (activation [i-width-width] +
                activation[i-1-width] + activation [i-width] + activation [i+1-width] +
                activation [i-2] + activation [i-1] + activation [i] + activation [i+1] + activation [i+2] +
                activation[i-1+width] + activation [i+width] + activation [i+1+width] +
                activation [i+width+width]) / 13;

            inhibited[i-width-width] += inhibit;
            inhibited[i-1-width] += inhibit;
            inhibited[i-width] += inhibit;
            inhibited[i+1-width] += inhibit;
            inhibited[i-2] += inhibit;
            inhibited[i-1] += inhibit;
            inhibited[i] += inhibit;
            inhibited[i+1] += inhibit;
            inhibited[i+2] += inhibit;
            inhibited[i-1+width] += inhibit;
            inhibited[i+width] += inhibit;
            inhibited[i+1+width] += inhibit;
            inhibited[i+width+width] += inhibit;
        }
    }

    // inhibit the input cells
    for (i = 0; i < length; i++)
    {
        inhibited[i] = activation[i] - inhibited[i] / 38; // 19

        // keep legal numbers
        if (inhibited[i] < 0)
        {
            inhibited[i] = 0;
        }

        // normalize
        if (inhibited[i] > max)
        {
            max = inhibited[i];
        }
    }

    // normalize
    // TODO: THIS WILL MOST LIKELY PRODUCE VERY LARGE OUTLIERS
    //for (i = 0; i < length; i++)
    //{
    //    inhibited[i] = inhibited[i] / max;
    //}

    return inhibited;
}

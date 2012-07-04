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
 * File:   Qvadri2Sex.cpp
 * Author: jlangevoort
 * 
 * Created on August 12, 2011, 9:28 AM
 */

#include "Qvadri2Sex.h"

/**
 * Constructor
 *  Initializes conversion for fast conversion process
 *
 * @param width;                width of the input image
 * @param height;               height of the input image
 * @param numberOfChannels;     number of channels in the input image
 * @param hexelDiameter;	    diameter of the hexels
 */
Qvadri2Sex::Qvadri2Sex(int width, int height, int numberOfChannels, float hexelDiameter)
{
    int i, j, k;
    
    inputNumberOfChannels = numberOfChannels;
    inputLength = width * height * numberOfChannels;
    
    // TODO: INTEGRATE INTO SOMETHING MORE INTUITIVE (CHANGES ETC.)
    //t = 1.2529;
    t = hexelDiameter;
    // calculate length and width of hexagonal array
    this->width = (int)((width-1.5*t*sqrt(3))/(sqrt(3)*t));
    this->height = (int)((height-2*t)/(1.5*t));
    length = this->width * this->height;
    area =(float) (((3*sqrt(3))/2)*(t*t));

    // allocate memory for 'standard' hexagon with circumradius t
    float* x_hex = new float [6];
    float* y_hex = new float [6];
    // initialize 'standard' hexagon (x)
    x_hex[0] = (float) (0.5 * t * sqrt(3));
    x_hex[1] = x_hex[0];
    x_hex[2] = 0;
    x_hex[3] = - x_hex[0];
    x_hex[4] = x_hex[3];
    x_hex[5] = 0;
    // initialize 'standard' hexagon (y)
    y_hex[0] = (float) (0.5 * t);
    y_hex[1] = -y_hex[0];
    y_hex[2] = - t;
    y_hex[3] = y_hex[1];
    y_hex[4] = y_hex[0];
    y_hex[5] = t;

    // allocate memory for arrays to store (x,y) of each hexel
    float* xCoordinates = new float [length];
    float* yCoordinates = new float [length];

    // allocate memory to store the number of connections to each hexel
    number = new int [length];
    sum = new int [length];
    // stores the sum of 'number'
    int total = 0;

    // allocate memory for the output
    output = new uint8_t [length*numberOfChannels];

    for (k=0; k < length; k++)
    {
        number[k]=0;
        sum[k]=0;
        
        // calculate the (x,y) of the hexel
        xCoordinates[k] = (float) (((k%this->width)*sqrt(3)*t)+0.5*sqrt(3)*t);
        yCoordinates[k] = (float) (((k/this->width)*1.5*t)+t);
        if ((k/this->width)%2!=0) // odd (add indentation)
        {
            xCoordinates[k] = xCoordinates[k] + (float)(0.5*sqrt(3)*t);
        }

        // rewrite 'prototypical' hexagon to hexagon'
        for (i = 0; i < 6 ; i++)
        {
            x_hex[i] = x_hex[i] + xCoordinates[k];
            y_hex[i] = y_hex[i] + yCoordinates[k];
        }

        // calculate what pixels overlap (i.e. between min and max)
        int x_min = (int)(x_hex[3]), x_max = (int)(x_hex[0]);
        int y_min = (int)(y_hex[2]), y_max = (int)(y_hex[5]);

        // revert to 'prototypical' hexagon
        for (i = 0; i < 6 ; i++)
        {
            x_hex[i] = x_hex[i] - xCoordinates[k];
            y_hex[i] = y_hex[i] - yCoordinates[k];
        }

        // calculate the maximum number of overlapping pixels
        for (j = y_min; j <= y_max; j++)
        {
            for (i = x_min; i <= x_max; i++)
            {
                number[k]++;
            }
        }

        sum[k] = total;
        total = total + number[k];
    }

    // stores the connections and corresponding weights of each hexel
    // i.e. {a1, a2, a3, b1, b2, c1, c2, c3, c4, d1, e1, e2 ...}
    connection = new int [total];
    weight = new float [total];

    // for each hexel calculate the connections and corresponding weights
    int index = 0;
    for (k=0; k < length; k++)
    {
        // rewrite 'prototypical' hexagon to hexagon'
        for (i = 0; i < 6 ; i++)
        {
            x_hex[i] = x_hex[i] + xCoordinates[k];
            y_hex[i] = y_hex[i] + yCoordinates[k];
        }

        // calculate what pixels overlap (i.e. between min and max)
        int x_min = (int)(x_hex[3]), x_max = (int)(x_hex[0]);
        int y_min = (int)(y_hex[2]), y_max = (int)(y_hex[5]);

        // for each pixel that (possibly) overlaps
        for (j = y_min; j <= y_max; j++)
        {
            for (i = x_min; i <= x_max; i++)
            {
                // set connection (i.e. where in the RGB array)
                connection[index]=(i+j*width) * numberOfChannels;
                // set weight (i.e. overlap with the hexel)
                weight[index]=calcOverlap(i, j, x_hex, y_hex);
                // increase 'global' counter by one
                index++;
            }
        }

        // revert to 'prototypical' hexagon
        for (i = 0; i < 6 ; i++)
        {
            x_hex[i] = x_hex[i] - xCoordinates[k];
            y_hex[i] = y_hex[i] - yCoordinates[k];
        }
    }

    // clear memory of unused data structure
    delete[] x_hex;
    delete[] y_hex;
    delete[] xCoordinates;
    delete[] yCoordinates;
}

/**
 * Destructor
 */
Qvadri2Sex::~Qvadri2Sex()
{
    delete[] number;
    delete[] connection;
    delete[] weight;
    delete[] output;
}

/**
 * Calculates the Area of a Polygon
 *
 * @param x;    all the points on the x-axis
 * @param y;    all the points on the y-axis
 */
float Qvadri2Sex::calcPolygonArea (float* x, float* y, int length)
{
    int i;
    float area;

    // caclate the area
    //  sum of x[i] * (y[i+1]-y[i-1])
    area = x[0] * (y[1]-y[length-1]);
    for (i=1; i<length; i++)
    {
        area += x[i] * (y[i+1]-y[i-1]);
    }
    area = area/2;

    // if the area is negative, flip the sign
    if (area < 0)
        area = area * -1;

    return area;
}

/**
 * Calculate the intersection between an edge and some x value
 *
 * @param x;    point on the x-axis
 * @param x1;   starting point of the line
 * @param y1;
 * @param x2;   ending point point of the line
 * @param y2;
 * @return y-coordinate of the intersection
 */
float Qvadri2Sex::calcIntersectionX(int x, float x1, float y1, float x2, float y2)
{
    // if they are on the same height, return it
    if (y2-y1 == 0)
    {
        return y1;
    }

    // if y2 is higher than y1
    if (y2 > y1)
    {
        if (x2 > x1)
        {
            return y1 + ((x-x1) * ((y2-y1) / (x2-x1)));
        }
        else
        {
            return y2 - ((x-x2) * ((y2-y1) / (x1-x2)));
        }
    }
    // if y2 is lower than y1
    else
    {
        if (x2 > x1)
        {
            return y1 - ((x-x1) * ((y1-y2) / (x2-x1)));
        }
        else
        {
            return y2 + ((x-x2) * ((y1-y2) / (x1-x2)));
        }
    }
}

/**
 * Calculate the intersection between an edge and some y value
 *
 * @param y;    point on the y-axis
 * @param x1;   starting point of the line
 * @param y1;
 * @param x2;   ending point point of the line
 * @param y2;
 * @return x-coordinate of the intersection
 */
float Qvadri2Sex::calcIntersectionY(int y, float x1, float y1, float x2, float y2)
{
    // if they are on the same height, return it
    if (x2-x1 == 0)
    {
        return x1;
    }

    // if x2 is higher than x1
    if (x2 > x1)
    {
        if (y2 > y1)
        {
            return x1 + ((y-y1) * ((x2-x1) / (y2-y1)));
        }
        else
        {
            return x2 - ((y-y2) * ((x2-x1) / (y1-y2)));
        }
    }
    // if x2 is lower than x1
    else
    {
        if (y2 > y1)
        {
           return x1 - ((y-y1) * ((x1-x2) / (y2-y1)));
        }
        else
        {
            return x2 + ((y-y2) * ((x1-x2) / (y1-y2)));
        }
    }
}

/**
 * Sutherland-Hodgman Algorithm
 *
 * @param x;        coordinates of the pixels
 * @param y;
 * @param x_hex;    coordinates of the six corners of the hexagon
 * @param y_hex;
 * @return Area of overlap between hexel(x,y) and the pixels
 */
float Qvadri2Sex::calcOverlap(int x, int y, float* x_hex, float* y_hex)
{
    int i, length, index;

    // allocate temporary arrays to store edge information
    float * overlap_x = new float [10];
    float * overlap_y = new float [10];
    float * new_x = new float [10];
    float * new_y = new float [10];

    // initialize data structures
    for (i = 0; i < 10; i++)
    {
        overlap_x[i] = 0;
        overlap_y[i] = 0;
        new_x[i] = 0;
        new_y[i] = 0;
    }

    // set overlap to complete hexagon
    for (i = 0; i < 6; i++)
    {
        overlap_x[i] = x_hex[i];
        overlap_y[i] = y_hex[i];
    }
    overlap_x[6] = overlap_x[0];
    overlap_y[6] = overlap_y[0];

    //??
    length = 6;
    index = 0;
    
    // for each of the 4 clipping edges

    // for each of the edges of the polygon
    for (i = 0; i < length; i++)
    {
        if (overlap_x[i] >= x)
        {
            if (overlap_x[i+1] >= x)
            {
                new_x[index] = overlap_x[i+1];
                new_y[index] = overlap_y[i+1];
                index++;
            }
            else
            {
                new_x[index] = (float)x;
                new_y[index] = calcIntersectionX(x, overlap_x[i], overlap_y[i], overlap_x[i+1], overlap_y[i+1]);
                index++;
            }
        }
        else
        {
            if (overlap_x[i+1] >= x)
            {
                new_x[index] = (float)x;
                new_y[index] = calcIntersectionX(x, overlap_x[i], overlap_y[i], overlap_x[i+1], overlap_y[i+1]);
                index++;

                new_x[index] = overlap_x[i+1];
                new_y[index] = overlap_y[i+1];
                index++;
            }
        }
    }
    // if there was no overlap, return 0
    //  failsafe
    if (index == 0)
    {
        return 0;
    }

    //set overlap to newly constructed polygon
    for (i = 0; i < index; i++)
    {
        overlap_x[i] = new_x[i];
        overlap_y[i] = new_y[i];
    }
    overlap_x[index] = overlap_x[0];
    overlap_y[index] = overlap_y[0];

    // ??
    length = index;
    index = 0;

    // for each of the 4 clipping edges
    // for each of the edges of the polygon
    for (i = 0; i < length; i++)
    {
        if (overlap_x[i] < x+1)
        {
            if (overlap_x[i+1] < x+1)
            {
                new_x[index] = overlap_x[i+1];
                new_y[index] = overlap_y[i+1];
                index++;
            }
            else
            {
                new_x[index] = (float) x+1;
                new_y[index] = calcIntersectionX(x+1, overlap_x[i], overlap_y[i], overlap_x[i+1], overlap_y[i+1]);
                index++;
            }
        }
        else
        {
            if (overlap_x[i+1] < x+1)
            {
                new_x[index] = (float)x+1;
                new_y[index] = calcIntersectionX(x+1, overlap_x[i], overlap_y[i], overlap_x[i+1], overlap_y[i+1]);
                index++;

                new_x[index] = overlap_x[i+1];
                new_y[index] = overlap_y[i+1];
                index++;
            }
        }
    }
    // if there was no overlap, return 0
    //  failsafe
    if (index == 0)
    return 0;

    //set overlap to newly constructed polygon
    for (i = 0; i < index; i++)
    {
        overlap_x[i] = new_x[i];
        overlap_y[i] = new_y[i];
    }
    overlap_x[index] = overlap_x[0];
    overlap_y[index] = overlap_y[0];

    // ??
    length = index;
    index = 0;

    // for each of the 4 clipping edges
    // for each of the edges of the polygon
    for (i = 0; i < length; i++)
    {
        if (overlap_y[i] >= y)
        {
            if (overlap_y[i+1] >= y)
            {
                new_x[index] = overlap_x[i+1];
                new_y[index] = overlap_y[i+1];
                index++;
            }
            else
            {
                new_x[index] = calcIntersectionY(y, overlap_x[i], overlap_y[i], overlap_x[i+1], overlap_y[i+1]);
                new_y[index] = (float)y;
                index++;
            }
        }
        else
        {
            if (overlap_y[i+1] >= y)
            {
                new_x[index] = calcIntersectionY(y, overlap_x[i], overlap_y[i], overlap_x[i+1], overlap_y[i+1]);
                new_y[index] = (float)y;
                index++;

                new_x[index] = overlap_x[i+1];
                new_y[index] = overlap_y[i+1];
                index++;
            }
        }
    }
    // if there was no overlap, return 0
    //  failsafe
    if (index == 0)
    return 0;

    //set overlap to newly constructed polygon
    for (i = 0; i < index; i++)
    {
        overlap_x[i] = new_x[i];
        overlap_y[i] = new_y[i];
    }
    overlap_x[index] = overlap_x[0];
    overlap_y[index] = overlap_y[0];

    // ??
    length = index;
    index = 0;

    // for each of the 4 clipping edges
    // for each of the edges of the polygon
    for (i = 0; i < length; i++)
    {
        if (overlap_y[i] < y+1)
        {
            if (overlap_y[i+1] < y+1)
            {
                new_x[index] = overlap_x[i+1];
                new_y[index] = overlap_y[i+1];
                index++;
            }
            else
            {
                new_x[index] = calcIntersectionY(y+1, overlap_x[i], overlap_y[i], overlap_x[i+1], overlap_y[i+1]);
                new_y[index] = (float)y+1;
                index++;
            }
        }
        else
        {
            if (overlap_y[i+1] < y+1)
            {
                new_x[index] = calcIntersectionY(y+1, overlap_x[i], overlap_y[i], overlap_x[i+1], overlap_y[i+1]);
                new_y[index] = (float)y+1;
                index++;

                new_x[index] = overlap_x[i+1];
                new_y[index] = overlap_y[i+1];
                index++;
            }
        }
    }
    // if there was no overlap, return 0
    //  failsafe
    if (index == 0)
    return 0;

    //set overlap to newly constructed polygon
    for (i = 0; i < index; i++)
    {
        overlap_x[i] = new_x[i];
        overlap_y[i] = new_y[i];
    }
    overlap_x[index] = overlap_x[0];
    overlap_y[index] = overlap_y[0];

    // ??
    length = index;
    index = 0;

    // caculate the overlap by calculating the area of the overlapping polygon
    //  devided by size of the 'standard' hexagon
    float overlap = calcPolygonArea(overlap_x, overlap_y, length)/area;

    // clear memory of unused data structures
    delete[] overlap_x;
    delete[] overlap_y;
    delete[] new_x;
    delete[] new_y;

    // return overlap
    return overlap;
}

/**
 * Converts a square into a hexagonal implementation
 *
 * @param qvadri;       image (RGB [width*height*numberOfChannels])
 * @return image rewritten to a hexagonal representation
 */
uint8_t* Qvadri2Sex::convert(uint8_t* qvadri)
{
    int i, j, index, counter;
    float activationR, activationG, activationB;
    
    #pragma omp parallel for private(i, j, index, counter, activationR, activationG, activationB)
    for(i = 0; i < length; i++)
    {
        activationR = 0;
        activationG = 0;
        activationB = 0;

        for(j = 0, counter = sum[i]; j < number[i]; j++, counter++)
        {
            index = connection[counter];
            
            activationR += qvadri[index] * weight[counter];
            activationG += qvadri[index+1] * weight[counter];
            activationB += qvadri[index+2] * weight[counter];
        }
        
        output[i*3] = activationR;
        output[i*3+1] = activationG;
        output[i*3+2] = activationB;
    }
    
    return output;
}

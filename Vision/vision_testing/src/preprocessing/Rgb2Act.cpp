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
 * File:   Rgb2Act.cpp
 * Author: jlangevoort
 * 
 * Created on August 12, 2011, 11:00 AM
 */

#include "Rgb2Act.h"

// define method of conversion
enum method
{
    red,
    green,
    blue,
    red_green,
    green_blue,
    blue_red,
    red_green_blue
};

/**
 * Constructor
 *  Initializes data structures and stores input image data
 *
 * @param width;                    width of the input image
 * @param height;                   height of the input image
 * @param numberOfChannels;         number of channels in the input image
 */
Rgb2Act::Rgb2Act(int width, int height, int numberOfChannels)
{
    // set image properties
    length = width*height;
    this->numberOfChannels = numberOfChannels;
    // allocate memory for output
    output = new float [width*height];
}

/**
 * Destructor
 */
Rgb2Act::~Rgb2Act()
{
    delete[] output;
}

/**
 * Converts RGB value to an actvation dependent only on the red channel
 *
 * @param input;    input image
 * @return activation based solely on the red channel
 */
float* Rgb2Act::getRed(uint8_t* input)
{
    int i, j;
    for(i = 0, j = 0; i < length; i++, j+=numberOfChannels)
    {
        output[i] = input[j]/255.0;
    }

    return output;
}

/**
 * Converts RGB value to an actvation dependent only on the green channel
 *
 * @param input;    input image
 * @return activation based solely on the green channel
 */
float* Rgb2Act::getGreen(uint8_t* input)
{
    int i, j;
    for(i = 0, j = 0; i < length; i++, j+=numberOfChannels)
    {
        output[i] = input[j+1]/255.0;
    }

    return output;
}

/**
 * Converts RGB value to an actvation dependent only on the blue channel
 *
 * @param input;    input image
 * @return activation based solely on the blue channel
 */
float* Rgb2Act::getBlue(uint8_t* input)
{
    int i, j;
    for(i = 0, j = 0; i < length; i++, j+=numberOfChannels)
    {
        output[i] = input[j+2]/255.0;
    }

    return output;
}

/**
 * Converts RGB value to an actvation dependent on both the red and the green channel
 *
 * @param input;    input image
 * @return activation based on both the red and the green channel
 */
float* Rgb2Act::getRedGreen(uint8_t* input)
{
    int i, j;
    for(i = 0, j = 0; i < length; i++, j+=numberOfChannels)
    {
        output[i] = (input[j]+input[j+1])/510.0; // X/2/255
    }

    return output;
}

/**
 * Converts RGB value to an actvation dependent on both the green and the blue channel
 *
 * @param input;    input image
 * @return activation based on both the green and the blue channel
 */
float* Rgb2Act::getGreenBlue(uint8_t* input)
{
    int i, j;
    for(i = 0, j = 0; i < length; i++, j+=numberOfChannels)
    {
        output[i] = (input[j+1]+input[j+2])/510.0; // X/2/255
    }

    return output;
}

/**
 * Converts RGB value to an actvation dependent on both the blue and the red channel
 *
 * @param input;    input image
 * @return activation based on both the blue and the red channel
 */
float* Rgb2Act::getBlueRed(uint8_t* input)
{
    int i, j;
    for(i = 0, j = 0; i < length; i++, j+=numberOfChannels)
    {
        output[i] = (input[j+2]+input[j])/510.0; // X/2/255
    }

    return output;
}

/**
 * Converts RGB value to an actvation dependent on all the channels
 *
 * @param input;    input image
 * @return activation based on all the channels
 */
float* Rgb2Act::getRedGreenBlue(uint8_t* input)
{
    int i, j;
    for(i = 0, j = 0; i < length; i++, j+=numberOfChannels)
    {
        output[i] = (input[j]+input[j+1]+input[j+2])/765.0; // X/3/255
    }

    return output;
}

/**
 * Converts RGB signal into activation depedent on the method chosen
 * 
 * @param input;    input image
 * @return activation based on method of conversion
 */
float* Rgb2Act::getActivation(uint8_t* input, int methodType)
{
    if(methodType == red)
    {
        return getRed(input);
    }
    else if(methodType == green)
    {
        return getGreen(input);
    }
    else if(methodType == blue)
    {
        return getBlue(input);
    }
    else if(methodType == red_green)
    {
        return getRedGreen(input);
    }
    else if(methodType == green_blue)
    {
        return getGreenBlue(input);
    }
    else if(methodType == blue_red)
    {
        return getBlueRed(input);
    }
    else // if(methodType == red_green_blue)
    {
        return getRedGreenBlue(input);
    }
}
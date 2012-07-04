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
 * File:   Preprocessing.cpp
 * Author: jlangevoort
 * 
 * Created on September 08, 2011, 8:48 AM
 */

#include "Preprocessing.h"

// define conversion types
enum conversion
{
    act,
    lms
};

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

// define output types
enum output
{
    qvadriangvlatvm,
    sexangvlatvm
};

/**
 * Constructor
 *  initializes conversion method and output type
 *  allocates memory for data structures and initializes where necessary
 */
Preprocessing::Preprocessing(int width, int height, int hexel, float hexelDiameter, int colorspace, int convert, int l, int m, int s, int normalize, int lateral)
{
    conversionType = colorspace;
    methodType = convert;
    outputType = hexel;
    
    bool estSexangvlatvm;
    
    if (outputType)
		estSexangvlatvm = true;
	else
		estSexangvlatvm = false;
	
	if (lateral)
		isLatInh = true;
	else
		isLatInh = false;

    // set input parameters to standard
    inputWidth = width;
    inputHeight = height;
    inputNumberOfChannels = 3;	// standard RGB colorspace

    if(hexel == 0) // 0  == pixel
    {
        // set output parameters
        outputWidth = inputWidth;
        outputHeight = inputHeight;
        outputLength = outputWidth * outputHeight;
        
        // if there is any lateral inhibition
        if(lateral == 1)
        {
            // TODO: integrate number of connections made to each horizontal cell
            latInh = new LatInh(outputWidth, outputHeight, estSexangvlatvm);//, overlap, number);
            latInh2 = new LatInh(outputWidth, outputHeight, estSexangvlatvm);//, overlap, number);
        }
    }
    else // if (hexel == 1) // 1 == hexel
    {
        // convert qvadriangvlvs to sexangvlvs
        qvadri2sex = new Qvadri2Sex (inputWidth, inputHeight, inputNumberOfChannels, hexelDiameter);

        // set output parameters
        outputWidth = qvadri2sex->getWidth();
        outputHeight = qvadri2sex->getHeight();
        outputLength = qvadri2sex->getLength();
        
        // if there is any lateral inhibition
        if(lateral == 1)
        {       
            // TODO: integrate number of connections made to each horizontal cell
            latInh = new LatInh(outputWidth, outputHeight, estSexangvlatvm);//, overlap, number);
            latInh2 = new LatInh(outputWidth, outputHeight, estSexangvlatvm);//, overlap, number);
        }
    }

    if(colorspace == 1) // 1 == colorspace
    {
        // allocate memory for and initialize RGB to activation converter
        rgb2act = new Rgb2Act(outputWidth, outputHeight, inputNumberOfChannels);
        rgb2act2 = new Rgb2Act(outputWidth, outputHeight, inputNumberOfChannels);
    }
    else // if (colorspace == 0)
    {
        // allocate memory for and initialize RGB to cone activation converter
        rgb2lms = new Rgb2Lms(outputWidth, outputHeight, inputNumberOfChannels);
        // TODO: integrate into program parameters
        rgb2lms->setCellType(l, m, s);
		
		rgb2lms2 = new Rgb2Lms(outputWidth, outputHeight, inputNumberOfChannels);
		rgb2lms2->setCellType(1-l, 1-m, s);
    }
}

/**
 * Destructor
 */
Preprocessing::~Preprocessing()
{
    delete[] qvadri2sex;
    delete[] rgb2act;
    delete[] rgb2lms;
    delete[] latInh;
    
    delete[] activation;
    
    // COLOR TEST
    delete[] rgb2act2;
    delete[] rgb2lms2;
    delete[] latInh2;
    delete[] activation2;
}

/**
 * Gets the input data and converts it into useable activation
 * 
 * @param input		RGB array (uint8_t [width * heigth * number of channels])
 * @return activation based on the input and the chosen parameters
 */
float* Preprocessing::process(uint8_t* input)
{
    // convert input if needed
    if (outputType == 1)
    {
        input = qvadri2sex->convert(input);
    }

	// convert to correct data type
    if(conversionType == 1)
    {
        activation = rgb2act->getActivation(input, methodType);
        activation2 = rgb2act2->getActivation(input, 1-methodType);
    }
    else // if (conversionType == lms)
    {
        activation = rgb2lms->getLms(input);
        activation2 = rgb2lms2->getLms(input);
    }
    
    // if there is any lateral inhibition
    if (isLatInh)
    {
        activation = latInh->inhibit(activation);
        activation2 = latInh2->inhibit(activation2);
    }
    
    bool normalize = true;
    // normalize when needed
    if (normalize)
    {
        int i;
        float max = 0;
        for (i = 0; i < outputLength; i++)
        {
            if (activation[i] > max)
                max = activation[i];
        }
        for (i = 0; i < outputLength; i++)
        {
            activation[i] = activation[i] / max;
            if (activation[i] > 1)
                activation[i] = 1;
        }
    }
    
    // return calculated activation
    return activation;
}

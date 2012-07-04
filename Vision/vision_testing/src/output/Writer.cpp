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
 * Writer.cpp
 *
 *  Created on: June 7th 2011
 *      Author: J.E.Langevoort
 *
 * Handles the writing of images within the program
 */

#include "Writer.h"

/*
 * Initialize with default image dimensions
 */
Writer::Writer(int width, int height, int nchannels)
{
  // initialize image
  image = 0;
  ilGenImages(1, &image);
  ilBindImage(image);

  // enables overwriting of files
  ilEnable(IL_FILE_OVERWRITE);

  // set image properties
  this->width = width;
  this->height = height;
  this->nchannels = nchannels;
}

/**
 * Destuctor
 */
Writer::~Writer()
{
  ilBindImage(0);
  ilDeleteImage(image);
}

/**
 * Writes image to file
 */
void Writer::saveImage(char* filename, uint8_t* pixelMap)
{
  ilTexImage(width, height, 1, nchannels, IL_RGB, IL_UNSIGNED_BYTE, pixelMap);
  ilSaveImage(filename);
}


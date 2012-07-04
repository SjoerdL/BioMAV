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
 * File:   Image.cpp
 * Author: jlangevoort
 *
 * Created on August 27, 2011, 11:00 AM
 */

#include "Image.h"

#include <sstream>
using namespace std;

/*
 * Reads standard image with 'filename'
 */
Image::Image(int numberOfFiles)
{
  counter = 200;
  //this->width = width;
  //this->height = height;
  this->numberOfFiles = numberOfFiles + counter - 1;

  char* filename = (char*)"input/152.jpg";

  // initialize dev image library image
  ilInit();

  // initialize, open and load prototypical image
  image = 0;
  ilGenImages(1, &image);
  ilBindImage(image);
  // When loading images DevIL preserve their origin.
  //  If you want them at a fixed origin you must enable IL_ORIGIN_SET and set the origin you want.
  //ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
  ilEnable(IL_ORIGIN_SET);
  ilLoadImage(filename);

  // extract necessary data from image
  width = ilGetInteger(IL_IMAGE_WIDTH);
  height = ilGetInteger(IL_IMAGE_HEIGHT);
  nchannels = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
  // allocate memory space for the image
  pixelMap = new uint8_t[width * height * nchannels];
  // copy for testing purposes
  ilCopyPixels(0,0,0,width,height,1,IL_RGB,IL_UNSIGNED_BYTE,pixelMap);
}

/*
 * Destructor
 */
Image::~Image()
{
  delete []pixelMap;
  ilBindImage(0);
  ilDeleteImage(image);
}

/*
 * Tries to open an image with 'filename'
 */
void Image::readImage(char* filename)
{
    ilLoadImage(filename);
    ilCopyPixels(0,0,0,width,height,1,IL_RGB,IL_UNSIGNED_BYTE,pixelMap);
}

/*
 * opens and returns data of the next image file
 */
uint8_t* Image::getData()
{
	if (counter == numberOfFiles)
		return 0;

	counter++;
    
    string filename;
    stringstream ss;

    filename = "input/";
    //test.append((string)counter);
    ss << counter;
    filename.append(ss.str());
    filename.append(".jpg");
        
	readImage((char*)filename.c_str());
	return pixelMap;
}

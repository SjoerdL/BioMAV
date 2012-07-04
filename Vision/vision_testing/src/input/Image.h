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
 * File:   Image.h
 * Author: jlangevoort
 *
 * Created on August 27, 2011, 11:00 AM
 *
 * Handles the reading of images within the program
 *
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <IL/il.h>      // image library (devIL)
#include <stdint.h>     // typedefs with exact-width integer types

class Image
{
private:
  // image
  ILuint image;
  // pixel map of the image
  uint8_t* pixelMap;
  // image properties
  int width;
  int height;
  int nchannels;
  // file-counter
  int counter;
  int numberOfFiles;

public:
  Image (int numberOfFiles);
  ~Image();
  // read image
  void readImage (char* filename);
  // return pixel map
  uint8_t* getData();
  // image properties
  int getWidth (){return width;};
  int getHeight (){return height;};
  int getNchannels (){return nchannels;};
};

#endif /* IMAGE_H_ */



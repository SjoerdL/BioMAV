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
 * Writer.h
 *
 *  Created on: June 7th 2011
 *      Author: J.E.Langevoort
 *
 * Handles the writing of images within the program
 */

#ifndef WRITER_H_
#define WRITER_H_

#include <IL/il.h>      // image library (devIL)
#include <stdint.h>     // typedefs with exact-width integer types

class Writer
{
private:
  // image
  ILuint image;
  // image properties
  int width;
  int height;
  int nchannels;
public:
  Writer(int width, int height, int nchannels);
  ~Writer();
  // write image to file
  void saveImage (char* filename, uint8_t* image);
};

#endif /* WRITER_H_ */

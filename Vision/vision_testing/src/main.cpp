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
 * File:   main.cpp
 * Author: jlangevoort
 *
 * Created on August 12, 2011, 8:20 AM
 */

#include <stdint.h>         // exact-width integer types
#include <stdio.h>          // standard input and output operations

#include <iostream>

#include "Input.h"          // input class
#include "Preprocessing.h"  // preprocessing class
#include "Processing.h"     // processing class
#include "Recognition.h"	// processing class

#if defined(__APPLE__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

using namespace std;

#include "GlobalDrawCode.cpp"

/**
 * Initializes data and processes the stream until forced to quit
 *  prints data to openGL screen
 */
int main(int argc, char **argv)
{
  if (argc < 12)
  {
    printf("12 arguments needed, %d given\n", argc);
    return 0;
  }
      
  width = atoi(argv[1]);
  height = atoi(argv[2]); 
  int webcamIndex = atoi(argv[3]);
  int hexel = atoi(argv[4]);
  float hexelDiameter = atof(argv[5]);
  int colorspace = atoi(argv[6]);
  int convert = atoi(argv[7]);
  int l = atoi(argv[8]);
  int m = atoi(argv[9]);
  int s = atoi(argv[10]);
  int normalize = atoi(argv[11]);
  int lateral = atoi(argv[12]);
      
  input = new Input(width, height, webcamIndex);
  init(input, width, height, hexel, hexelDiameter, colorspace, convert, l, m, s, normalize, lateral);
  
  return 0;
}

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
#include "nl_ru_ai_projects_parrot_poledetection_CPPPoleDetector.h"

#include <stdint.h>         // exact-width integer types
#include <stdio.h>          // standard input and output operations

#include <boost/thread.hpp>

#include <iostream>

#include "../Preprocessing.h"  // preprocessing class
#include "../Processing.h"     // processing class
#include "../Recognition.h"    // processing class

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
using namespace boost;

recursive_mutex sharedDataMutex;
uint8_t* rawImage;
int fullWidth, fullHeight;
bool newData = false;

#define SYNC_MUTEX_LOCK_CMD unique_lock<recursive_mutex> sharedDataLock(sharedDataMutex)
#define SYNC_MUTEX_UNLOCK_CMD sharedDataLock.unlock()

class Input {
  private:
    int width, height;
  public:
    Input (int w, int h) {
      this->width = w;
      this->height = h;
    }
  
    uint8_t* getInput() {
      if (newData) {
	      newData = false;
        return rawImage;
      }
      return NULL;
    }
    
    // return data for usage as parameters for processing
    int getWidth(){
      return this->width;
    }
    
    int getHeight(){
      return this->height;
    }
    
    int getNumberOfChannels(){
      return 3;
    }
};

///////////////// main.cpp stuff //////////////////

#include "../GlobalDrawCode.cpp"

////////////////////////// JNI Stuff ///////////////////////

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_poledetection_CPPPoleDetector_init0
  (JNIEnv* env, jclass, jint _width, jint _height, jboolean _hexel, jdouble _hexelDiameter, jboolean _colorspace, jboolean _convert, jint _l, jint _m, jint _s, jboolean _normalize, jboolean _lateral) {
  cout << "In init" << endl;

  width = _width;
  height = _height; 
  fullWidth = _width;
  fullHeight = _height;
  
  rawImage = new uint8_t [width*height*3];
  
  int hexel = _hexel ? 1 : 0;
  float hexelDiameter = 3.75;
  int colorspace = _colorspace ? 1 : 0;
  int convert = _convert ? 1 : 0;
  int l = 1;
  int m = 0;
  int s = 0;
  int normalize = _normalize ? 1 : 0;
  int lateral = _lateral ? 1 : 0;

  newData = false;
  
  init(new Input(_width, _height), width, height, hexel, hexelDiameter, colorspace, convert, l, m, s, normalize, lateral);
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_poledetection_CPPPoleDetector_setData(JNIEnv* env, jclass, jobject buffer) {
  lock_guard<recursive_mutex> sharedDataLock(sharedDataMutex);
  
  if (env->GetDirectBufferCapacity(buffer) < fullWidth * fullHeight * 3) {
    cerr << "Image buffer size too small: " << env->GetDirectBufferCapacity(buffer) << " < " << fullWidth * fullHeight * 3 << endl;
  } else {
    void* dbAddress = env->GetDirectBufferAddress(buffer);
    if (dbAddress == NULL) {
      cerr << "Image buffer has no contents (env->GetDirectBufferAddress == NULL)" << endl;
    } else {
      newData = true;
      memcpy(rawImage, dbAddress, fullWidth * fullHeight * 3);
    }
  }
}

JNIEXPORT jdoubleArray JNICALL Java_nl_ru_ai_projects_parrot_poledetection_CPPPoleDetector_getPolePositions(JNIEnv* env, jclass cls) {
  lock_guard<recursive_mutex> sharedDataLock(sharedDataMutex);
  getData();

  jdoubleArray result = env->NewDoubleArray(recognition->getPoleCount());
  
  double dvalues[recognition->getPoleCount()];
  for (int i = 0; i < recognition->getPoleCount(); i++) {
    dvalues[i] = recognition->getPolePosition(i);
  }
  env->SetDoubleArrayRegion(result, 0, recognition->getPoleCount(), dvalues);
  
  return result;
}

JNIEXPORT jdoubleArray JNICALL Java_nl_ru_ai_projects_parrot_poledetection_CPPPoleDetector_getPoleDistances
  (JNIEnv* env, jclass) {
  lock_guard<recursive_mutex> sharedDataLock(sharedDataMutex);
  getData();

  jdoubleArray result = env->NewDoubleArray(recognition->getPoleCount());
  
  double dvalues[recognition->getPoleCount()];
  for (int i = 0; i < recognition->getPoleCount(); i++) {
    dvalues[i] = recognition->getPoleDistance(i);
  }
  env->SetDoubleArrayRegion(result, 0, recognition->getPoleCount(), dvalues);
  
  return result;
}

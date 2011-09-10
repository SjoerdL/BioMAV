/*
 * GroundStation.cpp
 *
 *  Created on: 11.07.2011
 *      Author: paul
 */

#include "nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation.h"

#include <memory>
#include <iostream>

#include <boost/shared_array.hpp>
#include <boost/thread.hpp>

#include "../drone/DroneControl.hpp"

using namespace std;
using namespace boost;

recursive_mutex droneControlObjectMutex;
auto_ptr<DroneControl> droneControlObject;

// Front image data
shared_array<char> droneFrontImageData;
int allocatedFrontImageBufferData = 0;
jobject frontImageBufferData = NULL;

// Navdata data goarded by a mutex
recursive_mutex navdataMutex;
long sensorTimeStamp = -1;
double rotationVector[3] = {0, 0, 0};

void navdataCallback() {
//  cout << "Navdata callback!" << endl;
//  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex); Not completely correct... cleanup must somehow be synchronized with this...
  lock_guard<recursive_mutex> navdataLock(navdataMutex);

  if (droneControlObject.get() != NULL) {
    sensorTimeStamp = droneControlObject->getNavdataTimestamp();

    rotationVector[0] += droneControlObject->getGyroX();
    rotationVector[1] += droneControlObject->getGyroY();
    rotationVector[2] += droneControlObject->getGyroZ();
  } else {
    return;
  }
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_createInstance0(JNIEnv* env, jclass cls) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject = auto_ptr<DroneControl>(new DroneControl());
  droneControlObject->setNavDataCallback(&navdataCallback);
}

JNIEXPORT jobject JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getFrontCameraImage0(JNIEnv* env, jobject obj) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  int imageSize = droneControlObject->getImageWidth() * droneControlObject->getImageHeight() * 3;

  if (imageSize == 0) {
    return NULL;
  }

  if (imageSize != allocatedFrontImageBufferData) {
    if (frontImageBufferData != NULL) {
      env->DeleteGlobalRef(frontImageBufferData);
      frontImageBufferData = NULL;
    }

    allocatedFrontImageBufferData = imageSize;
    if (imageSize > 0) {
      droneFrontImageData = shared_array<char>(new char[allocatedFrontImageBufferData]);
      frontImageBufferData = env->NewDirectByteBuffer(droneFrontImageData.get(), allocatedFrontImageBufferData);
      if (frontImageBufferData == NULL) {
        return NULL;
      }
    }
  }
  char* imageData = droneControlObject->getImageData();
  memcpy(droneFrontImageData.get(), imageData, imageSize);

  return frontImageBufferData;
}

JNIEXPORT jlong JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getFrontCameraTimeStamp0(JNIEnv* env, jobject obj) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  return droneControlObject->getImageTimestamp();
}

JNIEXPORT jdoubleArray JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getRotationVector0(JNIEnv* env, jobject obj) {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);

  jdoubleArray result = env->NewDoubleArray(3);
  if (result == NULL) {
    return NULL;
  }

  env->SetDoubleArrayRegion(result, 0, 3, rotationVector);
  rotationVector[0] = 0;
  rotationVector[1] = 0;
  rotationVector[2] = 0;

  return result;
}

JNIEXPORT jlong JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getSensoryTimeStamp0
  (JNIEnv *, jobject) {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  return sensorTimeStamp;
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_clearEmergencyFlag0(JNIEnv* env, jobject) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->setEmergency(false);
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_flyForward0(JNIEnv* env, jobject, jdouble speed) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->setForward(speed);
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_flySideways0(JNIEnv* env, jobject, jdouble speed) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->setSidewards(speed);
}

JNIEXPORT jint JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getFlyingState0(JNIEnv* env, jobject) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  if (droneControlObject->getErrorState() != DRONE_ERROR_NO_ERROR) {
    return nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_EMERGENCY;
  }

  if (droneControlObject->getStartMode()) {
    return nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_FLYING;
  }

  return nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_LANDED;
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_hover0(JNIEnv* env, jobject) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->hover();
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_land0(JNIEnv* env, jobject) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->setStartMode(false);
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_takeoff0(JNIEnv* env, jobject) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->setStartMode(true);
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_setEmergencyFlag0(JNIEnv* env, jobject) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->setEmergency(true);
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_setHeight0(JNIEnv* env, jobject, jdouble height) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->setHeight(height);
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_spin0(JNIEnv* env, jobject, jdouble speed) {
  lock_guard<recursive_mutex> dcoLock(droneControlObjectMutex);
  droneControlObject->setYawSpeed(speed);
}


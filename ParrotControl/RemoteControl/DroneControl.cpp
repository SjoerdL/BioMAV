#include "DroneControl.hpp"

#include <iostream>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/median.hpp>

#include <SDL/SDL.h>

#define DEBUG_STRING(s)
//#define DEBUG_STRING(s) cout << s << endl;

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define MIN_HEIGHT 300
#define MAX_HEIGHT 4000
#define HEIGHT_DEFAULT_DEVIATION 100
#define HEIGHT_CORRECTION_FACTOR 2.5
#define HEIGHT_MIN_CORRECTION_VALUE 0.05

#define DRONE_ANGLE_MAX_VALUE 38000
#define DRONE_ANGLE_MAX_ANGLE (M_PI / 4.0)

#define DRONE_GYRO_FULL_CIRCLE 6500

// For measure stabilization purposes - 100 ~= 500ms
#define HEIGHT_HISTORY_LENGTH 100

using namespace std;
using namespace boost;

bool oneDroneInitialized = false;

void DroneControlNavdataHandlerFunc(const navdata_unpacked_t* const navData, void* userdata) {
  ((DroneControl*) userdata)->processNavdata(navData);
}

void DroneControlImageHandlerFunc(char* imageData, int width, int height, void* userdata) {
  ((DroneControl*) userdata)->processImage(imageData, width, height);
}


void DroneControl :: processNavdata(const navdata_unpacked_t* const navData) {
/*  cout << "Raw: " << navData->navdata_altitude.altitude_raw
       << "  vz: " << navData->navdata_altitude.altitude_vz
       << "  vision: " << navData->navdata_altitude.altitude_vision
       << "  ref: " << navData->navdata_altitude.altitude_ref << endl;*/
  {
    lock_guard<recursive_mutex> controlThreadLock(controlThreadMutex);
    lock_guard<recursive_mutex> navdataLock(navdataMutex);
    navigationReceived = true;
    navdataTimeStamp = SDL_GetTicks() - droneControlStartTime;
    newNavdata = true;
    memcpy(&lastNavdata, navData, sizeof(lastNavdata));

    if (externalNavdataCallback != NULL) {
      externalNavdataCallback();
    }
  }

  notifyControlThread.notify_all();
}

void DroneControl :: processImage(char* imageData, int width, int height) {
  lock_guard<recursive_mutex> navdataLock(imageMutex);
  if (allocatedImageSize < width * height * 4) {
    if (allocatedImageSize > 0) {
      delete[] primaryImageStorage;
      delete[] outerImageStorage;
    }

    allocatedImageSize = width * height * 4;
    primaryImageStorage = new char[allocatedImageSize];
  }
  imageTimeStamp = SDL_GetTicks() - droneControlStartTime;
  imageWidth = width;
  imageHeight = height;
  memcpy(primaryImageStorage, imageData, width * height * 4);
}

void DroneControl :: hover() {
  {
    lock_guard<recursive_mutex> controlThreadLock(controlThreadMutex);
    lock_guard<recursive_mutex> navdataLock(navdataMutex);

    if (!inHeightCommand) {
      forwardAngle = 0;
      sidewardsAngle = 0;
      yawSpeed = 0;
      targetHeight = currentHeight;
      heightSpeed = 0;
    }

    hovering = true;
  }

  notifyControlThread.notify_all();
}

void DroneControl :: setEmergency(bool b) {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  drone_setEmergency(b);
}

void DroneControl :: updateFlyingState() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  drone_fly(-forwardAngle, sidewardsAngle, heightSpeed, yawSpeed);

  hovering = false;
}

void DroneControl :: setForward(float f) {
  {
    lock_guard<recursive_mutex> controlThreadLock(controlThreadMutex);
    lock_guard<recursive_mutex> navdataLock(navdataMutex);
    forwardAngle = MIN(1, MAX(-1, f));
    hovering = false;
  }

  notifyControlThread.notify_all();
}

void DroneControl :: setSidewards(float f) {
  {
    lock_guard<recursive_mutex> controlThreadLock(controlThreadMutex);
    lock_guard<recursive_mutex> navdataLock(navdataMutex);
    sidewardsAngle = MIN(1, MAX(-1, f));
    hovering = false;
  }

  notifyControlThread.notify_all();
}

void DroneControl :: setHeight(float f) {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if ((!navigationReceived) || (fabs((currentHeight - f) * (MAX_HEIGHT - MIN_HEIGHT)) > HEIGHT_DEFAULT_DEVIATION)) {
    targetHeight = f;
    inHeightCommand = true;
    heightCorrectionDirection = (targetHeight < currentHeight ? -1 : 1);
  }
}

void DroneControl :: setYawSpeed(float f) {
  {
    lock_guard<recursive_mutex> controlThreadLock(controlThreadMutex);
    lock_guard<recursive_mutex> navdataLock(navdataMutex);
    yawSpeed = MIN(1, MAX(-1, f));
    hovering = false;
  }

  notifyControlThread.notify_all();
}

void DroneControl :: setStartMode(bool b) {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (b) {
    drone_takeoff();
  } else {
    drone_land();
  }
  currentStartMode = b;
}

void DroneControl :: operator()() {
  unique_lock<mutex> threadLock(notifyControlThreadMutex);

//  Uint32 startTime = (int) SDL_GetTicks();

  while (runControlThread) {
    {
      lock_guard<recursive_mutex> controlThreadLock(controlThreadMutex);
      DEBUG_STRING("run control thread: locking navdataMutex");
      lock_guard<recursive_mutex> navdataLock(navdataMutex);
      DEBUG_STRING("run control thread: locked navdataMutex");

      if (newNavdata) {
        newNavdata = false;

        // process navdata...
        float newHeight = (float) (lastNavdata.navdata_altitude.altitude_vision - MIN_HEIGHT) / (float) (MAX_HEIGHT - MIN_HEIGHT);
//        cout << lastNavdata.navdata_euler_angles.theta_a << endl;

        medianHeightBuffer.push_back(newHeight);
        {
          accumulators::accumulator_set<float, accumulators::features<accumulators::tag::median> > acc;

          for (MedianHeightBufferType::iterator it = medianHeightBuffer.begin(); it < medianHeightBuffer.end(); it++) {
            acc(*it);
          }
          currentHeight = accumulators::median(acc);
        }
//        cout << (float) (SDL_GetTicks() - startTime) / 1000.0 << "\t" << currentHeight << endl;

        if (!inHeightCommand) {
        } else {
          if ((currentHeight - targetHeight) * heightCorrectionDirection > (float) -HEIGHT_DEFAULT_DEVIATION / ((float) 2 * (MAX_HEIGHT - MIN_HEIGHT))) {
            inHeightCommand = false;
            if (hovering) {
              hover();
            } else {
              updateFlyingState();
            }
//            cout << "Height reached!" << endl;
          } else {
            heightSpeed = heightCorrectionDirection * MAX(HEIGHT_MIN_CORRECTION_VALUE, HEIGHT_CORRECTION_FACTOR * fabs(targetHeight - currentHeight));
            heightSpeed = MAX(-1, MIN(1, heightSpeed));
            bool tmpHovering = hovering;
            updateFlyingState();
            hovering = tmpHovering;
          }
        }
      } else {
        if (inHeightCommand || (!hovering)) {
          updateFlyingState();
        } else {
          drone_hover();
        }
      }
    }

    DEBUG_STRING("waiting for: notifyControlThread");
    notifyControlThread.wait(threadLock);
    DEBUG_STRING("received: notifyControlThread");
  }
}

bool DroneControl :: isEmergencySet() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  return drone_getEmergency();
}

bool DroneControl :: isHoverSet() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  return hovering && currentStartMode;
}

void DroneControl :: getSetMovementVector(float& forward, float& sideward, float& yaw, float& height) {
  forward = forwardAngle;
  sideward = sidewardsAngle;
  yaw = yawSpeed;
  height = heightSpeed;
}

bool DroneControl :: getStartMode() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  return currentStartMode;
}

unsigned int DroneControl :: getImageTimestamp() {
  lock_guard<recursive_mutex> imageLock(imageMutex);
  return imageTimeStamp;
}

char* DroneControl :: getImageData() {
  lock_guard<recursive_mutex> imageLock(imageMutex);

  if (allocatedOuterImageSize < allocatedImageSize) {
    if (outerImageStorage != NULL) {
      delete[] outerImageStorage;
    }
    allocatedOuterImageSize = allocatedImageSize;
    outerImageStorage = new char[allocatedOuterImageSize];
  }
  if ((outerImageStorage != NULL) && (primaryImageStorage != NULL)) {
    memcpy(outerImageStorage, primaryImageStorage, allocatedImageSize);
  }

  return outerImageStorage;
}

int DroneControl :: getImageWidth() {
  lock_guard<recursive_mutex> imageLock(imageMutex);
  return imageWidth;
}

int DroneControl :: getImageHeight() {
  lock_guard<recursive_mutex> imageLock(imageMutex);
  return imageHeight;
}

void DroneControl :: setNavDataCallback(DroneControlNavdataCallback callback) {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  externalNavdataCallback = callback;
}

unsigned int DroneControl :: getNavdataTimestamp() {
  lock_guard<recursive_mutex> imageLock(imageMutex);
  return navdataTimeStamp;
}

int DroneControl :: getErrorState() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return DRONE_ERROR_COMMUNICATION_LOST;
  }
  if ((lastNavdata.ardrone_state & ARDRONE_ANGLES_OUT_OF_RANGE) != 0) {
    return DRONE_ERROR_ANGLE_ERROR;
  }
  if ((lastNavdata.ardrone_state & ARDRONE_MOTORS_MASK) != 0) {
    return DRONE_ERROR_MOTOR_ERROR;
  }
  if ((lastNavdata.ardrone_state & ARDRONE_VBAT_LOW) != 0) {
    return DRONE_ERROR_BATTERY_LOW;
  }
  if ((lastNavdata.ardrone_state & ARDRONE_EMERGENCY_MASK) != 0) {
    return DRONE_ERROR_EMERGENCY;
  }
/*  if ((lastNavdata.ardrone_state & ARDRONE_COM_LOST_MASK) != 0) {
    return DRONE_ERROR_COMMUNICATION_LOST;
  }*/
  return DRONE_ERROR_NO_ERROR;
}

float DroneControl :: getHeight() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  return currentHeight;
}

float DroneControl :: getEulerTheta() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_euler_angles.theta_a * DRONE_ANGLE_MAX_ANGLE / DRONE_ANGLE_MAX_VALUE;
}

float DroneControl :: getEulerPhi() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_euler_angles.phi_a * DRONE_ANGLE_MAX_ANGLE / DRONE_ANGLE_MAX_VALUE;
}

float DroneControl :: getGyroX() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_phys_measures.phys_gyros[GYRO_X] / DRONE_GYRO_FULL_CIRCLE * M_PI * 2;
}

float DroneControl :: getGyroY() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_phys_measures.phys_gyros[GYRO_Y] / DRONE_GYRO_FULL_CIRCLE * M_PI * 2;
}

float DroneControl :: getGyroZ() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_phys_measures.phys_gyros[GYRO_Z] / DRONE_GYRO_FULL_CIRCLE * M_PI * 2;
}

float DroneControl :: getAccX() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_phys_measures.phys_accs[ACC_X];
}

float DroneControl :: getAccY() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_phys_measures.phys_gyros[ACC_Y];
}

float DroneControl :: getAccZ() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_phys_measures.phys_gyros[ACC_Z];
}

float DroneControl :: getGroundspeedX() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_demo.vx;
}

float DroneControl :: getGroundspeedY() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_demo.vy;
}

float DroneControl :: getGroundspeedZ() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_demo.vz;
}

int DroneControl :: getBatteryLevel() {
  lock_guard<recursive_mutex> navdataLock(navdataMutex);
  if (!navigationReceived) {
    return 0;
  }
  return lastNavdata.navdata_demo.vbat_flying_percentage;
}

DroneControl :: DroneControl() {
  droneControlStartTime = SDL_GetTicks() - 1;

  if (oneDroneInitialized) {
    throw string("Drone already initialized");
  }

  if (!drone_init(this)) {
    throw string("Drone initialization failed");
  }
  cout << "Drone initialized" << endl;

  drone_registerNavdataCallback(&DroneControlNavdataHandlerFunc);
  drone_registerVideoCallback(&DroneControlImageHandlerFunc);

  oneDroneInitialized = true;

  currentStartMode = false;

  inHeightCommand = false;
  heightSpeed = 0;
  hovering = true;
  initialized = true;

  navigationReceived = false;
  newNavdata = false;
  navdataTimeStamp = 0;

  medianHeightBuffer.set_capacity(HEIGHT_HISTORY_LENGTH);
  medianHeightBuffer.clear();

  imageTimeStamp = 0;
  imageWidth = -1;
  imageHeight = -1;
  allocatedImageSize = 0;
  primaryImageStorage = NULL;
  allocatedOuterImageSize = 0;
  outerImageStorage = NULL;

  externalNavdataCallback = NULL;

  runControlThread = true;
  controlThread = thread(boost::ref(*this));
}

DroneControl :: ~DroneControl() {
  if (initialized) {
    cout << "Drone shutdown" << endl;

    {
      lock_guard<recursive_mutex> controlThreadLock(controlThreadMutex);
      runControlThread = false;
    }
    notifyControlThread.notify_all();
    controlThread.join();

    cout << "Control thread terminated" << endl;

    if (primaryImageStorage != NULL) {
      delete[] primaryImageStorage;
      primaryImageStorage = NULL;
    }
    if (outerImageStorage != NULL) {
      delete[] outerImageStorage;
      outerImageStorage = NULL;
    }

    drone_free();

    oneDroneInitialized = false;
  }
}

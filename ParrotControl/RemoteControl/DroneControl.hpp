#ifndef DRONECONTROL_HPP_
#define DRONECONTROL_HPP_

#include <string>
#include <boost/thread.hpp>
#include <boost/circular_buffer.hpp>

extern "C" {
  #include "DroneControlC.h"
}

enum {
  DRONE_ERROR_NO_ERROR,
  DRONE_ERROR_EMERGENCY,
  DRONE_ERROR_BATTERY_LOW,
  DRONE_ERROR_COMMUNICATION_LOST,
  DRONE_ERROR_MOTOR_ERROR,
  DRONE_ERROR_ANGLE_ERROR,
};

typedef void (*DroneControlNavdataCallback)();

void DroneControlNavdataHandlerFunc(const navdata_unpacked_t* const navData, void* userdata);
void DroneControlImageHandlerFunc(char* imageData, int width, int height, void* userdata);

typedef boost::circular_buffer<float> MedianHeightBufferType;

class DroneControl {
protected:
  unsigned int droneControlStartTime;

  boost::recursive_mutex navdataMutex;

  unsigned int navdataTimeStamp;
  bool initialized, hovering, navigationReceived;

  float forwardAngle, sidewardsAngle, yawSpeed, heightSpeed;

  boost::thread controlThread;
  boost::recursive_mutex controlThreadMutex;
  bool newNavdata, runControlThread;
  boost::mutex notifyControlThreadMutex;
  boost::condition_variable notifyControlThread;
  navdata_unpacked_t lastNavdata;

  MedianHeightBufferType medianHeightBuffer;

  bool currentStartMode;

  bool inHeightCommand;
  float heightCorrectionDirection;
  float currentHeight, targetHeight;

  boost::recursive_mutex imageMutex;
  int allocatedImageSize, imageWidth, imageHeight;
  unsigned int imageTimeStamp;
  char* primaryImageStorage;
  int allocatedOuterImageSize;
  char* outerImageStorage;

  DroneControlNavdataCallback externalNavdataCallback;

  virtual void processNavdata(const navdata_unpacked_t* const navData);
  virtual void processImage(char* imageData, int width, int height);

  virtual void updateFlyingState();
public:
  virtual void setEmergency(bool b);
  virtual void setStartMode(bool b);

  virtual void hover();
  virtual void setForward(float f);
  virtual void setSidewards(float f);
  virtual void setHeight(float f);
  virtual void setYawSpeed(float f);

  virtual void operator()();

  virtual bool isEmergencySet();
  virtual bool getStartMode();
  virtual bool isHoverSet();
  virtual void getSetMovementVector(float& forward, float& sideward, float& yaw, float& height);

  virtual unsigned int getImageTimestamp();
  virtual char* getImageData();
  virtual int getImageWidth();
  virtual int getImageHeight();

  virtual void setNavDataCallback(DroneControlNavdataCallback callback);

  virtual unsigned int getNavdataTimestamp();

  virtual int getErrorState();

  virtual float getHeight();
  virtual float getEulerTheta();
  virtual float getEulerPhi();

  virtual float getGyroX();
  virtual float getGyroY();
  virtual float getGyroZ();

  virtual float getAccX();
  virtual float getAccY();
  virtual float getAccZ();

  virtual float getGroundspeedX();
  virtual float getGroundspeedY();
  virtual float getGroundspeedZ();

  virtual int getBatteryLevel();

  DroneControl();
  virtual ~DroneControl();

  friend void DroneControlNavdataHandlerFunc(const navdata_unpacked_t* const navData, void* userdata);
  friend void DroneControlImageHandlerFunc(char* imageData, int width, int height, void* userdata);
};

#endif /* DRONECONTROL_HPP_ */

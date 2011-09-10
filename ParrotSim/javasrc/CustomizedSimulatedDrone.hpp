#ifndef CUSTOMIZEDSIMULATEDDRONE_HPP_
#define CUSTOMIZEDSIMULATEDDRONE_HPP_

#include "SimulatedDrone.hpp"

#include "Parrot.hpp"

struct DroneIndexStruct {
  int index;
  VirtualParrotDrone* droneRef;
  jobject javaObjectRef;
};

DroneIndexStruct getDroneIndexStruct(VirtualParrotDrone* droneRef);
DroneIndexStruct getDroneIndexStruct(JNIEnv* env, VirtualParrotDrone* droneRef);
DroneIndexStruct getDroneIndexStruct(int droneIndex);

void cleanupDroneReferences(JNIEnv* env);

#endif // CUSTOMIZEDSIMULATEDDRONE_HPP_

#ifndef SHAREDJNIDATA_HPP_
#define SHAREDJNIDATA_HPP_

#include <vector>

#include <jni.h>

#include <Parrot.hpp>

#include <boost/thread.hpp>

extern std::vector<VirtualParrotDrone*> drones;
extern jobject javaSimulatorObject;
extern boost::recursive_mutex odeMutex;

void callAllDroneUpdateFunctions(JNIEnv* env, jmethodID notifyMethod);

#endif // SHAREDJNIDATA_HPP_

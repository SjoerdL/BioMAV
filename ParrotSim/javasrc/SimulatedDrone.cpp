#include "CustomizedSimulatedDrone.hpp"
#include "SharedJNIData.hpp"

#include <cstdlib>

#include <string>
#include <iostream>
#include <vector>

#include <boost/thread.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include "Lua2JavaTypeConversion.hpp"

#ifdef DEBUG
#define STRINGIFY1(x) #x
#define STRINGIFY2(x) STRINGIFY1(x)
#define DEBUG_STRING(s) cerr << __FILE__ ":" STRINGIFY2(__LINE__) " " s << endl;
#else
#define DEBUG_STRING(s)
#endif 

using namespace std;
using namespace boost;
using namespace boost::multi_index;

struct DI_INDEX {};
struct DI_DRONEREF {};

typedef multi_index_container<
  DroneIndexStruct,
  indexed_by<
    ordered_unique< tag<DI_INDEX>, member<DroneIndexStruct, int, &DroneIndexStruct::index> >,
    ordered_unique< tag<DI_DRONEREF>, member<DroneIndexStruct, VirtualParrotDrone*, &DroneIndexStruct::droneRef> >
  >
> DroneIndex_Set;

recursive_mutex droneIndexMutex;
int droneIndexCounter = 1;
DroneIndex_Set droneIndices;

void callAllDroneUpdateFunctions(JNIEnv* env, jmethodID notifyMethod) {
  DroneIndex_Set::index<DI_INDEX>::type& droneIndexSet = droneIndices.get<DI_INDEX>();
  for (DroneIndex_Set::index<DI_INDEX>::type::iterator droneIndexIterator = droneIndexSet.begin(); droneIndexIterator != droneIndexSet.end(); droneIndexIterator++) {
    jobject obj = droneIndexIterator->javaObjectRef;
    if (obj != NULL) {
      DEBUG_STRING("Calling drone update function");
      env->CallVoidMethod(obj, notifyMethod);
    }
  }  
}

int getDroneIndex(JNIEnv* env, jobject obj) {
  jclass cls = env->GetObjectClass(obj);

  if (cls != NULL) {
    jfieldID fid = env->GetFieldID(cls, "droneRefID", "I");
    if (fid != NULL) {
      int result = env->GetIntField(obj, fid);
      return result;
    }
  }

  return -1;
}

DroneIndexStruct getDroneIndexStruct(int droneIndex) {
  lock_guard<recursive_mutex> droneIndexLock(droneIndexMutex);
  
  DroneIndexStruct result;
  memset(&result, 0, sizeof(result));

  if ((droneIndex < 1) || (droneIndex >= droneIndexCounter)) {
    return result;
  }
  
  DroneIndex_Set::index<DI_INDEX>::type& droneIndexSet = droneIndices.get<DI_INDEX>();
  DroneIndex_Set::index<DI_INDEX>::type::iterator droneIndexIterator = droneIndexSet.find(droneIndex);
  if (droneIndexIterator != droneIndexSet.end()) {
    return *droneIndexIterator;
  }
  return result;
}

DroneIndexStruct getDroneIndexStruct(VirtualParrotDrone* droneRef) {
  getDroneIndexStruct(NULL, droneRef);
}

DroneIndexStruct getDroneIndexStruct(JNIEnv* env, VirtualParrotDrone* droneRef) {
  lock_guard<recursive_mutex> droneIndexLock(droneIndexMutex);
  
  DroneIndex_Set::index<DI_DRONEREF>::type& droneIndexSet = droneIndices.get<DI_DRONEREF>();
  DroneIndex_Set::index<DI_DRONEREF>::type::iterator droneIndexIterator = droneIndexSet.find(droneRef);
  if (droneIndexIterator != droneIndexSet.end()) {
    DEBUG_STRING("Drone index data found - returning found java object");
    return *droneIndexIterator;
  }

  DroneIndexStruct result;
  memset(&result, 0, sizeof(result));
  
  if (droneRef != NULL) {
    if (env != NULL) {
      DEBUG_STRING("No drone index data found - creating new Java object");

      jclass droneClass = env->FindClass("nl/ru/ai/projects/parrot/dronecontrol/simulator/SimulatedDrone");
      if (droneClass == NULL) {
        cerr << "Could not retrieve nl/ru/ai/projects/parrot/dronecontrol/simulator/SimulatedDrone" << endl;
      }
      jmethodID droneConstructorReference = env->GetMethodID(droneClass, "<init>", "(Lnl/ru/ai/projects/parrot/dronecontrol/simulator/Simulator;I)V");
      if (droneConstructorReference == NULL) {
        cerr << "Could not retrieve constructor of SimulatedDrone" << endl;
      }
      jobject droneObject = env->NewObject(droneClass, droneConstructorReference, javaSimulatorObject, droneIndexCounter);
      if (droneObject == NULL) {
        cerr << "Could not create SimulatedDrone" << endl;
      }

      if (droneObject != NULL) {    
        DEBUG_STRING(" -> Creating drone");
        result.droneRef = droneRef;
        result.index = droneIndexCounter;
        result.javaObjectRef = env->NewGlobalRef(droneObject);
        
        droneIndices.insert(result);
        
        droneIndexCounter++;
      } else {
        cerr << "Error! Could not create drone reference!" << endl;
      }
    }
  }
  
  return result;
}

void cleanupDroneReferences(JNIEnv* env) {
  lock_guard<recursive_mutex> droneIndexLock(droneIndexMutex);
  
  DEBUG_STRING("Cleaning up drone references");

  DroneIndex_Set::index<DI_INDEX>::type& indexSet = droneIndices.get<DI_INDEX>();
  for (DroneIndex_Set::index<DI_INDEX>::type::iterator it = indexSet.begin(); it != indexSet.end(); it++) {
    env->DeleteGlobalRef(it->javaObjectRef);
  }
  droneIndices.clear();
}


JNIEXPORT jobjectArray JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_SimulatedDrone_runLuaCode0(JNIEnv* env, jobject obj, jstring code) {
  lock_guard<recursive_mutex> odeLock(odeMutex);
  DEBUG_STRING("-> SimulatedDrone.runLuaCode0");
  
  if (code == NULL) {
    return NULL;
  }

  int droneIndex = getDroneIndex(env, obj);
  DroneIndexStruct droneIndexStruct = getDroneIndexStruct(droneIndex);
  if (droneIndexStruct.droneRef == NULL) {
    cerr << "Drone object corresponding to java object not found" << endl;
    return NULL;
  }

  DEBUG_STRING("Calling parrot lua function");
  
  // Prepare code string
  const char* rawCodeString = env->GetStringUTFChars(code, NULL);
  if (rawCodeString == NULL) {
    return NULL;
  }
  int codeStringLength = env->GetStringLength(code);
  char* codeString = new char[codeStringLength + 1];
  memcpy(codeString, rawCodeString, codeStringLength);
  codeString[codeStringLength] = 0;
  env->ReleaseStringUTFChars(code, rawCodeString);

  DEBUG_STRING("  Using code: " << codeString);
  
  // Call function
  LuaReferenceStruct luaRefStruct;
  droneIndexStruct.droneRef->lockLua(luaRefStruct);
  lua_State* lua = luaRefStruct.lua;
  
  int stacktop = lua_gettop(lua);
  luaL_dostring(lua, codeString);
  delete[] codeString;
  
  // Analyze results and put them into object array
  DEBUG_STRING("Re-encoding results to Java types");
  
  int paramCount = lua_gettop(lua) - stacktop;
  jobjectArray result = encodeLuaResults(env, lua, paramCount);

  lua_settop(lua, stacktop);
  
  DEBUG_STRING("<- SimulatedDrone.runLuaCode0");

  return result;
}
  

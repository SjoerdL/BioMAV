#include "nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick.h"

#include <iostream>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <SDL/SDL.h>

#include "../joystick/ParrotJoystick.hpp"

#define DEBUG_STRING(s) cout << s << endl;

using namespace std;
using namespace boost;

recursive_mutex joystickMutex;
shared_ptr<ParrotJoystick> parrotJoystickPointer;

class SDLJoystickThread {
  private:
    thread sdlThread;
    int joystickIndex;

    bool initialized;
    bool failed;
  public:
    virtual void operator()() {
      SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTTHREAD);
      {
        lock_guard<recursive_mutex> lock(joystickMutex);
        try {
          parrotJoystickPointer = shared_ptr<ParrotJoystick>(new ParrotJoystick(joystickIndex));
          if (!parrotJoystickPointer->loadSettings("joystick.cfg")) {
            parrotJoystickPointer->calibrateAndAssociate();
            parrotJoystickPointer->saveSettings("joystick.cfg");
          }
        }
        catch (string& s) {
          cerr << "Error: " << s << endl;
          failed = true;
          return;
        }
      }

      initialized = true;
      while (!sdlThread.interruption_requested()) {
        {
          lock_guard<recursive_mutex> lock(joystickMutex);
          parrotJoystickPointer->update();
        }
        SDL_Delay(10);
      }

      {
        lock_guard<recursive_mutex> lock(joystickMutex);
        parrotJoystickPointer.reset();
      }

      SDL_Quit();
    }

    SDLJoystickThread(int i) {
      joystickIndex = i;
      initialized = false;
      failed = false;

      sdlThread = thread(ref(*this));
      while ((!initialized) && (!failed)) {
      }

      if (failed) {
        sdlThread.join();
        throw string("Cannot initialize joystick");
      }
    }

    virtual ~SDLJoystickThread() {
      sdlThread.interrupt();
      sdlThread.join();
    }
};

shared_ptr<SDLJoystickThread> joystickThread;

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_initJoystick0(JNIEnv *, jclass, jint i) {
  try {
    joystickThread = shared_ptr<SDLJoystickThread>(new SDLJoystickThread(i));
  }
  catch (string& s) {
    cerr << "Error: " << s << endl;
    return false;
  }
  return true;
}

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_getOptionButton0(JNIEnv *, jclass, jint i) {
  lock_guard<recursive_mutex> lock(joystickMutex);
  if (i == 0) {
    return parrotJoystickPointer->optionalButton1();
  }
  if (i == 1) {
    return parrotJoystickPointer->optionalButton2();
  }
  return false;
}

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_getStartMode0(JNIEnv *, jclass) {
  lock_guard<recursive_mutex> lock(joystickMutex);
  return parrotJoystickPointer->doStart();
}

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_getEmergencyMode0(JNIEnv *, jclass) {
  lock_guard<recursive_mutex> lock(joystickMutex);
  return parrotJoystickPointer->isEmergency();
}

JNIEXPORT jfloat JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_getPitch0(JNIEnv *, jclass) {
  lock_guard<recursive_mutex> lock(joystickMutex);
  return parrotJoystickPointer->getForwardSetting();
}

JNIEXPORT jfloat JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_getYaw0(JNIEnv *, jclass) {
  lock_guard<recursive_mutex> lock(joystickMutex);
  return parrotJoystickPointer->getSpinSetting();
}

JNIEXPORT jfloat JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_getRoll0(JNIEnv *, jclass) {
  lock_guard<recursive_mutex> lock(joystickMutex);
  return parrotJoystickPointer->getStrafeSetting();
}

JNIEXPORT jfloat JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_getHeight0(JNIEnv *, jclass) {
  lock_guard<recursive_mutex> lock(joystickMutex);
  return parrotJoystickPointer->getHeightSetting();
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_SDLDroneJoystick_shutdownJoystick0(JNIEnv *, jclass) {
  shared_ptr<SDLJoystickThread> tmpJTPtr;
  {
    lock_guard<recursive_mutex> lock(joystickMutex);
    tmpJTPtr = joystickThread;
    joystickThread.reset();
  }
}

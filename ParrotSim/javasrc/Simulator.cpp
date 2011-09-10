#include "Simulator.hpp"
#include "SharedJNIData.hpp"

#include <cstring>

#include <iostream>
#include <vector>
#include <map>

#ifdef _WIN32
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL/SDL.h>
#include <IL/il.h>
#include <IL/ilut.h>

#include <Simulator.hpp>
#include <Parrot.hpp>
#include <Texture.hpp>

#include "Lua2JavaTypeConversion.hpp"

#include "CustomizedSimulatedDrone.hpp"

#ifdef DEBUG
#define STRINGIFY1(x) #x
#define STRINGIFY2(x) STRINGIFY1(x)
#define DEBUG_STRING(s) cerr << __FILE__ ":" STRINGIFY2(__LINE__) " " s << endl;
#else
#define DEBUG_STRING(s)
#endif 

#define LOOP_DEBUG_STRING(x) //DEBUG_STRING(x)

#define WINDOW_WIDTH  FRAME_WIDTH
#define WINDOW_HEIGHT FRAME_HEIGHT
#define WINDOW_NAME   "Parrot AR Simulator"

#define MIN(x, y) ( ((x) < (y)) ? (x) : (y) )
#define MAX(x, y) ( ((x) > (y)) ? (x) : (y) )

using namespace std;
using namespace boost;

recursive_mutex odeMutex;

struct DroneImageData {
  int width, height;
  unsigned char* data;
  unsigned char* nonSharedData;
  jobject javaByteBuffer;
};

typedef vector<DroneImageData> DroneImageDataVector; 
typedef vector<VirtualParrotDrone*> DroneVector;
typedef map< VirtualParrotDrone*, DroneImageDataVector > DroneImageMap;

JavaVM* javaVM = NULL;
jobject javaSimulatorObject = NULL;

SDL_mutex* simulatorMutex = NULL;
Simulation* simulator = NULL;

SDL_Thread* simulationThread = NULL;

mutex threadReadySemaphoreMutex;
condition_variable threadReadySemaphore;

//// Shared data ////

// Initialized during thread initialization
DroneVector drones;

// Continously updated (using the given mutex)
shared_mutex sharedDataMutex;
bool coupleTime = false;
bool activateOffscreenRendering = true;
long lastRenderTimestamp = -1;
long lastTimestamp = -1;
DroneImageMap droneImages;
bool runSimThread = false;
bool simulator_useSDL = false;

void addNewDroneWithImageBuffer(JNIEnv* env, VirtualParrotDrone* drone) {
  for (DroneVector::iterator it = drones.begin(); it != drones.end(); it++) {
    if (*it == drone) {
      return;
    }
  }

  DEBUG_STRING("  -> Adding new drone " << drone);

  drones.push_back(drone);

  vector<DroneImageData> thisDroneImages;
  for (int j = 0; j < drone->getCameraCount(); j++) {
    DEBUG_STRING("     -> adding image " << j);
    DroneImageData imgData;
    drone->getCameraResolution(j, imgData.width, imgData.height);
    imgData.data = new unsigned char[imgData.width * imgData.height * 3];
    imgData.nonSharedData = new unsigned char[imgData.width * imgData.height * 3];

    jobject jbb = env->NewDirectByteBuffer(imgData.nonSharedData, 3 * imgData.width * imgData.height);
    imgData.javaByteBuffer = env->NewGlobalRef(jbb);
    env->DeleteLocalRef(jbb);

    thisDroneImages.push_back(imgData);
  }
  droneImages[drone] = thisDroneImages;
}

void cleanDroneImageBuffers() {
  DroneVector keysToDelete;
  for (DroneImageMap::iterator it = droneImages.begin(); it != droneImages.end(); it++) {
    bool doDelete = true;
    
    for (DroneVector::iterator it2 = drones.begin(); it2 != drones.end(); it2++) {
      if (it->first == *it2) {
        doDelete = false;
      }
    }
    
    if (doDelete) {
      keysToDelete.push_back(it->first);
    }
  }
  
  LOOP_DEBUG_STRING("Deleting " << keysToDelete.size() << " drones");
  for (DroneVector::iterator it = keysToDelete.begin(); it != keysToDelete.end(); it++) {
    droneImages.erase(*it);
  }
}

bool initializeSDL() {
  DEBUG_STRING("Initializing SDL...");
  if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 0) {
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_OPENGL);
    SDL_WM_SetCaption(WINDOW_NAME, NULL);

    #ifdef WIN32
    DEBUG_STRING("Initializing GLEW...");
    glewInit();
    #endif

    ilutInit();
    activateTextureObjects(true);

    return true;
  }
  return false;
}

bool deinitializeSDL() {
  DEBUG_STRING("Deinitialize SDL");
  activateTextureObjects(false);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  return true;
}

int simulationMainLoop(JNIEnv* env, unique_lock<mutex>& trsLock) {
  bool SDLinitialized = false;
  activateTextureObjects(false);
  
  DEBUG_STRING("Thread started");

  if (!runSimThread) {
    DEBUG_STRING("Thread terminated because runSimThread != true");
    trsLock.unlock();
    threadReadySemaphore.notify_all();
    return -2;
  }

  DEBUG_STRING("Initializing ODE...");
  dInitODE();

  DEBUG_STRING("Initializing IL...");
  ilInit();
  ilutInit();

  DEBUG_STRING("Initializing Simulator...");
  try {
    simulator = new Simulation();
  }
  catch (string s) {
    cerr << "Exception during create simulation: " << s << endl;
    runSimThread = false;
    simulator = NULL;      

    dCloseODE();
    ilShutDown();
    deinitializeSDL();

    trsLock.unlock();
    threadReadySemaphore.notify_all();
    
    return -1;      
  }

  bool localDoRun = true;
  jobject externalTimerSynchronizer = NULL;
  jmethodID externalTimerUpdateMethodID = NULL;
  
  jmethodID notifyNewSensorDataMethodID = NULL;

  SDL_LockMutex(simulatorMutex);
  {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);

    DEBUG_STRING("Setting initial parameters...");
    coupleTime = simulator->getCoupleTime();
    lastRenderTimestamp = simulator->getLastRenderTimestamp();

    DEBUG_STRING(" -> collecting images & drones");
    for (int i = 0; i < simulator->getDroneCount(); i++) {
      DEBUG_STRING("    -> Collecting drone " << i + 1 << " of " << simulator->getDroneCount());
      addNewDroneWithImageBuffer(env, simulator->getDrone(i)); 
    }

    localDoRun = runSimThread;
    if (env != NULL) {
      {
        // External timer stuff
        jclass simulatorClass = env->GetObjectClass(javaSimulatorObject);
        jfieldID externalTimerField = env->GetFieldID(simulatorClass, "externalTimerSynchronizer", "Lnl/ru/ai/projects/parrot/dronecontrol/simulator/ExternalTimerSynchronizer;");
        externalTimerSynchronizer = env->GetObjectField(javaSimulatorObject, externalTimerField);
        
        if (externalTimerSynchronizer != NULL) {
          jclass externalTimerSyncClass = env->GetObjectClass(externalTimerSynchronizer);
          externalTimerUpdateMethodID = env->GetMethodID(externalTimerSyncClass, "externalTimerUpdate", "(J)V");
        } else {
          cerr << "Error! Could retrieve ExternalTimerSynchronizer object from Simulator field" << endl;
        }
      }
      
      {
        // Notify new sensor data stuff
        jclass simulatedDroneClass = env->FindClass("nl/ru/ai/projects/parrot/dronecontrol/simulator/SimulatedDrone");
        notifyNewSensorDataMethodID = env->GetMethodID(simulatedDroneClass, "notifyNewSensorData", "()V");
        if (notifyNewSensorDataMethodID == NULL) {
          cerr << "Error! Could retrieve method ID of SimulatedDrone.notifyNewSensorData()" << endl;
        }
      } 
    } else {
      cerr << "Error - could not attach simulation thread to Java VM" << endl;
      localDoRun = false;
    }

    DEBUG_STRING("Thread signaling ready");
    threadReadySemaphore.notify_all();
    trsLock.unlock();
  }
  SDL_UnlockMutex(simulatorMutex);

  DEBUG_STRING("Starting main loop");
  // Mainloop
  bool simulating = true;
  while (simulating && (localDoRun)) {
    LOOP_DEBUG_STRING("Start of main loop");

    {
      SDL_LockMutex(simulatorMutex);
      LOOP_DEBUG_STRING("Running simulation step");
      long lastSimTimeStamp = simulator->getLastSimulationTimestamp();
      {
        lock_guard<recursive_mutex> odeLock(odeMutex);
        simulating = simulator->simulate();
      }
      
      LOOP_DEBUG_STRING("Calling drone update functions");
      callAllDroneUpdateFunctions(env, notifyNewSensorDataMethodID);
      if (lastSimTimeStamp != simulator->getLastSimulationTimestamp()) {
        LOOP_DEBUG_STRING("Updating Java external timer");
        env->CallVoidMethod(externalTimerSynchronizer, externalTimerUpdateMethodID, (long) simulator->getLastSimulationTimestamp());
      }
      
      {
        lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);

        LOOP_DEBUG_STRING("Adding new drones");
        for (int i = 0; i < simulator->getDroneCount(); i++) {
          addNewDroneWithImageBuffer(env, simulator->getDrone(i)); 
        }
        cleanDroneImageBuffers();
      }
      SDL_UnlockMutex(simulatorMutex);
    }
    
    if (SDLinitialized) { 
      // Poll and handle SDL events if SDL initialized
      SDL_Event event;

      while (SDL_PollEvent(&event)) {
        SDL_LockMutex(simulatorMutex);
        simulator->processSDLEvent(&event);
        SDL_UnlockMutex(simulatorMutex);
      }
      LOOP_DEBUG_STRING("SDL Poll done");

      // Render simulator contents
      SDL_LockMutex(simulatorMutex);
      simulator->draw();
      SDL_UnlockMutex(simulatorMutex);
      LOOP_DEBUG_STRING("simulator->draw() done");
    }

    SDL_LockMutex(simulatorMutex);
    {
      lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    
      // Update shared values
      if (coupleTime != simulator->getCoupleTime()) {
        simulator->setCoupleTime(coupleTime);
      }
      lastRenderTimestamp = simulator->getLastRenderTimestamp();
      lastTimestamp = simulator->getLastSimulationTimestamp();
      SDL_UnlockMutex(simulatorMutex);
      LOOP_DEBUG_STRING("Render timetamps updated");

      // Copy images
      if (SDLinitialized) {
        SDL_LockMutex(simulatorMutex);
        if (activateOffscreenRendering) {
          LOOP_DEBUG_STRING("Copying image data");

          for (DroneVector::iterator drone_it = drones.begin(); drone_it != drones.end(); drone_it++) {
            DroneImageDataVector thisDroneImages = droneImages[*drone_it];
            
            // Find drone index
            int droneIndex = -1;
            for (int di = 0; di < drones.size(); di++) {
              if (simulator->getDrone(di) == *drone_it) {
                droneIndex = di;
                break;
              }
            }
            
            if (droneIndex >= 0) {
              int imageIndex = 0;
              for (DroneImageDataVector::iterator it = thisDroneImages.begin(); it != thisDroneImages.end(); it++) {
                ILuint img = simulator->getDroneImage(droneIndex, imageIndex);
                ilBindImage(img);

                ILubyte* srcImgData = ilGetData();

                int imgWidth = MIN(it->width, ilGetInteger(IL_IMAGE_WIDTH));
                int imgHeight = MIN(it->height, ilGetInteger(IL_IMAGE_HEIGHT));
                if (imgWidth > 0) {
                  for (int y = 0; y < imgHeight; y++) {
                    memcpy(it->data + y * imgWidth * 3, srcImgData + (imgHeight - 1 - y) * imgWidth * 3, imgWidth * 3);
                  }
                }
                
                imageIndex++;
              }
            }
          }

          SDL_UnlockMutex(simulatorMutex);
          LOOP_DEBUG_STRING("Copying image data done");
        }
        simulator->activateOffscreenRendering(activateOffscreenRendering);
      }

      // Update run info
      localDoRun = runSimThread;
      LOOP_DEBUG_STRING("Run info updated");

      // Activate or deactivate SDL if requested
      if (simulator_useSDL && (!SDLinitialized)) {
        if (initializeSDL()) {
          SDL_LockMutex(simulatorMutex);
          simulator->enableGL(true);
          SDL_UnlockMutex(simulatorMutex);
          
          SDLinitialized = true;
        } else {
          simulator_useSDL = false;
          cerr << "Error! Could not initialize SDL" << endl;
        }
      } 
      if ((!simulator_useSDL) && SDLinitialized) {
        SDL_LockMutex(simulatorMutex);
        simulator->enableGL(false);
        deinitializeSDL();
        SDL_UnlockMutex(simulatorMutex);
        
        SDLinitialized = false;
      }
    }
  }

  while (localDoRun) {
    SDL_Delay(100);

    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    localDoRun = runSimThread;
  }

  DEBUG_STRING("Destroying drone images (copies and Java Native Buffers)");
  drones.clear();
  for (DroneImageMap::iterator it = droneImages.begin(); it != droneImages.end(); it++) {
    DroneImageDataVector imageDataVector = it->second; 
    for (DroneImageDataVector::iterator it2 = imageDataVector.begin(); it2 != imageDataVector.end(); it2++) {
      if (it2->data != NULL) {
        delete[] it2->data;
      }
      if (it2->nonSharedData != NULL) {
        delete[] it2->nonSharedData;
      }
      if (it2->javaByteBuffer != NULL) {
        env->DeleteGlobalRef((jobject) it2->javaByteBuffer);
        it2->javaByteBuffer = NULL;
      }
    }
  }
  droneImages.clear();

  if (simulator != NULL) {
    DEBUG_STRING("Destroying simulator object");
  
    delete simulator;
    simulator = NULL;
  }

  DEBUG_STRING("Cleaning Java drone references");
  cleanupDroneReferences(env);

  // Shutdown SDL (and other libraries)
  DEBUG_STRING("Shutting down libraries");
  
  DEBUG_STRING("Shutting down il");
  ilShutDown();

  if (SDLinitialized) {
    SDL_LockMutex(simulatorMutex);
    deinitializeSDL();
    SDL_UnlockMutex(simulatorMutex);
  
    SDLinitialized = false;
  }

  DEBUG_STRING("Shutting down ode");
  dCloseODE();

  DEBUG_STRING("Simulation loop finished!");

  return 0;
}

int simulationMainThread(void* userdata) {
  unique_lock<mutex> trsLock(threadReadySemaphoreMutex);
  
  DEBUG_STRING("Attaching simulation thread to Java VM");
  JNIEnv* env = NULL;
  javaVM->AttachCurrentThread((void**) &env, NULL);
  if (env == NULL) {
    cerr << "Could not attach thread to Java VM" << endl;
    return -3;
  }

  int res = simulationMainLoop(env, trsLock);

  DEBUG_STRING("Detaching simulation thread from Java VM");
  javaVM->DetachCurrentThread();
  env = NULL;
  
  return res;
}



/********* Java implementations *********/

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_runSimulator0(JNIEnv* env, jclass, jobject obj) {
  unique_lock<mutex> trsLock(threadReadySemaphoreMutex);
  runSimThread = true;

  if (SDL_InitSubSystem(SDL_INIT_EVENTTHREAD) != 0) {
    runSimThread = false;
    return false;
  }
  
  jclass cls = env->GetObjectClass(obj);
  env->GetJavaVM(&javaVM);
  javaSimulatorObject = env->NewGlobalRef(obj);
  
  DEBUG_STRING("Cleaning things for initialization");
  drones.clear();
  droneImages.clear();

  {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    
    DEBUG_STRING("Starting thread");
    // Create thread, initialize libraries there and wait for initialization to be finished!
    simulatorMutex = SDL_CreateMutex();
    simulationThread = NULL;
  }

  simulationMainLoop(env, trsLock);
}

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_waitReady0
  (JNIEnv* env, jclass cls) {
  DEBUG_STRING("Locking thread ready semaphore");
  unique_lock<mutex> trsLock(threadReadySemaphoreMutex);
/*  DEBUG_STRING("Waiting for thread ready..."); Not necessary since threadReadySemaphore is continuously locked if runSimulator is called)
  threadReadySemaphore.wait(trsLock);*/
  
  DEBUG_STRING("Locking shared data mutex");
  lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
  DEBUG_STRING("Returning result");
  return runSimThread;
}

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_initSimulator
  (JNIEnv* env, jclass , jobject obj) {
  unique_lock<mutex> trsLock(threadReadySemaphoreMutex);
  runSimThread = true;

  if (SDL_InitSubSystem(SDL_INIT_EVENTTHREAD) != 0) {
    runSimThread = false;
    return false;
  }
  
  jclass cls = env->GetObjectClass(obj);
  env->GetJavaVM(&javaVM);
  javaSimulatorObject = env->NewGlobalRef(obj);
  
  DEBUG_STRING("Cleaning things for initialization");
  drones.clear();
  droneImages.clear();

  {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    
    DEBUG_STRING("Starting thread");
    // Create thread, initialize libraries there and wait for initialization to be finished!
    runSimThread = true;
    simulatorMutex = SDL_CreateMutex();
    simulationThread = NULL;
  }

  simulationThread = SDL_CreateThread(&simulationMainThread, NULL);
  DEBUG_STRING("Waiting for thread to be ready");
  threadReadySemaphore.wait(trsLock);
  DEBUG_STRING("Thread is up and running");

  if (runSimThread) {
  } else {
    DEBUG_STRING("But an error ocurred!!!");
//    jmethodID metID = env->GetMethodID(cls, "cleanupSimulator", "()V");
//    if (metID == NULL) {
//      cerr << "Error! metID in Simulator.initSimulator is NULL" << endl;
      // HOWEVER, THE FUNCTION MUST BE CALLED!!!
    Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_cleanupSimulator(env, cls);
//    } else {
//      env->CallStaticVoidMethod(cls, metID);
//    }
    return false;
  }
  return true;
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_cleanupSimulator
  (JNIEnv* env, jclass cls) {
  DEBUG_STRING("Locking and setting runSimThread to false");
  {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    runSimThread = false;
  }
  if (simulationThread != NULL) {
    DEBUG_STRING("Wait for simulation thread to complete");
    SDL_WaitThread(simulationThread, 0);
    simulationThread = NULL;
  } else {
    DEBUG_STRING("No simulator thread in use");
  }
  
  DEBUG_STRING("Freeing Java VM objects");
  env->DeleteGlobalRef(javaSimulatorObject);
  javaSimulatorObject = NULL;
  javaVM = NULL;

  DEBUG_STRING("Destroying mutexes");
  SDL_DestroyMutex(simulatorMutex);
  
  simulatorMutex = NULL;
  
  DEBUG_STRING("Shutting down SDL EVENTTHREAD");
  SDL_QuitSubSystem(SDL_INIT_EVENTTHREAD);
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_setCoupleTime0
  (JNIEnv* env, jclass cls, jboolean b) {
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    coupleTime = b;
  }
}

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_getCoupleTime0
  (JNIEnv* env, jclass cls) {
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    bool res = false;
    if (runSimThread) {
      res = coupleTime;
    }
    return res;
  }
  return false;
}

JNIEXPORT jlong JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_getLastRenderTimeStamp0
  (JNIEnv* env, jclass cls) {
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    long res = -1;
    if (runSimThread) {
      res = lastRenderTimestamp;
    }
    return res;
  }
  return -1;
}

JNIEXPORT jlong JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_getCurrentTimeStamp0(JNIEnv * env, jclass cls) {
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    long res = -1;
    if (runSimThread) {
      res = lastTimestamp;
    }
    return res;
  }
  return -1;
}

JNIEXPORT jobjectArray JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_getDrones0
  (JNIEnv* env, jclass) {
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    jobjectArray res = NULL;
    if (runSimThread) {
      DEBUG_STRING("Returning drones");
      jclass virtualParrotDroneClass = env->FindClass("nl/ru/ai/projects/parrot/dronecontrol/simulator/SimulatedDrone");
      
      res = env->NewObjectArray(drones.size(), virtualParrotDroneClass, NULL);
      if (res != NULL) {
        int i = 0;
        for (DroneVector::iterator it = drones.begin(); it != drones.end(); it++) {
          jobject droneObj = getDroneIndexStruct(env, *it).javaObjectRef;
          env->SetObjectArrayElement(res, i++, droneObj);
        }
      } else {
        cerr << "Error! Could not create java array" << endl;
      }
    }
    return res;
  }
  return NULL;
}

JNIEXPORT jobject JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_getImage0
  (JNIEnv* env, jclass cls, jint droneIndex, jint imageIndex) {
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);

    jobject res = NULL;
    unsigned char* nsData = NULL;
    int size = 0;

    if (runSimThread) {
      VirtualParrotDrone* virtualParrotDrone = getDroneIndexStruct(droneIndex).droneRef;
      if (virtualParrotDrone != NULL) {
        DroneImageDataVector imageDataVector = droneImages[virtualParrotDrone];
        if ((imageIndex >= 0) && (imageIndex < imageDataVector.size())) {
          size = 3 * imageDataVector[imageIndex].width * imageDataVector[imageIndex].height;
          res = imageDataVector[imageIndex].javaByteBuffer;
          memcpy(imageDataVector[imageIndex].nonSharedData, imageDataVector[imageIndex].data, size);
        }
      } else {
        cerr << "Virtual parrot drone with RefID " << droneIndex << " is null" << endl;
      }
    }

    return res;
  }
  return NULL;
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_activateVideoInterface0(JNIEnv* env, jclass cls, jboolean b) {
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    activateOffscreenRendering = b;
  }
}

JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_activateRendering0(JNIEnv* env, jclass cls, jboolean b) {
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    simulator_useSDL = b;
  }
}

JNIEXPORT jboolean JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_isRenderingActivated0
(JNIEnv* env, jclass cls) {
  bool res = false;
  if (simulator != NULL) {
    lock_guard<shared_mutex> sharedDataLock(sharedDataMutex);
    res = simulator_useSDL;
  }
  return res;
}

JNIEXPORT jobjectArray JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_simulator_Simulator_runLuaCode0(JNIEnv* env, jclass cls, jstring code) {
  if (code == NULL) {
    return NULL;
  }

  if (simulator != NULL) {
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

    // Call function
    SDL_LockMutex(simulatorMutex);
    
    lua_State* lua = simulator->getLua();
    
    int stacktop = lua_gettop(lua);
    luaL_dostring(lua, codeString);
    delete[] codeString;
    
    // Analyze results and put them into object array
    int paramCount = lua_gettop(lua) - stacktop;
    jobjectArray result = encodeLuaResults(env, lua, paramCount);

    lua_settop(lua, stacktop);
    SDL_UnlockMutex(simulatorMutex);
    
    return result;
  }

  return NULL;
}



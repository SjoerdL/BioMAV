#include "Simulator.hpp"

#include <cstdio>

#include <iostream>
#include <sstream>

#include "LuaToolbox.hpp"

#include "WorldBlock.hpp"

#define SIMULATION_GRAIN 0.01
#define FRAME_RATE 20

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#ifdef DEBUG
#define STRINGIFY1(x) #x
#define STRINGIFY2(x) STRINGIFY1(x)
#define DEBUG_STRING(s) cerr << __FILE__ ":" STRINGIFY2(__LINE__) " " s << endl;
#else
#define DEBUG_STRING(s)
#endif 

using namespace std;

void Simulation :: drawSequence() {
  DEBUG_STRING("Draw sequence");
  {
    GLfloat pos[4] = {-1, 0, 2.5, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
  }

  world.draw();

  glFinish();
}

Simulation :: Simulation() {
  stopSimulation = false;
  glEnabled = false;

  startTime = SDL_GetTicks();

  timeDrift = 0;
  frameRateDrift = 0;

  coupleTime = true;
  offscreenRendering = true;

  renderTimeStamp = -1;
  simulationTimeStamp = 0;

  // Setup cameras
  views[0].camera.eye();
  views[0].fov = 90;
  views[0].aspect = 1;

  views[1].camera.eye();
  views[1].fov = 90;
  views[1].aspect = 1;

  views[2].camera.eye();
  views[2].fov = 90;
  views[2].aspect = 1;

  // Setup drone
  drone = new VirtualParrotDrone();
  world.addWorldElement(drone);

  {
    std::vector<ILuint> droneImages;
    for (int i = 0; i < drone->getCameraCount(); i++) {
      droneImages.push_back(ilGenImage());
    }

    cameraImages.push_back(droneImages);
  }

  DEBUG_STRING("Creating lua environment (simulator)");

  lua = NULL;
  lua = luaL_newstate();
  if (lua != NULL) {
    luaL_openlibs(lua);
    tolua_SimulationEnv_open(lua);
  } 
  if (lua == NULL) {
    cerr << "Warning! Could not create lua simulation context" << endl;
  }

  if (lua != NULL) {
    tolua_pushusertype(lua, &world, "ParrotWorld");
    lua_setglobal(lua, "world");

    tolua_pushusertype(lua, drone, "VirtualParrotDrone");
    lua_setglobal(lua, "drone");
    drone->routeScriptLuaFunctions(lua);

    lua_pushnumber(lua, MOTOR_DEFAULT_FORCE);
    lua_setglobal(lua, "MOTOR_DEFAULT_FORCE");

    lua_pushnumber(lua, DRONE_LENGTH);
    lua_setglobal(lua, "DRONE_LENGTH");

    lua_pushnumber(lua, DRONE_WIDTH);
    lua_setglobal(lua, "DRONE_WIDTH");

    lua_pushinteger(lua, 0);
    lua_setglobal(lua, "LEFT");

    lua_pushinteger(lua, 1);
    lua_setglobal(lua, "RIGHT");

    lua_pushinteger(lua, 0);
    lua_setglobal(lua, "FRONT");

    lua_pushinteger(lua, 1);
    lua_setglobal(lua, "BACK");

    DEBUG_STRING("Running simulator init script");
    runScript(lua, "simulation.lua");
  }
}

Simulation :: ~Simulation() {
  DEBUG_STRING("Shutting down simulator lua environment");
  if (lua != NULL) {
    lua_close(lua);
    lua = NULL;
  }

  DEBUG_STRING("Cleaning up IL images");
  for (vector< vector<ILuint> >::iterator it1 = cameraImages.begin(); it1 < cameraImages.end(); it1++) {
    for (vector<ILuint>::iterator it2 = (*it1).begin(); it2 < (*it1).end(); it2++) {
      ilDeleteImage(*it2);
    }
  }
  cameraImages.clear();

  DEBUG_STRING("~Simulation done");
}

void Simulation :: setCoupleTime(bool c) {
  coupleTime = c;
}

bool Simulation :: getCoupleTime() const {
  return coupleTime;
}

bool Simulation :: simulate() {
  DEBUG_STRING("Making simulation step");

  // Make time coupling (if activated)
  if (coupleTime) {
    timeDrift += (double) (SDL_GetTicks() - startTime) / 1000.0;
    if (timeDrift > 1 * SIMULATION_GRAIN) {
      timeDrift = 1 * SIMULATION_GRAIN; // Limit time drift if PC is too slow
    }
  } else {
    timeDrift = 0;
  }
  startTime = SDL_GetTicks();

  if (timeDrift >= 0) {
    // Try to call "beforeStep"
    if (lua != NULL) {
      lua_gc(lua, LUA_GCRESTART, 0);
      lua_gc(lua, LUA_GCCOLLECT, 1);

      int stackPosition = lua_gettop(lua);
      
      lua_getglobal(lua, "beforeStep");
      if (lua_isfunction(lua, -1)) {
        lua_pushnumber(lua, SIMULATION_GRAIN);

        string errorMessage;
        if (!runFunction(lua, 1, 0, errorMessage)) {
          cout << " -> " << errorMessage << endl;
        }
      } else {
        lua_pop(lua, 1);
      }
      
      lua_settop(lua, stackPosition);
    }

    simulationTimeStamp += SIMULATION_GRAIN;

    frameRateDrift += SIMULATION_GRAIN;
    if (frameRateDrift > 2 * (1.0 / FRAME_RATE)) {
      frameRateDrift > 2 * (1.0 / FRAME_RATE); // Limit also the frame rate drift that is bound to simulation time
    }

    world.stepWorld(SIMULATION_GRAIN);    

    timeDrift -= SIMULATION_GRAIN;

    if (lua != NULL) {
      int stackPosition = lua_gettop(lua);
      lua_getglobal(lua, "afterStep");
      if (lua_isfunction(lua, -1)) {
        lua_pushnumber(lua, SIMULATION_GRAIN);

        string errorMessage;
        if (!runFunction(lua, 1, 0, errorMessage)) {
          cout << " -> " << errorMessage << endl;
        }
      } else {
        lua_pop(lua, 1);
      }
      lua_settop(lua, stackPosition);
    }
  }

  return !stopSimulation;
}

void Simulation :: draw() {
  if (!glEnabled) {
    return;
  }

  DEBUG_STRING("Drawing");

  if (frameRateDrift > 0) {
    frameRateDrift -= 1.0 / FRAME_RATE;

    // Setup render parameters
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 1, 0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_NORMALIZE);
    
    glEnable(GL_TEXTURE_2D);

    // Print warning if viewport dimensions might be insufficient!
    int widthheight[2];
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, widthheight);
    if ((widthheight[0] < 1024) || (widthheight[1] < 1024)) {
      cerr << "Warning!!! MAX_VIEWPORT_DIMS: " << widthheight[0] << "; " << widthheight[1] << " have to be >= 1024 each" << endl;
    }

    // Draw draw offscreen images
    if (offscreenRendering) {
      DEBUG_STRING("Drawing offscreen images");
      for (int droneIndex = 0; droneIndex < 1; droneIndex++) {
        for (int viewIndex = 0; viewIndex < drone->getCameraCount(); viewIndex++) {
          int width, height;
          drone->getCameraResolution(viewIndex, width, height);
          glViewport(0, 0, width, height);

          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          gluPerspective(90, (float) width / (float) height, 0.01, 100);

          glMatrixMode(GL_MODELVIEW);
          Matrix viewMatrix;
          drone->getCameraTransformation(viewIndex, viewMatrix);
          glLoadMatrixf(viewMatrix.getValues());

          drawSequence();

          if (viewIndex < drone->getCameraCount()) {
            if (cameraImages.size() < 1) {
              cerr << "INTERNAL ERROR : Too few camera images!??" << endl;
            } else {
              ilBindImage(cameraImages[0][viewIndex]);
              ilutGLScreen();
            }
          }
        }
      }
    }

    DEBUG_STRING("Drawing screen image");

    // Draw screen image
    for (int i = 0; i < MIN(2, drone->getCameraCount()); i++) {
      drone->getCameraTransformation(i, views[i].camera); 
    }

    DEBUG_STRING("Got camera transformations");

    {
      Vector pos = drone->getPosition();
      views[2].camera.setViewMatrix(Vector(-3, -2, 2), pos, Vector(0, 0, 1));
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int view_i = 0; view_i < VIEW_COUNT; view_i++) {
      DEBUG_STRING("Switching to view " << view_i);
      switch (view_i) {
        case 0:
          glViewport(FRAME_WIDTH * 2 / 3, 0, FRAME_WIDTH / 3, FRAME_HEIGHT / 2);
          break;
        case 1:
          glViewport(FRAME_WIDTH * 2 / 3, FRAME_HEIGHT / 2, FRAME_WIDTH / 3, FRAME_HEIGHT / 2);
          break;
        case 2:
          glViewport(0, 0, FRAME_WIDTH * 2 / 3, FRAME_HEIGHT);
          break;
      }

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(views[view_i].fov, views[view_i].aspect, 0.01, 100);

      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(views[view_i].camera.getValues());

      drawSequence();
    }

    SDL_GL_SwapBuffers();

    renderTimeStamp = 1000 * simulationTimeStamp;
  } else {
//    SDL_GL_SwapBuffers();// <- do not do this... (probably) causes graphics glitches when flipping, 
                         //    is slow, and the screen is already redrawn often enough 
  }
}

int Simulation :: getDroneCount() {
  return 1;
}

VirtualParrotDrone* Simulation :: getDrone(int i) {
  if (i == 0) {
    return drone;
  }
  return NULL;
}

int Simulation :: getDroneImageCount(int i) {
  if (i == 0) {
    return drone->getCameraCount();
  }

  return -1;
}

ILuint Simulation :: getDroneImage(int i, int j) {
  if (i == 0) {
    if ((j >= 0) && (j < cameraImages.size())) {
      return cameraImages[i][j];
    }
  }

  return 0;
}

void Simulation :: enableGL(bool b) {
  if (glEnabled != b) {
    glEnabled = b;
    world.enableGL(b);
  }
}

void Simulation :: activateOffscreenRendering(bool b) {
  offscreenRendering = b;
}

long Simulation :: getLastRenderTimestamp() {
  return renderTimeStamp;
}

long Simulation :: getLastSimulationTimestamp() {
  return 1000 * simulationTimeStamp;
}

lua_State* Simulation :: getLua() {
  return lua;
}

void Simulation :: processSDLEvent(SDL_Event* event) {
  if (event->type == SDL_QUIT) {
    stopSimulation = true;
  }
}



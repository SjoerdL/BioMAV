#include <iostream>

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

#include "Texture.hpp"
#include "Simulator.hpp"

using namespace std;

#define WINDOW_WIDTH  FRAME_WIDTH
#define WINDOW_HEIGHT FRAME_HEIGHT
#define WINDOW_NAME   "Parrot AR Simulator"

int main(int args, char** argv) {
  #ifdef WIN32
  freopen("CON", "w", stdout);
  freopen("CON", "w", stderr);
  #endif

  if (SDL_Init(SDL_INIT_VIDEO) == 0) {
    // Setup SDL
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_OPENGL);
    SDL_WM_SetCaption(WINDOW_NAME, NULL);


    dInitODE();
  
    ilInit();
    ilutInit();

    #ifdef WIN32
    glewInit();
    #endif

    activateTextureObjects(true);
    // Mainloop
    Simulation sim;
    
    sim.enableGL(true);
    sim.activateOffscreenRendering(false);
    while (sim.simulate()) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        sim.processSDLEvent(&event);
      }
   
      sim.draw();
    }

    dCloseODE();
    ilShutDown();
  } else {
    cout << "Error intializing SDL" << endl;
  }

  SDL_Quit();

  return 0;
}

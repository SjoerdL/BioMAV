#ifndef SIMULATOR_HPP_
#define SIMULATOR_HPP_

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

#include <tolua++.h>
extern "C" {
  #include <lualib.h>
}

#include <SimulationEnv.hpp>

#include "vector3d.hpp"
#include "World.hpp"
#include "Parrot.hpp"

#define VIEW_COUNT 3

#define FRAME_WIDTH  1000
#define FRAME_HEIGHT 666

struct CameraView {
  Matrix camera; 

  float fov, aspect;
};

/**
The Simulation-class takes care of the whole simulation (model and visually). At initialization the object expects that
at initialization certain requirements have been met.

- The libraries IL, ILUT (from DevIL) and ODE are all initialized


Optionally, OpenGL support can be activated after a valid OpenGL context has been initialized by calling
enableGL(true). After a call to enableGL(false), the OpenGL context can be deinitialized again.
*/
class Simulation { 
  protected:
    lua_State* lua;

    bool coupleTime;
    bool stopSimulation;
    
    bool glEnabled;
    bool offscreenRendering;

    VirtualParrotDrone* drone;
    ParrotWorld world;

    double timeDrift, frameRateDrift;
    unsigned int startTime;
    long renderTimeStamp;
    double simulationTimeStamp;

    CameraView views[VIEW_COUNT];

    std::vector< std::vector<ILuint> > cameraImages;

    virtual void drawSequence();
  public:
    /**
      Creates a new simulation object. The required libraries (enumerated above) need to be before
      construction.
    */
    Simulation();
    
    /**
      Destroys the simulator and all objects that are part of the simulation.
    */
    virtual ~Simulation();

    /**
      Couples the simulation time to the system clock (default) or disables the coupling depending on the parameter c. 
      If activated the simulation time is synchronized with the system clock such that the simulation clock runs at most
      as fast the real time (possibly slower). This is useful if you want a nice visualization of the simulation.
      
      @param c
        True to activate coupling between system 
    */
    virtual void setCoupleTime(bool c);
    
    /**
      Checks whether time coupling has been activated.
      
      @return
        True if system time coupling is activated, false otherwise
    */
    virtual bool getCoupleTime() const;
    
    /**
      Executes a simulation step. A simulation step is only executed if:
      
       - system time coupling is deactivated
       - system time advance demands executing a new simulation step to keep the simulation time in synchrony with the system time
      
      A simulation step consists of the following steps:
      
       -# call to "beforeStep" in the simulator script
       -# call to ParrotWorld::stepWorld()
       -# call to "afterStep" in the simulator script
    */
    virtual bool simulate();
    
    /**
      Renders all 3D data to the current OpenGL context. Rendering is only done if OpenGL support has been enabled using
      enableGL(true).
    */
    virtual void draw();

    /**
      Returns the amount of drones (more general: robots) that are part of the current simulation.
      
      @return
        The amount of drones that are part of the simulation
    */
    virtual int getDroneCount();
    
    /**
      Gets the reference to drone where i is the index of the drone that should be retrieved.
      
      @param i
        Designates the drone index from that the amount of rendered images are retrieved. Valid are in the range from 0 to
        getDroneCount - 1.
      @return
        A reference to a drone object.
    */
    virtual VirtualParrotDrone* getDrone(int i);
    
    /**
      Returns how many images a drone returns as sensor data. Since the simulation object manages drawing to the open gl context,
      this sensor data is retrived from the simulation object rather the individual robot object (perhaps a TODO?)
      
      @param i
        Designates the drone index from that the amount of rendered images are retrieved. Valid are in the range from 0 to
        getDroneCount - 1.
      @return
        The amount of images the given drone provides.
    */
    virtual int getDroneImageCount(int i);
    
    /**
      Returns the image with index j of drone i as an DevIL-image. The image will only have any contents if "draw" has been called
      before, OpenGL is activated and offecreen rendering is activated using activateOffscreenRendering(true)
      
      @param i
        Designates the drone from that the image should be retrieved
      @param j
        Designates the image index that should be retrieved. Use getDroneImageCount to check how many images are retrived by a drone.
        Valid indexes are between 0 and getDroneImageCount - 1.
      @return
        The DevIL image containing the contents of the retrieved index. If no image has been rendered yet the contents might be empty
        (meaning for DevIL that they are a 1x1, black image)
    */
    virtual ILuint getDroneImage(int i, int j);

    /**
      Activates openGL-support and initializes all needed OpenGL objects (like vertex buffers etc).
    
      @param b
        Controls whther to enable or disable OpenGL support
    */
    virtual void enableGL(bool b);
    
    /**
      Activates the rendering of offscreen images. These are used to create the drone images retrieved by getDroneImage.
      
      @param b
        Controls whether to activate or deactivate rendering of offscreen images
    */
    virtual void activateOffscreenRendering(bool b);

    /**
      Gets the time stamp in milli seconds when the last image was rendered. The timestamp referes to simulation time.
      
      @return
        Time stamp in milli seconds simulation time (since the start of the simulation)
    */
    virtual long getLastRenderTimestamp();
    
    /**
      Gets the current timestamp of the simulation (in milli seconds).

      @return
        Time stamp in milli seconds simulation time (since the start of the simulation)
    */
    virtual long getLastSimulationTimestamp();

    /**
      Returns the lua state used by the simulator
      
      @return
        Lua environment used by the simulator
    */
    virtual lua_State* getLua();

    /**
      Lets the simulator process a given SDL-event. Should be called for default handling of SDL events whenever a SDL event has been
      received.
      
      @param event
        Pointer to the SDL_Event structure as returned by SDL_PollEvent (or similar)
    */
    virtual void processSDLEvent(SDL_Event* event);
};

#endif // SIMULATOR_HPP_


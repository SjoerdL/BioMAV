#ifndef WORLD_HPP_
#define WORLD_HPP_

class ParrotWorld;

#include <vector>

#include <ode/ode.h>

namespace PW {
  void CollisionCallback(ParrotWorld* pw, dGeomID o1, dGeomID o2);
}

typedef std::vector<ParrotWorld*> ParrotWorldPtrVector;

class WorldElement { // tolua_export
  protected:
    ParrotWorldPtrVector registeredParrotWorlds;
    
    virtual void unregisterFromRegisteredWorlds();
  public:
    virtual ~WorldElement(); // tolua_export
  
    virtual void draw() = 0;
    virtual void step(double deltaT) = 0;

    virtual void allocateOpenGLObjects();
    virtual void deallocateOpenGLObjects();

    virtual void registerWorld(ParrotWorld& world);
    virtual void unregisterWorld(ParrotWorld& world);
}; // tolua_export

#include "Pole.hpp"

typedef std::vector<WorldElement*> WorldElementPtrVector;
typedef std::vector<Pole*> PolePtrVector;

class ParrotWorld { // tolua_export
  protected:
    dWorldID world;
    dSpaceID collisionSpace;
    dJointGroupID contactJointGroup;

    bool glEnabled;

    WorldElementPtrVector worldElements;
    PolePtrVector poles;

    virtual void collide(dGeomID g1, dGeomID g2);
  public:
    /**
     * Creates a new (empty) simulator world
     */
    ParrotWorld();
    
    /**
     * Destructor for the parrot world. Destroys the lua environment and safely unregisters all
     * involves objects
     */ 
    virtual ~ParrotWorld();

    /**
      Performs a simulation step in this world. TODO: Extend this stub
      
      @param deltaT
        The time step in seconds that should be simulated.
    */
    virtual void stepWorld(double deltaT);
    
    /**
     * Renders all world objects using their draw function.
     */
    virtual void draw();

    /**
     * Adds a world element to the simulation environment.
     * 
     * @param we
     *   The new world element that is added to the simulation.
     */
    virtual void addWorldElement(WorldElement* we); // tolua_export
    
    /**
     * Removes a world object from the simulation environment
     * 
     * @param
     *   The world element that should be removed from the simulation
     * @return
     *   True if the world element was sucessfully removed from the simulation, false otherwise
     */
    virtual bool removeWorldElement(WorldElement* we); // tolua_export

    virtual dWorldID getWorldID();
    virtual dSpaceID getCollisionSpaceID();

    virtual int getPoleCount(); // tolua_export
    virtual Pole* getPole(int i); // tolua_export
    
    virtual void enableGL(bool b);

    virtual void registerPole(Pole* pole);
    virtual bool unregisterPole(Pole* pole);

    friend void PW::CollisionCallback(ParrotWorld*, dGeomID, dGeomID);
}; // tolua_export

#endif // WORLD_HPP_


#include "World.hpp"

#include <iostream>

#define XCHG(t, x, y) \
  do { \
    t tmp = (x); \
    (x) = (y); \
    (y) = tmp; \
  } while (false)

using namespace std;

namespace PW {
  void CollisionCallback(ParrotWorld* pw, dGeomID o1, dGeomID o2) {
    pw->collide(o1, o2);
  }
}

void WorldElement :: unregisterFromRegisteredWorlds() { 
  ParrotWorldPtrVector rwCopy = registeredParrotWorlds;
  for (ParrotWorldPtrVector::iterator it = rwCopy.begin(); it < rwCopy.end(); it++) {
    (*it)->removeWorldElement(this);
  }
}

WorldElement :: ~WorldElement() {
  unregisterFromRegisteredWorlds();
}

void WorldElement :: allocateOpenGLObjects() {
}

void WorldElement :: deallocateOpenGLObjects() {
}

void WorldElement :: registerWorld(ParrotWorld& world) {
  registeredParrotWorlds.push_back(&world);
}

void WorldElement :: unregisterWorld(ParrotWorld& world) {
  for (ParrotWorldPtrVector::iterator it = registeredParrotWorlds.begin(); it < registeredParrotWorlds.end(); it++) {
    if (*it == &world) {
      registeredParrotWorlds.erase(it);
      return;
    }
  }
  return;
}


ParrotWorld :: ParrotWorld() {
  glEnabled = false;

  world = dWorldCreate();
  collisionSpace = dSimpleSpaceCreate(0);
  contactJointGroup = dJointGroupCreate(0);

  dWorldSetGravity(world, 0, 0, -9.81);
}

ParrotWorld :: ~ParrotWorld() {
  WorldElementPtrVector worldElementCpy = worldElements;
  for (WorldElementPtrVector::iterator it = worldElementCpy.begin(); it < worldElementCpy.end(); it++) {
    delete (*it);
  }
  worldElements.clear();

  dJointGroupDestroy(contactJointGroup);
  dSpaceDestroy(collisionSpace);
  dWorldDestroy(world);
}

void ParrotWorld :: collide(dGeomID g1, dGeomID g2) {
  dBodyID b1 = dGeomGetBody(g1);
  dBodyID b2 = dGeomGetBody(g2);

  if (b2 == 0) {
    if (b1 == 0) {
      return; // Nothing to do...
    }
  }

  dContact contacts[16];
  int contactCount = dCollide(g1, g2, 16, &(contacts[0].geom), sizeof(*contacts));
  for (int i = 0; i < contactCount; i++) {
    contacts[i].surface.mode = 0;
    contacts[i].surface.mu = 4;

    dJointID cJoint = dJointCreateContact(world, contactJointGroup, &(contacts[i]));
    dJointAttach(cJoint, b1, b2);
  }
}

void ParrotWorld :: stepWorld(double deltaT) {
  for (WorldElementPtrVector::iterator it = worldElements.begin(); it < worldElements.end(); it++) {
    (*it)->step(deltaT);
  }

  dSpaceCollide(collisionSpace, this, (dNearCallback*) &(PW::CollisionCallback));

//  dWorldQuickStep(world, deltaT);
  dWorldStep(world, deltaT);

  dJointGroupEmpty(contactJointGroup);
}

void ParrotWorld :: draw() {
  for (WorldElementPtrVector::iterator it = worldElements.begin(); it < worldElements.end(); it++) {
    (*it)->draw();
  }
}

void ParrotWorld :: addWorldElement(WorldElement* we) {
  if (we == NULL) {
    cerr << "Tried to add NULL world element!" << endl;
  } else {
    for (WorldElementPtrVector::const_iterator it = worldElements.begin(); it < worldElements.end(); it++) {
      if (*it == we) {
        cerr << "World element already in world" << endl;
        return;
      }
    }

    worldElements.push_back(we);
    we->registerWorld(*this);
    if (glEnabled) {
      we->allocateOpenGLObjects();
    } else {
      we->deallocateOpenGLObjects();
    }
  }
}

bool ParrotWorld :: removeWorldElement(WorldElement* we) {
  if (we == NULL) {
    return false; // NULL is never part of the simulation
  } else {
    for (WorldElementPtrVector::iterator it = worldElements.begin(); it < worldElements.end(); it++) {
      if (we == *it) {
        we->unregisterWorld(*this);
        worldElements.erase(it);
        return true;
      }
    }
    return false;
  }
}

dWorldID ParrotWorld :: getWorldID() {
  return world;
}

dSpaceID ParrotWorld :: getCollisionSpaceID() {
  return collisionSpace;
}

void ParrotWorld :: enableGL(bool b) {
  if (glEnabled != b) {
    glEnabled = b;
    for (WorldElementPtrVector::const_iterator it = worldElements.begin(); it < worldElements.end(); it++) {
      if (b) {
        (*it)->allocateOpenGLObjects();
      } else {
//        (*it)->deallocateOpenGLObjects(); <- should not be intialized
      }
    }
  }
}

int ParrotWorld :: getPoleCount() {
//  std::cout << "PoleCount called and retruned " << poles.size() << std::endl;
  return poles.size();
}

Pole* ParrotWorld :: getPole(int i) {
  if ((i >= 0) && (i < getPoleCount()))
    return poles[i];
  return NULL;
}

void ParrotWorld :: registerPole(Pole* pole) {
  unregisterPole(pole);
  poles.push_back(pole);
}

bool ParrotWorld :: unregisterPole(Pole* pole) {
  for (PolePtrVector::iterator it = poles.begin(); it < poles.end(); it++) {
    if (*it == pole) {
      poles.erase(it);
      return true;
    }
  }
  return false;
}



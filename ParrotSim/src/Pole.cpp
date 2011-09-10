#include "Pole.hpp"

#include <iostream>

#include <cmath>

#define SLICES 16

#define WRITE_FLOAT(ptr, f) *(ptr) = (f); (ptr)++
#define WRITE_VECTOR(ptr, vec) \
  do { \
    Vector w = (vec); \
    WRITE_FLOAT(ptr, w[0]); \
    WRITE_FLOAT(ptr, w[1]); \
    WRITE_FLOAT(ptr, w[2]); \
  } while (false)

using namespace std;

namespace PoleNS {
  bool poleMeshInitialized = false;
  Mesh poleVertexMesh;
  const int vbVertexCount = SLICES * 4;

  void genVertexBuffer() {
    poleVertexMesh.setSize(vbVertexCount);

    for (int i = 0; i < SLICES; i++) {
      Vector lowI (cos(2 * M_PI * i / SLICES), sin(2 * M_PI * i / SLICES), 0);
      Vector highI (cos(2 * M_PI * (i + 1) / SLICES), sin(2 * M_PI * (i + 1) / SLICES), 0);

      poleVertexMesh.setPosition(i * 4 + 0,      lowI);
      poleVertexMesh.setNormal(i * 4 + 0,        lowI);
      poleVertexMesh.setTexCoordinate(i * 4 + 0, Vector(1.0 * i / SLICES, 0.0, 0.0));
      
      poleVertexMesh.setPosition(i * 4 + 1,      highI);
      poleVertexMesh.setNormal(i * 4 + 1,        highI);
      poleVertexMesh.setTexCoordinate(i * 4 + 1, Vector(1.0 * (i + 1) / SLICES, 0.0, 0.0));
      
      poleVertexMesh.setPosition(i * 4 + 2,      highI + Vector(0, 0, 1));
      poleVertexMesh.setNormal(i * 4 + 2,        highI);
      poleVertexMesh.setTexCoordinate(i * 4 + 2, Vector(1.0 * (i + 1) / SLICES, 1.0, 0.0));
      
      poleVertexMesh.setPosition(i * 4 + 3,      lowI + Vector(0, 0, 1));
      poleVertexMesh.setNormal(i * 4 + 3,        lowI);
      poleVertexMesh.setTexCoordinate(i * 4 + 3, Vector(1.0 * i / SLICES, 1.0, 0.0));
      
/*      WRITE_VECTOR(vbPtr, lowI);
      WRITE_VECTOR(vbPtr, lowI);
      WRITE_FLOAT(vbPtr, 1.0 * i / SLICES);
      WRITE_FLOAT(vbPtr, 0.0);

      WRITE_VECTOR(vbPtr, highI);
      WRITE_VECTOR(vbPtr, highI);
      WRITE_FLOAT(vbPtr, 1.0 * (i + 1) / SLICES);
      WRITE_FLOAT(vbPtr, 0.0);

      WRITE_VECTOR(vbPtr, highI + Vector(0, 0, 1));
      WRITE_VECTOR(vbPtr, highI);
      WRITE_FLOAT(vbPtr, 1.0 * (i + 1) / SLICES);
      WRITE_FLOAT(vbPtr, 1.0);

      WRITE_VECTOR(vbPtr, lowI + Vector(0, 0, 1));
      WRITE_VECTOR(vbPtr, lowI);
      WRITE_FLOAT(vbPtr, 1.0 * i / SLICES);
      WRITE_FLOAT(vbPtr, 1.0);*/
    }

//    std::cout << ((long) vbPtr - (long) vbData) / sizeof(float) << " == " << vbVertexCount << std::endl;
    poleMeshInitialized = true;
  }

  Mesh* getVertexBufferInstance() {
    if (!poleMeshInitialized) {
      genVertexBuffer();
    }
    return &poleVertexMesh;
  }

  void releaseVertexBufferInstance() {
  }
}

Pole :: Pole(const Vector& position, double radius, double height, Texture* tex) {
  collisionCapsule = 0;
  mesh = NULL;

  this->tex = NULL;
  if (tex != NULL) {
    this->tex = new Texture(*tex);
  }

  Matrix translateMatrix;
  locationMatrix.setScale(radius, radius, height);
  translateMatrix.setTranslate(position);
  locationMatrix = translateMatrix * locationMatrix;
//  std::cout << "Translation: " << locationMatrix.at(0, 3) << " " << locationMatrix.at(1, 3) << " " << locationMatrix.at(2, 3) << " " << std::endl;

  collisionCapsule = dCreateCCylinder(0, radius, height);
  dGeomSetPosition(collisionCapsule, position[0], position[1], position[2]);

  mesh = PoleNS::getVertexBufferInstance();
}

Pole :: ~Pole() {
  unregisterFromRegisteredWorlds();
  
  if (mesh != NULL) {
    PoleNS::releaseVertexBufferInstance();
  }

  if (tex != NULL) {
    delete tex;
  }

  if (collisionCapsule != 0) {
    dGeomDestroy(collisionCapsule);
  }
}

void Pole :: draw() {
  if (tex != NULL) {
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getTextureID());
  }

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf(locationMatrix.getValues());

  glColor3f(1, 1, 1);

  if (mesh != NULL) {
    mesh->draw();
  }

  glPopMatrix();

  if (tex != NULL) {
    glDisable(GL_TEXTURE_2D);
  }
}

void Pole :: step(double deltaT) {
}

Vector Pole :: getPosition() {
  return locationMatrix.at(3);
}

void Pole :: registerWorld(ParrotWorld& world) {
  WorldElement::registerWorld(world);
  
  dSpaceAdd(world.getCollisionSpaceID(), collisionCapsule);
  world.registerPole(this);
}

void Pole :: unregisterWorld(ParrotWorld& world) {
  WorldElement::unregisterWorld(world);

  dSpaceRemove(world.getCollisionSpaceID(), collisionCapsule);
  world.unregisterPole(this);
}

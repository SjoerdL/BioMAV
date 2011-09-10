#include "WorldBlock.hpp"

#include <iostream>
#include <cmath>

#include <GL/gl.h>
#include <ode/ode.h>

#define BLOCK_SUBDIVISION 2.0

#define MAX(x, y) (((x) > (y)) ? (x) : (y) )

#define SET_FLOAT(ptr, f) do { float* fp = (float*) (ptr); *fp = (f); } while (false)

void storeSurface(Mesh& mesh, int& i,
                  const Vector& v1,  const Vector& v2,  const Vector& v3,  const Vector& v4, const Vector& center,
                  const Vector& tv1, const Vector& tv2, const Vector& tv3, const Vector& tv4) {
  Vector normal = (v2 - v1).cross(v3 - v1);
  if (normal * (center - v1) > 0) {
    normal = normal * -1;
  }
  normal.normalize();

  mesh.setPosition(i, v1);
  mesh.setNormal(i, normal);
  mesh.setTexCoordinate(i, tv1);
  i++;

  mesh.setPosition(i, v2);
  mesh.setNormal(i, normal);
  mesh.setTexCoordinate(i, tv2);
  i++;

  mesh.setPosition(i, v3);
  mesh.setNormal(i, normal);
  mesh.setTexCoordinate(i, tv3);
  i++;

  mesh.setPosition(i, v4);
  mesh.setNormal(i, normal);
  mesh.setTexCoordinate(i, tv4);
  i++;
}

void storeSubdividedSurface(Mesh& mesh, int& i, 
                           const Vector& v1,  const Vector& v2,  const Vector& v3,  const Vector& v4, const Vector& center,
                           const Vector& tv1, const Vector& tv2, const Vector& tv3, const Vector& tv4) {
  if ((v1 - v2).abs() > BLOCK_SUBDIVISION) {
    Vector mids1 = (v1 + v2) * 0.5;
    Vector mids2 = (v3 + v4) * 0.5;
    Vector tmids1 = (tv1 + tv2) * 0.5;
    Vector tmids2 = (tv3 + tv4) * 0.5;

    storeSubdividedSurface(mesh, i, v1, mids1, mids2, v4, center, tv1, tmids1, tmids2, tv4);
    storeSubdividedSurface(mesh, i, mids2, v3, v2, mids1, center, tmids2, tv3, tv2, tmids1);
  } else if ((v1 - v4).abs() > BLOCK_SUBDIVISION) {
    Vector mids1 = (v1 + v4) * 0.5;
    Vector mids2 = (v2 + v3) * 0.5;
    Vector tmids1 = (tv1 + tv4) * 0.5;
    Vector tmids2 = (tv2 + tv3) * 0.5;

    storeSubdividedSurface(mesh, i, v1, mids1, mids2, v2, center, tv1, tmids1, tmids2, tv2);
    storeSubdividedSurface(mesh, i, mids1, v4, v3, mids2, center, tmids1, tv4, tv3, tmids2);
  } else {
    storeSurface(mesh, i, v1, v2, v3, v4, center, tv1, tv2, tv3, tv4);
  }
}

void WorldBlock :: draw() {
  if (tex != NULL) {
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getTextureID());
  }

  glColor3f(1, 1, 1);
  mesh.draw();

  if (tex != NULL) {
    glDisable(GL_TEXTURE_2D);
  }
}

void WorldBlock :: step(double deltaT) {
}

void WorldBlock :: allocateOpenGLObjects() {
  mesh.allocateGLBuffer();
}

void WorldBlock :: deallocateOpenGLObjects() {
  mesh.freeGLBuffer();
}

void WorldBlock :: registerWorld(ParrotWorld& world) {
  WorldElement::registerWorld(world);
  
  dSpaceAdd(world.getCollisionSpaceID(), collisionBox);
}

void WorldBlock :: unregisterWorld(ParrotWorld& world) {
  WorldElement::unregisterWorld(world);
  
  dSpaceRemove(world.getCollisionSpaceID(), collisionBox);
}

WorldBlock :: WorldBlock(const Vector& v1, const Vector& v2, Texture* tex, double texScale) : WorldElement() {
  Vector dimensions = v2 - v1;
  dimensions.set(0, fabs(dimensions[0]));
  dimensions.set(1, fabs(dimensions[1]));
  dimensions.set(2, fabs(dimensions[2]));

  bottomVectors[0] = Vector(v1[0], v1[1], v1[2]);
  bottomVectors[1] = Vector(v2[0], v1[1], v1[2]);
  bottomVectors[2] = Vector(v2[0], v2[1], v1[2]);
  bottomVectors[3] = Vector(v1[0], v2[1], v1[2]);

  topVectors[0] = Vector(v1[0], v1[1], v2[2]);
  topVectors[1] = Vector(v2[0], v1[1], v2[2]);
  topVectors[2] = Vector(v2[0], v2[1], v2[2]);
  topVectors[3] = Vector(v1[0], v2[1], v2[2]);

  center = (v1 + v2) * 0.5;

  collisionBox = dCreateBox(0, fabs(v1[0] - v2[0]), fabs(v1[1] - v2[1]), fabs(v1[2] - v2[2]));
  dGeomSetPosition(collisionBox, center[0], center[1], center[2]);

  this->tex = NULL;
  if (tex != NULL)
    this->tex = new Texture(*tex);

  // Fill buffer with data...
  {
    Vector maxTexCoords = dimensions * (1.0 / texScale);

    int maxQuads    = 2 * MAX(1, ((int) (ceil(fabs(v1[0] - v2[0]) / (BLOCK_SUBDIVISION / 2)) * ceil(fabs(v1[1] - v2[1]) / (BLOCK_SUBDIVISION / 2))))) +
                      2 * MAX(1, ((int) (ceil(fabs(v1[1] - v2[1]) / (BLOCK_SUBDIVISION / 2)) * ceil(fabs(v1[2] - v2[2]) / (BLOCK_SUBDIVISION / 2))))) + 
                      2 * MAX(1, ((int) (ceil(fabs(v1[2] - v2[2]) / (BLOCK_SUBDIVISION / 2)) * ceil(fabs(v1[0] - v2[0]) / (BLOCK_SUBDIVISION / 2)))));
    mesh.setSize(maxQuads * 4);
    int vertexIndex = 0;

    storeSubdividedSurface(mesh, vertexIndex, 
                           bottomVectors[0], bottomVectors[1], bottomVectors[2], bottomVectors[3], center, 
                           Vector(maxTexCoords[0], 0, 0), Vector(0, 0, 0), Vector(0, maxTexCoords[1], 0), Vector(maxTexCoords[0], maxTexCoords[1], 0));
    storeSubdividedSurface(mesh, vertexIndex, 
                           bottomVectors[0], bottomVectors[1], topVectors[1],    topVectors[0],    center, 
                           Vector(maxTexCoords[0], 0, 0), Vector(0, 0, 0), Vector(0, maxTexCoords[2], 0), Vector(maxTexCoords[0], maxTexCoords[2], 0));
    storeSubdividedSurface(mesh, vertexIndex, 
                           bottomVectors[1], bottomVectors[2], topVectors[2],    topVectors[1],    center, 
                           Vector(maxTexCoords[1], 0, 0), Vector(0, 0, 0), Vector(0, maxTexCoords[2], 0), Vector(maxTexCoords[1], maxTexCoords[2], 0));
    storeSubdividedSurface(mesh, vertexIndex, 
                           bottomVectors[2], bottomVectors[3], topVectors[3],    topVectors[2],    center, 
                           Vector(maxTexCoords[0], 0, 0), Vector(0, 0, 0), Vector(0, maxTexCoords[2], 0), Vector(maxTexCoords[0], maxTexCoords[2], 0));
    storeSubdividedSurface(mesh, vertexIndex, 
                           bottomVectors[3], bottomVectors[0], topVectors[0],    topVectors[3],    center, 
                           Vector(maxTexCoords[1], 0, 0), Vector(0, 0, 0), Vector(0, maxTexCoords[2], 0), Vector(maxTexCoords[1], maxTexCoords[2], 0));
    storeSubdividedSurface(mesh, vertexIndex, 
                           topVectors[0],    topVectors[1],    topVectors[2],    topVectors[3],    center, 
                           Vector(maxTexCoords[0], 0, 0), Vector(0, 0, 0), Vector(0, maxTexCoords[1], 0), Vector(maxTexCoords[0], maxTexCoords[1], 0));
    mesh.setCount(vertexIndex);
  }
}

WorldBlock :: ~WorldBlock() {
  WorldElement::unregisterFromRegisteredWorlds();
  
  dGeomDestroy(collisionBox);

  if (tex != NULL) {
    delete tex;
    tex = NULL;
  }
}


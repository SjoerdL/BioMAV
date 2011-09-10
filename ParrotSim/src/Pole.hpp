#ifndef POLE_HPP_
#define POLE_HPP_

class Pole;

#include "World.hpp"
#include "vector3d.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"

class Pole : public WorldElement { // tolua_export
  protected:
    Matrix locationMatrix;

    Mesh* mesh;
    Texture* tex;
    dGeomID collisionCapsule;
  public:
    Pole(const Vector& position, double radius, double height, Texture* tex); // tolua_export
    virtual ~Pole(); // tolua_export

    virtual void draw();
    virtual void step(double deltaT);

    virtual Vector getPosition(); // tolua_export

    virtual void registerWorld(ParrotWorld& world);
    virtual void unregisterWorld(ParrotWorld& world);
}; // tolua_export

#endif // POLE_HPP_

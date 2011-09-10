#ifndef WORLDBLOCK_HPP_
#define WORLDBLOCK_HPP_

#include "World.hpp"
#include "vector3d.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

class WorldBlock : public WorldElement { // tolua_export
  protected:
    Vector bottomVectors[4];
    Vector topVectors[4];

    Vector center;

    Mesh mesh;
    Texture* tex;

    dGeomID collisionBox;
  public:
    WorldBlock(const Vector& v1, const Vector& v2, Texture* tex, double texScale); // tolua_export
    virtual ~WorldBlock(); // tolua_export

    virtual void draw();
    virtual void step(double deltaT);

    virtual void allocateOpenGLObjects();
    virtual void deallocateOpenGLObjects();

    virtual void registerWorld(ParrotWorld& world);
    virtual void unregisterWorld(ParrotWorld& world);
}; // tolua_export

#endif // WORLDBLOCK_HPP_


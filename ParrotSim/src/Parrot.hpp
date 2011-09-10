#ifndef PARROT_HPP_
#define PARROT_HPP_

#include <memory>
#include <map>

#include <boost/thread.hpp>

#include <tolua++.h>
extern "C" {
  #include <lualib.h>
}

#ifdef _WIN32
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

#include "World.hpp"
#include "WavefrontObj.hpp"
#include "vector3d.hpp"



#define DRONE_LENGTH 0.60
#define DRONE_WIDTH 0.60
#define DRONE_HEIGHT 0.1

#define DRONE_MASS 1.0

#define MOTOR_DEFAULT_FORCE (9.81 * DRONE_MASS / 4.0)

#define MOTOR_FRONT_LEFT 0, 0
#define MOTOR_FRONT_RIGHT 1, 0
#define MOTOR_BACK_LEFT 0, 1
#define MOTOR_BACK_RIGHT 1, 1

namespace ParrotDrone {
  int rerouteParrotCall(lua_State* lua);
  int rerouteGetField(lua_State* lua);
}

typedef boost::lock_guard<boost::recursive_mutex> RecursiveMutexLockGuard;

struct LuaReferenceStruct {
  std::auto_ptr<RecursiveMutexLockGuard> luaLock;
  lua_State* lua;
};

class VirtualParrotDrone : public WorldElement { // tolua_export
  protected:
    std::auto_ptr<boost::recursive_mutex> droneMutexPtr;

    lua_State* lua;

    WavefrontMesh* mesh;

    Matrix meshReposMatrix;
    Matrix locationMatrix;

    dBodyID body;
    dGeomID collisionGeom;

    float forces[2][2];

    virtual void calculateReposMatrix();

    virtual void updateMatrixFromBody();
  public:
    VirtualParrotDrone();                               // tolua_export
    virtual ~VirtualParrotDrone();                      // tolua_export

    virtual Vector getPosition() const;                 // tolua_export
    virtual void setPosition(const Vector& pos);        // tolua_export

    virtual Matrix getLocationMatrix() const;           // tolua_export
    virtual void setLocationMatrix(const Matrix& mat);  // tolua_export

    virtual Vector getSpeed() const;                    // tolua_export
    virtual Vector getAngularSpeed() const;             // tolua_export

    virtual void setSpeed(const Vector& v);             // tolua_export
    virtual void setAngularSpeed(const Vector& v);      // tolua_export

    virtual void addTorque(float x, float y, float z);  // tolua_export

    virtual int getCameraCount() const;                 // tolua_export
    virtual Matrix getCameraTransformation(int i);      // tolua_export
    virtual void getCameraTransformation(int i, Matrix& mat);
    virtual void getCameraResolution(int i, int& w, int& h);

    virtual void setMotorPower(int leftright, int frontback, float force); // tolua_export
    virtual float getMotorPower(int leftright, int frontback) const;       // tolua_export

    virtual void callScriptFunction(const std::string& name, const std::string& functionSignature, const std::string& parameters, std::string& results); // tolua_export

    virtual void routeScriptLuaFunctions(lua_State* external_lua);
    virtual void lockLua(LuaReferenceStruct& luaStruct) const;

    virtual void draw();
    virtual void step(double deltaT);
    
    virtual void registerWorld(ParrotWorld& world);
    virtual void unregisterWorld(ParrotWorld& world);

    friend int ParrotDrone::rerouteGetField(lua_State*);
    friend int ParrotDrone::rerouteParrotCall(lua_State*);
}; // tolua_export

#endif // PARROT_HPP_


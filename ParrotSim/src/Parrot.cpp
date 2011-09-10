#include "Parrot.hpp"

#include <ParrotEnv.hpp>

#include "LuaToolbox.hpp"
#include "LuaStringParameterToolbox.hpp"

#ifdef DEBUG
#define STRINGIFY1(x) #x
#define STRINGIFY2(x) STRINGIFY1(x)
#define DEBUG_STRING(s) cerr << __FILE__ ":" STRINGIFY2(__LINE__) " " s << endl;
#else
#define DEBUG_STRING(s)
#endif 

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

using namespace std;
using namespace boost;
using namespace ParrotDrone;

namespace ParrotDrone {
  bool transduceStackValues(lua_State* from, lua_State* to, int start, int count) {
    int to_top = lua_gettop(to);
    for (int i = start; i < start + count; i++) {
      // Move all parameters to new environment
//      cout << lua_typename(from, lua_type(from, i)) << endl;
      switch (lua_type(from, i)) {
        case LUA_TNIL:
          lua_pushnil(to);
          break;
        case LUA_TNUMBER:
          lua_pushnumber(to, lua_tonumber(from, i));
          break;
        case LUA_TBOOLEAN:
          lua_pushboolean(to, lua_toboolean(from, i));
          break;
        case LUA_TSTRING:
          lua_pushstring(to, lua_tostring(from, i));
          break;
//        case LUA_TTABLE:  // TODO !
//          break;
        case LUA_TUSERDATA:
//          cout << "  Transducing " << tolua_typename(from, i) << endl;
          tolua_pushusertype(to, tolua_tousertype(from, i, NULL), tolua_typename(from, i));
          break;
        default:
          lua_settop(to, to_top);
          return false;
      }
    }
    return true;
  } 

  int rerouteParrotCall(lua_State* lua) {
    char errorMessage[128] = "";
    int result = -1;

    int parameterCount = lua_gettop(lua);

    { // Only use objects in here!!!!
      lua_pushvalue(lua, lua_upvalueindex(1));
      VirtualParrotDrone* drone = (VirtualParrotDrone*) tolua_tousertype(lua, -1, NULL); // It seems that tolua++ cannot interprete upvalues
      lua_pop(lua, 1);

      LuaReferenceStruct luaRefStruct;
      drone->lockLua(luaRefStruct);
      lua_State* drone_lua = luaRefStruct.lua;
      int drone_lua_top = lua_gettop(drone_lua); // Save to rest drone lua stack lateron

      // Push function
      tolua_pushusertype(drone_lua, drone, "VirtualParrotDrone");
      lua_getfield(drone_lua, -1, lua_tostring(lua, lua_upvalueindex(2)));

      if (!transduceStackValues(lua, drone_lua, 1, parameterCount)) {
        strcpy(errorMessage, "Parameter cannot be transferred to other lua environment (only allowed nil, number, boolean, string or C-object references)");
        result = -1;
      } else {
        string errorString;
        if (runFunction(drone_lua, parameterCount, LUA_MULTRET, errorString)) { // Call drone function
          result = lua_gettop(drone_lua) - drone_lua_top - 1; // "- 1" because one parrot drone-ref is left on the stack
          if (transduceStackValues(drone_lua, lua, drone_lua_top + 1 + 1, result)) {
            // Done :D
          } else {
            result = -1;
            strcpy(errorMessage, "Result cannot be transferred to other lua environment (only allowed nil, number, boolean, string or C-object references)");
          }
        } else {
          strcpy(errorMessage, errorString.c_str());
          result = -1;
        }
      }

      lua_settop(drone_lua, drone_lua_top); // Always reset drone lua stack!
    }

    if (result < 0) {
      luaL_error(lua, errorMessage); // CAUTION - LONG JUMP!
    }
    return result;
  }

  int rerouteGetField(lua_State* lua) {
    char errorMessage[128] = "";
    int result = -1;

    { // Only use objects in here!!!!
      int paramCount = lua_gettop(lua);
      if (paramCount < 2) { 
        strcpy(errorMessage, "Not enough parameters!");
      } else {
        if (!lua_isstring(lua, 2)) {
          lua_pushnil(lua);
          result = 1;
        } else {
          string key (lua_tostring(lua, 2));

          VirtualParrotDrone* this_drone = (VirtualParrotDrone*) tolua_tousertype(lua, 1, NULL);

          LuaReferenceStruct luaRefStruct;
          this_drone->lockLua(luaRefStruct);
          lua_State* drone_lua = luaRefStruct.lua;

          tolua_pushusertype(drone_lua, this_drone, "VirtualParrotDrone");
          lua_getfield(drone_lua, -1, key.c_str());
          if (lua_isnil(drone_lua, -1)) {
            // Call old index function
            lua_pop(drone_lua, 2);

            lua_getmetatable(lua, 1);
            lua_getfield(lua, -1, "Parrot_old_index");
            if (!lua_isfunction(lua, -1)) {
              lua_pushnil(lua);
              result = 1;
            } else {
              lua_pushvalue(lua, 1);
              lua_pushvalue(lua, 2);
              if (runFunction(lua, 2, 1)) {
                result = 1;
              } else {
                lua_pushnil(lua);
                result = 1;
              }
            }
          } else {
            // Create new function with matching cclosures
            lua_pushcclosure(lua, &rerouteParrotCall, 2);
            lua_pop(drone_lua, 2);
            result = 1;
          }
        }        
      }
    }

    if (result < 0) {
      luaL_error(lua, errorMessage); // CAUTION - LONG JUMP!
    }
    return result;
  }
}

VirtualParrotDrone :: VirtualParrotDrone() : WorldElement() {
  DEBUG_STRING("VirtualParrotDrone created");
  
  droneMutexPtr = auto_ptr<recursive_mutex>(new recursive_mutex());
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  
  lua = NULL;

  mesh = new WavefrontMesh("res/ParrotAR.obj");

  calculateReposMatrix();

  forces[0][0] = 0;
  forces[1][0] = 0;
  forces[0][1] = 0;
  forces[1][1] = 0;

  lua = luaL_newstate();
  if (lua != NULL) {
    int stackTop = lua_gettop(lua);
    luaL_openlibs(lua);
    tolua_ParrotEnv_open(lua);
    lua_settop(lua, stackTop);
  } 
  if (lua == NULL) {
    cout << "Warning! Could not create lua parrot context" << endl;
  }

  if (lua != NULL) {
    int stackTop = lua_gettop(lua);

    tolua_pushusertype(lua, this, "VirtualParrotDrone");
    lua_setglobal(lua, "drone");

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

    runScript(lua, "parrot.lua");
    
    lua_settop(lua, stackTop);
  }
}

VirtualParrotDrone :: ~VirtualParrotDrone() {
//  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr); <- unceccessary stuff may not run in here in paralell
  DEBUG_STRING("VirtualParrotDrone destroying");

  delete mesh;

  if (collisionGeom != 0) {
    dGeomDestroy(collisionGeom);
  }
  if (body != 0) {
    dBodyDestroy(body);
  }

  if (lua != NULL) {
    lua_close(lua);
    lua = NULL;
  }
  DEBUG_STRING("VirtualParrotDrone destructor finished");
}

void VirtualParrotDrone :: calculateReposMatrix() {
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);

  Vector mins, maxs;
  bool first = true;

  const vector<WaveFrontVertex>* vertices = mesh->getVertices();
  for (vector<WaveFrontVertex>::const_iterator it = vertices->begin(); it < vertices->end(); it++) {
    Vector v (it->coordinate[0], it->coordinate[1], it->coordinate[2]);
    if (first) {
      mins = v;
      maxs = v;
    } else {
      for (int i = 0; i < 3; i++) {
        mins.set(i, MIN(mins[i], v[i]));
        maxs.set(i, MAX(maxs[i], v[i]));
      }
    }

    first = false;
  }

  Matrix translateMatrix;
  translateMatrix.setTranslate((mins + maxs) * -0.5);

  Matrix scaleMatrix;
  scaleMatrix.setScale(2.0 / (maxs[0] - mins[0]) * DRONE_LENGTH / 2, 2.0 / (maxs[1] - mins[1]) * DRONE_WIDTH / 2, 2.0 / (maxs[2] - mins[2]) * DRONE_HEIGHT / 2);

  meshReposMatrix = translateMatrix * scaleMatrix;
}

void VirtualParrotDrone :: updateMatrixFromBody() {
  DEBUG_STRING("Updating matrix from body");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);

  if (body != 0) {
    const dReal* rotationMatrix = dBodyGetRotation(body);
    const dReal* translationVector = dBodyGetPosition(body);  

    for (int row = 0; row < 3; row++)
      for (int col = 0; col < 4; col++) 
        locationMatrix.set(row, col, *(rotationMatrix++));
    for (int i = 0; i < 3; i++)
      locationMatrix.set(i, 3, translationVector[i]);
  }
}

Vector VirtualParrotDrone :: getPosition() const {
  DEBUG_STRING("getPosition()");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  return locationMatrix[3];
}

void VirtualParrotDrone :: setPosition(const Vector& pos) {
  DEBUG_STRING("setPosition()");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  locationMatrix.set(0, 3, pos[0]);
  locationMatrix.set(1, 3, pos[1]);
  locationMatrix.set(2, 3, pos[2]);

  if (body != 0) {
    dBodySetPosition(body, locationMatrix.at(0, 3), locationMatrix.at(1, 3), locationMatrix.at(2, 3));
  }
}    

Matrix VirtualParrotDrone :: getLocationMatrix() const {
  DEBUG_STRING("getLocationMatrix()");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);

  Matrix res = locationMatrix;

  if (body != 0) {
//    cout << "Retranslated matrix" << endl;
    const dReal* mat = dBodyGetRotation(body);

    int i = 0;
    for (int row = 0; row < 3; row++)
      for (int col = 0; col < 4; col++)
//    for (int col = 0; col < 3; col++)
//      for (int row = 0; row < 4; row++)
        res.set(row, col, mat[i++]);

	i = 0;
	const dReal* pos = dBodyGetPosition(body);
    res.set(0, 3, pos[i++]);
    res.set(1, 3, pos[i++]);
    res.set(2, 3, pos[i++]);
  }
  
  return res;
}

void VirtualParrotDrone :: setLocationMatrix(const Matrix& mat) {
  DEBUG_STRING("setLocationMatrix()");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  locationMatrix = mat;

  if (body != 0) {
    dMatrix3 rotationMatrix;

    int i = 0;
    for (int row = 0; row < 3; row++)
      for (int col = 0; col < 4; col++)
        rotationMatrix[i++] = locationMatrix.at(row, col);
		
    dBodySetRotation(body, rotationMatrix);
    dBodySetPosition(body, locationMatrix.at(0, 3), locationMatrix.at(1, 3), locationMatrix.at(2, 3));
  }
}

Vector VirtualParrotDrone :: getSpeed() const {
  DEBUG_STRING("getSpeed()");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  Vector res;

  if (body != 0) {
    const dReal* vec = dBodyGetLinearVel(body);
    res.set(0, vec[0]);
    res.set(1, vec[1]);
    res.set(2, vec[2]);
  }

  return res;
}

Vector VirtualParrotDrone :: getAngularSpeed() const {
  DEBUG_STRING("getAngularSpeed()");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  Vector res;

  if (body != 0) {
    const dReal* vec = dBodyGetAngularVel(body);
    res.set(0, vec[0]);
    res.set(1, vec[1]);
    res.set(2, vec[2]);
  }

  return res;
}

void VirtualParrotDrone :: setSpeed(const Vector& v) {
  DEBUG_STRING("setSpeed()");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  if (body != 0) {
    dBodySetLinearVel(body, v[0], v[1], v[2]);
  }
}

void VirtualParrotDrone :: setAngularSpeed(const Vector& v) {
  DEBUG_STRING("setAngularSpeed");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  if (body != 0) {
    dBodySetAngularVel(body, v[0], v[1], v[2]);
  }
}

void VirtualParrotDrone :: addTorque(float x, float y, float z) {
  DEBUG_STRING("addTorque()");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  if (body != 0) {
    dBodyAddTorque(body, x, y, z);
  }
}

int VirtualParrotDrone :: getCameraCount() const {
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  return 2;
}

Matrix VirtualParrotDrone :: getCameraTransformation(int i) {
  DEBUG_STRING("getCameraTransformation");
  Matrix mat;
  getCameraTransformation(i, mat);
  return mat;
}

void VirtualParrotDrone :: getCameraTransformation(int i, Matrix& mat) {
  DEBUG_STRING("getCameraTransformation");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  updateMatrixFromBody();

  mat.eye();
  if (i == 0) {
    Vector dir = locationMatrix[0];
    dir.normalize();
    Vector from = locationMatrix[3] + dir * (DRONE_LENGTH / 2);
    mat.setViewMatrix(from, from + dir, locationMatrix[2]);
  }
  if (i == 1) {
    Vector dir = locationMatrix[2] * -1.0;
    dir.normalize();
    Vector from = locationMatrix[3] + dir * (DRONE_HEIGHT / 2);
    mat.setViewMatrix(from, from + dir, locationMatrix[0]);
  }
}

void VirtualParrotDrone :: getCameraResolution(int i, int& w, int& h) {
  DEBUG_STRING("getCameraResolution");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);

  w = 0;
  h = 0;
  if (i == 0) {
    w = 320;
    h = 240;
  }
  if (i == 1) {
    w = 176;
    h = 144;
  }
}

void VirtualParrotDrone :: setMotorPower(int leftright, int frontback, float force) {
  DEBUG_STRING("setMotorPower");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  forces[leftright][frontback] = force;
}

float VirtualParrotDrone :: getMotorPower(int leftright, int frontback) const {
  DEBUG_STRING("getMotorPower");
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  return forces[leftright][frontback];
}

void VirtualParrotDrone :: callScriptFunction(const string& name, const string& functionSignature, const string& parameters, string& results) {
  DEBUG_STRING("callScriptFunction");

  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);

  results.clear();

  tolua_pushusertype(lua, this, "VirtualParrotDrone");
  lua_getfield(lua, -1, name.c_str());

  if (lua_isfunction(lua, -1)) {
    string param_sig, result_sig;

    if (splitParametersAndResults(functionSignature, param_sig, result_sig)) {
      int resultCount = countParameterValues(result_sig);

      if (resultCount >= 0) {
        tolua_pushusertype(lua, this, "VirtualParrotDrone");
        int paramCount = pushEncodedLuaValues(lua, param_sig, parameters);

        if (paramCount >= 0) {
          int error = lua_pcall(lua, paramCount + 1, resultCount, 0);
          if (error != 0) {
            cout << "VirtualParrotDrone :: callScriptFunction - Error during executing " << name << endl;
            if (lua_isstring(lua, -1)) {
              cout << " -> " << lua_tostring(lua, -1) << endl;
            }
            lua_pop(lua, 1);
          } else {
            encodeLuaValues(lua, result_sig, results);
          }
        } else {
          cout << "VirtualParrotDrone :: callScriptFunction - Failed to push parameters!" << endl;
          lua_pop(lua, 2);
        }
      } else {
        cout << "VirtualParrotDrone :: callScriptFunction - Failed to determine result count!" << endl;
        lua_pop(lua, 2);
      }
    } else {
      cout << "VirtualParrotDrone :: callScriptFunction - Failed to split parameters and results" << endl;
      lua_pop(lua, 1);
    }
  } else {
    cout << "VirtualParrotDrone :: callScriptFunction - Error: Function " << name << " does not exist" << endl;
    lua_pop(lua, 1);
  }

  lua_pop(lua, 1);
}

void VirtualParrotDrone :: routeScriptLuaFunctions(lua_State* external_lua) {
  tolua_pushusertype(external_lua, this, "VirtualParrotDrone");
  lua_getmetatable(external_lua, -1);

  // Save old index function
  lua_pushstring(external_lua, "Parrot_old_index");
  lua_getfield(external_lua, -2, "__index");
  lua_rawset(external_lua, -3); // Must be raw - probably the metatable of the metatable is causing problems otherwise

  // Register new index function
  lua_pushstring(external_lua, "__index");
  lua_pushcfunction(external_lua, &ParrotDrone::rerouteGetField);
  lua_rawset(external_lua, -3);

  lua_setmetatable(external_lua, -2); // this.metatable
  lua_pop(external_lua, 1); // this
}

void VirtualParrotDrone :: lockLua(LuaReferenceStruct& luaStruct) const {
  luaStruct.luaLock = auto_ptr<RecursiveMutexLockGuard>(new RecursiveMutexLockGuard(*droneMutexPtr));
  luaStruct.lua = lua;
}

void VirtualParrotDrone :: draw() {
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  updateMatrixFromBody();

  glMatrixMode(GL_MATRIX_MODE);
  glPushMatrix();
  glMultMatrixf(locationMatrix.getValues());
  glMultMatrixf(meshReposMatrix.getValues());

  glBegin(GL_TRIANGLES);
   glColor3f(1, 0, 0);

   const vector<WaveFrontVertex>* vertices = mesh->getVertices();
   for (vector<WaveFrontVertex>::const_iterator it = vertices->begin(); it < vertices->end(); it++) {
     glNormal3f(it->normal[0], it->normal[1], it->normal[2]);
     glMultiTexCoord2f(GL_TEXTURE0, it->textureCoordinate[0], it->textureCoordinate[1]);
     glVertex3f(it->coordinate[0], it->coordinate[1], it->coordinate[2]);
   }
  glEnd();

  glPopMatrix();
}

void VirtualParrotDrone :: step(double deltaT) {
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  if (lua != NULL) {
    lua_gc(lua, LUA_GCRESTART, 0);
    lua_gc(lua, LUA_GCCOLLECT, 1);
  }

  if (body != 0) {
    if (lua != NULL) {
      int stackPosition = lua_gettop(lua);
      
      DEBUG_STRING("Calling Lua-step");
      tolua_pushusertype(lua, this, "VirtualParrotDrone");
      lua_getfield(lua, -1, "step");
      if (lua_isfunction(lua, -1)) {
        tolua_pushusertype(lua, this, "VirtualParrotDrone");
        lua_pushnumber(lua, deltaT);

        int error = lua_pcall(lua, 2, 0, 0);

        if (error != 0) {
          cout << "Error in drone:step" << endl;
          if (lua_isstring(lua, -1)) {
            cout << " -> " << lua_tostring(lua, -1) << endl;
          }
          lua_pop(lua, 1);
        }
      } else {
        lua_pop(lua, 1);
      }
      lua_pop(lua, 1);
      
      lua_settop(lua, stackPosition);
      
      DEBUG_STRING("Lua-step called");
    }

    updateMatrixFromBody();
    for (int leftright = 0; leftright < 2; leftright++)
      for (int frontback = 0; frontback < 2; frontback++) {
        dBodyAddRelForceAtRelPos(body, 0, 0, forces[leftright][frontback], 
                                       -(frontback * 2 - 1) * DRONE_LENGTH / 4, (leftright * 2 - 1) * DRONE_WIDTH / 4, 0);
      }
  }
  DEBUG_STRING("Step done");
}

void VirtualParrotDrone :: registerWorld(ParrotWorld& world) {
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  WorldElement::registerWorld(world);
  
  body = dBodyCreate(world.getWorldID());

  if (lua != NULL) {
    tolua_pushusertype(lua, &world, "ParrotWorld");
    lua_setglobal(lua, "world");
  }

  dMass bodyMass;
  bodyMass.setBoxTotal(DRONE_MASS, DRONE_LENGTH, DRONE_WIDTH, DRONE_HEIGHT);
  dBodySetMass(body, &bodyMass);

  collisionGeom = dCreateBox(world.getCollisionSpaceID(), DRONE_LENGTH, DRONE_WIDTH, DRONE_HEIGHT);
  dGeomSetBody(collisionGeom, body);

  setLocationMatrix(locationMatrix);
}

void VirtualParrotDrone :: unregisterWorld(ParrotWorld& world) {
  lock_guard<recursive_mutex> droneMutexLock(*droneMutexPtr);
  WorldElement::unregisterWorld(world);
  
  dGeomDestroy(collisionGeom);
  dBodyDestroy(body);
  body = 0;
  collisionGeom = 0;
}

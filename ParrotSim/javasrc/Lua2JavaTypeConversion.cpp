#include "Lua2JavaTypeConversion.hpp"

#include <iostream>
#include <cstring>

#include "CustomizedSimulatedDrone.hpp"

#ifdef DEBUG
#define STRINGIFY1(x) #x
#define STRINGIFY2(x) STRINGIFY1(x)
#define DEBUG_STRING(s) cerr << __FILE__ ":" STRINGIFY2(__LINE__) " " s << endl;
#else
#define DEBUG_STRING(s)
#endif 

using namespace std;

bool encodeUsertypeToJava(JNIEnv* env, string& luaTypename, void* obj, jobject& result) {
  result = NULL;
  
  if (luaTypename == "VirtualParrotDrone") {
    DEBUG_STRING("Converting VirtualParrotDrone to JAva type");

    result = getDroneIndexStruct(env, (VirtualParrotDrone*) obj).javaObjectRef;
  }

  if (luaTypename == "Vector") {
    DEBUG_STRING("Converting Vector to Java type");

    jdoubleArray doubleArray = env->NewDoubleArray(3);
    if (doubleArray == NULL) {
      cerr << "Double array could not be created" << endl;
    }
    Vector* vector = (Vector*) obj;
    
    jdouble rawDoubleVector[3];
    if (vector == NULL) {
      cerr << "Vector object is NULL" << endl;
    } else {
      rawDoubleVector[0] = vector->at(0);
      rawDoubleVector[1] = vector->at(1);
      rawDoubleVector[2] = vector->at(2);
    }
    
    env->SetDoubleArrayRegion(doubleArray, 0, 3, rawDoubleVector);
    
    result = doubleArray;
  }
  
  return true;
}

bool luaToJavaType(JNIEnv* env, lua_State* lua, jobject& result) {
  result = NULL;

  jclass doubleClass = env->FindClass("java/lang/Double");
  if (doubleClass == NULL) {
    return false;
  }
  
  jmethodID doubleConstructor = env->GetMethodID(doubleClass, "<init>", "(D)V");
  if (doubleConstructor == NULL) {
    return false;
  }
  
  jclass booleanClass = env->FindClass("java/lang/Boolean");
  if (booleanClass == NULL) {
    return false;
  }
  
  jmethodID booleanConstructor = env->GetMethodID(booleanClass, "<init>", "(Z)V");
  if (booleanConstructor == NULL) {
    return false;
  }
  
  jclass mapClass = env->FindClass("java/util/Map");
  if (mapClass == NULL) {
    return false;
  }
  
  jmethodID mapPutMethod = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  if (mapPutMethod == NULL) {
    return false;
  }
  
  jclass hashMapClass = env->FindClass("java/util/HashMap");
  if (hashMapClass == NULL) {
    return false;
  }
  
  jmethodID hashMapConstructor = env->GetMethodID(hashMapClass, "<init>", "()V");
  if (hashMapConstructor == NULL) {
    return false;
  }

  int luaType = lua_type(lua, -1);
  switch (luaType) {
    case LUA_TNIL:
        DEBUG_STRING("Encoding NIL");
      // Result already NULL
      break;
    case LUA_TNUMBER:
      {
        DEBUG_STRING("Encoding lua number: " << lua_tonumber(lua, -1));

        result = env->NewObject(doubleClass, doubleConstructor, lua_tonumber(lua, -1));
        if (result == NULL) {
          cerr << "Double could not be created" << endl;
          return false;
        }
      }
      break;
    case LUA_TBOOLEAN:
      {
        DEBUG_STRING("Encoding lua boolean: " << (lua_toboolean(lua, -1) != 0));
        
        result = env->NewObject(booleanClass, booleanConstructor, (lua_toboolean(lua, -1) != 0 ? JNI_TRUE : JNI_FALSE));
        if (result == NULL) {
          cerr << "Boolean could not be created" << endl;
          return false;
        }
      }
      break;
    case LUA_TSTRING:
      {
        DEBUG_STRING("Lua string: " << lua_tolstring(lua, -1, NULL));
        
        size_t strSize = 0;
        const char* str = lua_tolstring(lua, -1, &strSize);
        char* unicodeString = new char[2 * strSize];
        memset(unicodeString, 0, 2 * strSize);
        for (int i = 0; i < strSize; i++) {
          unicodeString[2 * i] = str[i]; 
        }
        
        result = env->NewString((const jchar*) unicodeString, strSize);
        delete[] unicodeString;
        if (result == NULL) {
          cerr << "String could not be created" << endl;
          return false;
        }
      }
      break;
    case LUA_TTABLE:
      {
        DEBUG_STRING("Encoding lua table");

        result = env->NewObject(hashMapClass, hashMapConstructor);
        if (result == NULL) {
          return false;
        }
      
        lua_pushnil(lua);  
        while (lua_next(lua, -2) != 0) {
          // encode value at index -1 
          jobject value = NULL;
          if (!luaToJavaType(env, lua, value)) {
            lua_pop(lua, 2); // Remove value + key
            return false;
          }

          // remove value, encode key 
          lua_pop(lua, 1);

          jobject key = NULL;
          if (!luaToJavaType(env, lua, key)) {
            lua_pop(lua, 1); // Only remove key here
            return false;
          }
          
          // put key-value pair
          env->CallObjectMethod(result, mapPutMethod, key, value);
        }
      }
      break;
    case LUA_TUSERDATA:
      {
        DEBUG_STRING("Encoding user type");

        const char* toluaTypename = tolua_typename(lua, -1);
        
        DEBUG_STRING("  tolua typename is " << toluaTypename);
        
        if (toluaTypename == NULL) {
          // Non tolua-types (?)
          cerr << "Warning! Cannot convert lua userdata " << endl;
          // NULL returned
        } else {
          string typenameStr = toluaTypename;
          void* objReference = tolua_tousertype(lua, -1, NULL);
          
          DEBUG_STRING("  object reference " << objReference);
          return encodeUsertypeToJava(env, typenameStr, objReference, result);
        }
      }
      break;
    default:
      cerr << "Warning! Cannot convert lua type " << lua_typename(lua, luaType) << " to Java type" << endl;
      // Result stays NULL
      break;
  }
  
  return true;
}

jobjectArray encodeLuaResults(JNIEnv* env, lua_State* lua, int paramCount) {
  jclass objectClass = env->FindClass("java/lang/Object");
  if (objectClass == NULL) {
    return NULL;
  }

  int stacktop = lua_gettop(lua) - paramCount;
  
  jobjectArray result = env->NewObjectArray(paramCount, objectClass, NULL);
  if (result == NULL) {
    lua_settop(lua, stacktop);
    return NULL;
  }
  
  DEBUG_STRING("Encoding " << paramCount << " results");
  
  for (int parameterIndex = 0; parameterIndex < paramCount; parameterIndex++) {
    DEBUG_STRING("Encoding result " << parameterIndex);
    
    int stackIndex = stacktop + 1 + parameterIndex;

    lua_pushvalue(lua, stackIndex);
    
    jobject obj = NULL;
    if (luaToJavaType(env, lua, obj)) {
      env->SetObjectArrayElement(result, parameterIndex, obj);
    } else {
      lua_settop(lua, stacktop);
      return NULL;
    }
    
    lua_pop(lua, 1);
  }

  return result;
}


#ifndef LUA2JAVATYPECONVERSION_HPP_
#define LUA2JAVATYPECONVERSION_HPP_

#include <jni.h>

#include <tolua++.h>
extern "C" {
  #include <lua.h>
}

bool luaToJavaType(JNIEnv* env, lua_State* lua, jobject& result);
jobjectArray encodeLuaResults(JNIEnv* env, lua_State* lua, int paramCount);

#endif // LUA2JAVATYPECONVERSION_HPP_

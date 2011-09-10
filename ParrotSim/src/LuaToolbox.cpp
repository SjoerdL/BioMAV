#include "LuaToolbox.hpp"

#include <iostream>
#include <sstream>

using namespace std;

bool runScript(lua_State* lua, string fileName) {
  bool result = false;

  int stackPos = lua_gettop(lua);
  int error = luaL_loadfile(lua, fileName.c_str());
  if (error == 0) {
    runFunction(lua, 0, 0);
  } else {
    switch (error) {
      case LUA_ERRFILE:
        cout << "Error: Cannot load " << fileName << endl;
        break;
      case LUA_ERRSYNTAX:
        cout << "Error: Syntax error in " << fileName << endl;
        break;
      default:
        cout << "Error: OutOfMem or something else occurred during loading " << fileName << endl;
    }
  }
  if (lua_gettop(lua) > stackPos) {
    if (lua_isstring(lua, -1)) {
      cout << " -> " << lua_tostring(lua, -1) << endl;
    }
  }
  lua_settop(lua, stackPos);

  return result;
}

bool runFunction(lua_State* lua, int args, int results) {
  string errorString;

  bool res = runFunction(lua, args, results, errorString);
  if (!res) {
    cout << errorString;
  }
  return res;
}

bool runFunction(lua_State* lua, int args, int results, string& errorMessage) {
  errorMessage = "";

  int functionTop = lua_gettop(lua) - 1 - args;

  int error = lua_pcall(lua, args, results, 0);
  if (error == 0) {
    return true;
  } else {
    stringstream errorStream (stringstream::in | stringstream::out);
  
    switch (error) {
      case LUA_ERRRUN:
        errorStream << "Error: Runtime error" << endl;
        break;
      case LUA_ERRMEM:
        errorStream << "Error: Memory allocation error" << endl;
        break;
      case LUA_ERRERR:
        errorStream << "Error: Error ocurred" << endl;
        break;
      default:
        errorStream << "Error: (Some?) Error occurred" << endl;
    }

    while (lua_gettop(lua) > functionTop) {
      errorStream << " -> " << lua_tostring(lua, -1) << endl;
      lua_pop(lua, 1);
    }

    errorMessage = errorStream.str();

    return false;
  }
}

void debug_dumpTable(lua_State* lua) {
  int t = lua_gettop(lua);
  /* table is in the stack at index 't' */
  lua_pushnil(lua);  /* first key */
  while (lua_next(lua, t) != 0) {
    /* uses 'key' (at index -2) and 'value' (at index -1) */
    lua_pushvalue(lua, -2);
    cout << "  " << lua_tostring(lua, -1) << " - " << lua_typename(lua, lua_type(lua, -2)) << endl;
    /* removes 'value'; keeps 'key' for next iteration */
    lua_pop(lua, 2);
  }
}

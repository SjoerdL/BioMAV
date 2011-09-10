#ifndef LUATOOLBOX_HPP_
#define LUATOOLBOX_HPP_

#include <string>

#include <tolua++.h>
extern "C" {
  #include <lualib.h>
}

bool runScript(lua_State* lua, std::string fileName);
bool runFunction(lua_State* lua, int args, int results);
bool runFunction(lua_State* lua, int args, int results, std::string& errorMessage);

void debug_dumpTable(lua_State* lua);

#endif // LUATOOLBOX_HPP_

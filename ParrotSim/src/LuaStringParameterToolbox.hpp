#ifndef LUASTRINGPARAMETERTOOLBOX_HPP_
#define LUASTRINGPARAMETERTOOLBOX_HPP_

#include <string>

extern "C" {
  #include <lua.h>
}

bool splitParametersAndResults(const std::string& functionSignature, std::string& param_sig, std::string& result_sig);
int countParameterValues(const std::string& descriptor);

int pushEncodedLuaValues(lua_State* lua, const std::string& descriptor, const std::string& values);
int encodeLuaValues(lua_State* lua, const std::string& descriptor, std::string& encodedValues);

#endif // LUASTRINGPARAMETERTOOLBOX_HPP_


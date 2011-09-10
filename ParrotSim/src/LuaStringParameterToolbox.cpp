#include "LuaStringParameterToolbox.hpp"

#include <cstdlib>

#include <iostream>
#include <sstream>

int getDecimalLength(const std::string& s, int p) {
  int res = 0;

  if (p < s.size()) {
    bool min = false;
    if (s[p] == '-') {
      min = true;
      res++;
      p++;
    }

    while ((p < s.size()) && (s[p] >= '0') && (s[p] <= '9')) {
      res++;
      p++;
    }

    if (min && (res <= 1))
      res = 0;
  }

  return res;
}

int getFloatLength(const std::string& s, int p) {
  int res = 0;

  if (p < s.size()) {
    bool min = false;
    bool decimalPoint = false;

    if (s[p] == '-') {
      min = true;
      res++;
      p++;
    }

    while ((p < s.size()) && (s[p] >= '0') && (s[p] <= '9')) {
      res++;
      p++;
    }

    if ((p < s.size()) && (s[p] == '.')) {
      decimalPoint = true;
      p++;
      res++;

      while ((p < s.size()) && (s[p] >= '0') && (s[p] <= '9')) {
        res++;
        p++;
      }

      if ((res <= 1) || (min && (res <= 2)))
        res = 0;
    } else {
      if (min && (res <= 1))
        res = 0;
    }
  }

  return res;
}

bool isWhitespace(char c) {
  return (c == ' ') || (c == '\t') || (c == 13) || (c == 10);
}

void trimString(std::string& str) {
  int count;
  
  count = 0;
  for (std::string::const_iterator it = str.begin(); (it < str.end()) && (isWhitespace(*it)); it++) {
    count++;
  }
  str.erase(0, count);

  count = 0;
  for (std::string::const_reverse_iterator it = str.rbegin(); (it < str.rend()) && (isWhitespace(*it)); it++) {
    count++;
  }
  str.erase(str.length() - count);
}

bool encodeParamString(const std::string& srcString, std::string& targetString) {
  targetString.clear();
  targetString.reserve(srcString.length() * 120 / 100);

  for (std::string::const_iterator it = srcString.begin(); it < srcString.end(); it++) {
    if (*it == '\\') {
      targetString += "\\\\";
    } else if (*it == '\x09') {
      targetString += "\\\x09";
    } else {
      targetString += *it;
    }
  }

  return true;
}

bool decodeParamString(const std::string& srcString, std::string& targetString, int endChar = '\x09') {
  targetString.clear();
  targetString.reserve(srcString.length());

  std::string::const_iterator nextIt;
  for (std::string::const_iterator it = srcString.begin(); (it < srcString.end()) && (((int) *it) != endChar); ) {
    nextIt = it + 1;

    if ((*it == '\\') && (nextIt < srcString.end())) {
      if ((*nextIt == '\\') || (*nextIt == '\x09')) {
        targetString += *nextIt;
        nextIt++;
      } else {
        return false;
      }
    } else {
      targetString += *it;
    }

    it = nextIt;
  }

  return true;
}


bool splitParametersAndResults(const std::string& functionSignature, std::string& param_sig, std::string& result_sig) {
  // Check signature string
  param_sig.clear();
  result_sig.clear();

  // Only one ( allowed
  int bracketOpenPosition = 0;
  {
    int count = 0;
    for (int i = 0; i < functionSignature.length(); i++) {
      if (functionSignature[i] == '(') {
        count++;
        bracketOpenPosition = i;
      }
    }
    if (count != 1) {
      std::cout << "Invalid function signature: " << functionSignature << std::endl;
      return false;
    }
  }

  // Only one ) allowed
  int bracketClosePosition = 0;
  {
    int count = 0;
    for (int i = 0; i < functionSignature.length(); i++) {
      if (functionSignature[i] == ')') {
        count++;
        bracketClosePosition = i;
      }
    }
    if (count != 1) {
      std::cout << "Invalid function signature: " << functionSignature << std::endl;
      return false;
    }
  }

  // ( before )
  if (bracketClosePosition < bracketOpenPosition) {
    std::cout << "Invalid function signature: " << functionSignature << std::endl;
    return false;
  }

  // Generate partial signatures
  param_sig = functionSignature.substr(bracketOpenPosition + 1, bracketClosePosition - bracketOpenPosition - 1);
  trimString(param_sig);
  if (param_sig.length() == 0) {
    std::cout << "Invalid function signature: " << functionSignature << std::endl;
    return false;
  }

  result_sig = functionSignature.substr(bracketClosePosition + 1);
  trimString(result_sig);
  if (result_sig.length() == 0) {
    std::cout << "Invalid function signature: " << functionSignature << std::endl;
    return false;
  }

  return true;
}

int countParameterValues(const std::string& descriptor) {
  int res = 0;
  for (std::string::const_iterator it = descriptor.begin(); it < descriptor.end(); it++) {
    if (*it == 'V') {
    } else if ((*it == 'B') || (*it == 'I') || (*it == 'F') || (*it == 'S')) {
      res++;
    } else {
      std::cout << "countParameterValues: Unknown format letter '" << *it << "' in format string " << descriptor << std::endl;
      return -1;
    }
  }
  return res;
}

int pushEncodedLuaValues(lua_State* lua, const std::string& descriptor, const std::string& values) {
  int initialStackPosition = lua_gettop(lua);

  if (descriptor.size() > 0) {
    int desc_i = 0;
    int value_i = 0;

    int parameterCount = 0;

    bool errorOccurred = false;

    while ((desc_i < descriptor.size()) && (!errorOccurred)) {
      while (isWhitespace(values[value_i]) && (value_i < values.size())) value_i++;

      if (descriptor[desc_i] == 'V') {
        // Skip parameter
      } else {
        if (value_i >= values.size()) {
          std::cout << "pushEncodedLuaValues: Values string ended prematurely. Format: " << descriptor << "  values: " << values << std::endl;
          errorOccurred = true;
        } else {
          parameterCount++;
          switch (descriptor[desc_i]) {
            case 'B':
              {
                if (values.substr(value_i, 4) == "true") {
                  value_i += 4;
                  lua_pushboolean(lua, true);
                } else if (values.substr(value_i, 5) == "false") {
                  value_i += 5;
                  lua_pushboolean(lua, false);
                } else {
                  errorOccurred = true;
                }
                if (!isWhitespace(values[value_i]) && (value_i < values.size())) 
                  errorOccurred = true;
              }
              break;
            case 'I':
              {
                int decLen = getDecimalLength(values, value_i);
                if (decLen <= 0) {
                  errorOccurred = true;
                } else {
                  lua_pushinteger(lua, atoi(values.substr(value_i, decLen).c_str()));
                  value_i += decLen;
                }
              }
              break;
            case 'F':
              {
                int floatLen = getFloatLength(values, value_i);
                if (floatLen <= 0) {
                  errorOccurred = true;
                } else {
                  lua_pushnumber(lua, atof(values.substr(value_i, floatLen).c_str()));
                  value_i += floatLen;
                }
              }
              break;
            default:
              std::cout << "pushEncodedLuaValues: Unknown format letter " << descriptor[desc_i] << " in format string " << descriptor << std::endl;
              errorOccurred = true;
          }
        }
      }

      desc_i++;
    }

    if (!errorOccurred) {
      return parameterCount;
    }
  } else {
    std::cout << "pushEncodedLuaValues: Descriptor is empty: " << descriptor << std::endl;
  }

  lua_settop(lua, initialStackPosition);
  return -1;
}

int encodeLuaValues(lua_State* lua, const std::string& descriptor, std::string& encodedValues) {
  encodedValues.clear();
  int valueCount = countParameterValues(descriptor);

  if (valueCount >= 0) {
    for (int i = 0; i < valueCount; i++) {
      int stackIndex = -valueCount + i;

      switch (descriptor[i]) {
        case 'B':
          {
            if (!lua_isboolean(lua, stackIndex)) {
              std::cout << "encodeLuaValues: Return value " << i + 1 << " is not boolean" << std::endl;
              break;
            } else {
              if (lua_toboolean(lua, stackIndex) != 0) {
                encodedValues += "\ttrue";
              } else {
                encodedValues += "\tfalse";
              }
            }
          }
          break;
        case 'I':
          {
            if (!lua_isnumber(lua, stackIndex)) {
              std::cout << "encodeLuaValues: Return value " << i + 1 << " is not number" << std::endl;
              break;
            } else {
              int luaRes = lua_tointeger(lua, stackIndex);

              std::stringstream integerRes (std::stringstream::in | std::stringstream::out);
              integerRes << "\t" << luaRes;

              encodedValues += integerRes.str();
            }
          }
          break;
        case 'F':
          {
            if (!lua_isnumber(lua, stackIndex)) {
              std::cout << "encodeLuaValues: Return value " << i + 1 << " is not number" << std::endl;
              break;
            } else {
              float luaRes = lua_tonumber(lua, stackIndex);

              std::stringstream floatRes (std::stringstream::in | std::stringstream::out);
              floatRes << "\t" << luaRes;

              encodedValues += floatRes.str();
            }
          }
          break;
        case 'S':
          {
            if (!lua_isstring(lua, stackIndex)) {
              std::cout << "encodeLuaValues: Return value " << i + 1 << " is not string" << std::endl;
              break;
            } else {
              encodedValues += "\t";
              encodedValues += lua_tostring(lua, stackIndex);
            }
          }
          break;
        case 'V':
          break;
        default:
          std::cout << "encodeLuaValues: Unknown format letter '" << descriptor[i] << "' in format string " << descriptor << std::endl;
          break;
      }    
    }

    lua_pop(lua, valueCount);
  }

  trimString(encodedValues);

  return valueCount;
}


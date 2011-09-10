/*
  Implementation according to http://en.wikipedia.org/wiki/Wavefront_.obj_file
*/

#include "WavefrontObj.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>

#define LINE_LEN 256

#ifdef DEBUG
#define DEBUG_STRING(s) cerr << "WavefronObj DEBUG: " << s << endl
#else
#define DEBUG_STRING(s) do {} while(0)
#endif

using namespace std;

void splitAll(const string& splitString, vector<string>& splits, char delimiter = ' ', bool skipRepeated = true, bool trim = true) {
  splits.clear();
  
  int start = 0;
  int end = splitString.length();

  if (trim) {
    while ((start < splitString.length()) && (splitString[start] == delimiter)) {
      start++;
    }

    while ((end > 0) && (splitString[end - 1] == delimiter)) {
      end--;
    }
  }

  if (start < end) {
    int i = start;
    while (i < end) {
      size_t delPos = splitString.find(delimiter, i);
      
      if ((delPos == string::npos) || (delPos >= end)) {
        splits.push_back(splitString.substr(i, end - i));
        i = end;
      } else {
        splits.push_back(splitString.substr(i, delPos - i));
        i = delPos + 1;
        if (skipRepeated) {
          while ((i < end) && (splitString[i] == delimiter)) {
            i++;
          }
        }
      }
    }
  }
}

const vector<WaveFrontVertex>* WavefrontMesh :: getVertices() const {
  return &vertices;
}

WavefrontMesh :: WavefrontMesh(const string& fileName) {
  ifstream ins (fileName.c_str());

  vector< vector<float> > vertexCoordinates;
  vector< vector<float> > vertexNormals;
  vector< vector<float> > vertexTCoordinates;

  if (!ins.fail()) {
    char c_line[LINE_LEN];

    while (!ins.eof()) {
      ins.getline(c_line, LINE_LEN);
      string line = c_line;

      if ((line[0] == '#') or (line.empty())) {
      } else {
        vector<string> lineSplits;
        splitAll(line, lineSplits);

        if (lineSplits.size() == 0) {
          cerr << "Warning! Line not interpretable: " << line << endl;
        } else {
          DEBUG_STRING("Processing line: " << line);
          DEBUG_STRING(" --> First split: " << lineSplits[0]);

          string typeString = lineSplits[0];

          bool processed = false;

          if (typeString.compare("v") == 0) {
            processed = true;

            // Vertex Coordinate
            if (lineSplits.size() < 4) {
              cerr << "Warning! v argument needs at least 3 coordinates: " << line << endl;
            } else {
              vector<float> v;
              v.push_back(atof(lineSplits[1].c_str()));
              v.push_back(atof(lineSplits[2].c_str()));
              v.push_back(atof(lineSplits[3].c_str()));
              vertexCoordinates.push_back(v);
            }
          }

          if (typeString.compare("vt") == 0) {
            processed = true;

            // Texture Coordinate
            if (lineSplits.size() < 3) {
              cerr << "Warning! vt argument needs at least 2 coordinates: " << line << endl;
            } else {
              vector<float> v;
              v.push_back(atof(lineSplits[1].c_str()));
              v.push_back(atof(lineSplits[2].c_str()));
              vertexTCoordinates.push_back(v);
            }
          }

          if (typeString.compare("vn") == 0) {
            processed = true;

            // Vertex Normal
            if (lineSplits.size() < 4) {
              cerr << "Warning! vn argument needs at least 3 coordinates: " << line << endl;
            } else {
              vector<float> v;
              v.push_back(atof(lineSplits[1].c_str()));
              v.push_back(atof(lineSplits[2].c_str()));
              v.push_back(atof(lineSplits[3].c_str()));
              vertexNormals.push_back(v);
            }
          }

          if (typeString.compare("f") == 0) {
            processed = true;

            // Face
            if (lineSplits.size() != 4) {
              cerr << "Warning! This wavefront obj implementation only supports triangular faces: " << line << endl;
            } else {
              vector<WaveFrontVertex> newFace;
              newFace.reserve(3);
              
              for (vector<string>::iterator fv = lineSplits.begin() + 1; fv < lineSplits.end(); fv++) {
                vector<string> splits;
                splitAll(*fv, splits, '/', false);
                if (splits.size() == 0) {
                  cerr << "Erronous face description: " << line << endl;
                  newFace.clear();
                  break;
                } else {
                  int vertexIndex = atoi(splits[0].c_str()) - 1;
                  int textureIndex = -1;
                  int normalIndex = -1;

                  if (splits.size() > 1) {
                    if (!splits[1].empty()) {
                      textureIndex = atoi(splits[1].c_str()) - 1;
                    }
                    if (splits.size() > 2) {
                      normalIndex = atoi(splits[2].c_str()) - 1;
                    }
                  }

                  WaveFrontVertex v;
                  if (vertexIndex >= vertexCoordinates.size()) {
                    cerr << "Erronous vertex index: " << line << endl;
                    newFace.clear();
                    break;
                  } else {
                    DEBUG_STRING("Adding vertex data using index " << vertexIndex);

                    v.coordinate[0] = vertexCoordinates[vertexIndex][0];
                    v.coordinate[1] = vertexCoordinates[vertexIndex][1];
                    v.coordinate[2] = vertexCoordinates[vertexIndex][2];

                    if (textureIndex != -1) {
                      if (textureIndex >= vertexTCoordinates.size()) {
                        cerr << "Erronous texture coordinate index: " << line << endl;
                        newFace.clear();
                        break;
                      } else {
                        DEBUG_STRING("Adding texture data using index " << textureIndex);

                        v.textureCoordinate[0] = vertexTCoordinates[textureIndex][0];
                        v.textureCoordinate[1] = vertexTCoordinates[textureIndex][1];
                      }
                    }

                    if (normalIndex != -1) {
                      if (normalIndex >= vertexNormals.size()) {
                        cerr << "Erronous normal index: " << line << endl;
                        newFace.clear();
                        break;
                      } else {
                        DEBUG_STRING("Adding normal data using index " << normalIndex);

                        v.normal[0] = vertexNormals[normalIndex][0];
                        v.normal[1] = vertexNormals[normalIndex][1];
                        v.normal[2] = vertexNormals[normalIndex][2];
                      }
                    }
                  }

                  newFace.push_back(v);
                }
              }

              if (newFace.size() == 3) {
                for (vector<WaveFrontVertex>::iterator it = newFace.begin(); it < newFace.end(); it++)
                  vertices.push_back(*it);
              }
            }
          }

          if (!processed) {
            cerr << "Wavefront mesh warning: Unknown command: " << line << endl;
          }
        }
      }
    }

    ins.close();
  } else {
    throw string("File connot be opened");
  }
}

WavefrontMesh :: ~WavefrontMesh() {
}



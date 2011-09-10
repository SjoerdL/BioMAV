#ifndef WAVEFRONTOBJ_HPP_
#define WAVEFRONTOBJ_HPP_

#include <string>
#include <vector>

struct WaveFrontVertex {
  float coordinate[3];
  float normal[3];
  float textureCoordinate[2];
};

class WavefrontMesh {
  protected:
    std::vector<WaveFrontVertex> vertices;
  public:
    virtual const std::vector<WaveFrontVertex>* getVertices() const;

    WavefrontMesh(const std::string& fileName);
    virtual ~WavefrontMesh();
};

#endif // WAVEFRONTOBJ_HPP_

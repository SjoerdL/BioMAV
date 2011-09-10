#ifndef MESH_HPP_
#define MESH_HPP_

#include <vector>

#ifdef _WIN32
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

#include "vector3d.hpp"
#include "WavefrontObj.hpp"

typedef std::vector<Vector> MeshVectorArray;

class Mesh {
  protected:
    MeshVectorArray positions, normals, texCoordinates; 
    
    int primType;
    int count;
    
    bool glAllocated;
    GLuint glVertexBuffer;
  public:
    virtual Vector getPosition(int i) const;
    virtual Vector getNormal(int i) const;
    virtual Vector getTexCoordinate(int i) const;

    virtual void setPosition(int i, const Vector& v);
    virtual void setNormal(int i, const Vector& v);
    virtual void setTexCoordinate(int i, const Vector& v);
    
    virtual void setSize(int size);
    virtual int getSize() const;
    
    virtual int getCount() const;
    virtual void setCount(int c);
    
    virtual void setPrimitiveType(int primitiveType);
    virtual int getPrimitiveType() const;
    
    virtual void update();
    
    virtual void draw() const;
    
    virtual void allocateGLBuffer();
    virtual void freeGLBuffer();
    
    virtual void loadFromWavefront(const WavefrontMesh& wavefront);
    
    Mesh& operator=(const Mesh& other);
    
    Mesh();
    virtual ~Mesh();
};

#endif // MESH_HPP_


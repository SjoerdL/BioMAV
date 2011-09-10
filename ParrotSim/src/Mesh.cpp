#include "Mesh.hpp"

#ifdef DEBUG
#define STRINGIFY1(x) #x
#define STRINGIFY2(x) STRINGIFY1(x)
#define DEBUG_STRING(s) cerr << __FILE__ ":" STRINGIFY2(__LINE__) " " s << endl;
#else
#define DEBUG_STRING(s)
#endif 

using namespace std;

Vector Mesh :: getPosition(int i) const {
  return positions[i];
}

Vector Mesh :: getNormal(int i) const {
  return normals[i];
}

Vector Mesh :: getTexCoordinate(int i) const {
  return texCoordinates[i];
}

void Mesh :: setPosition(int i, const Vector& v) {
  positions[i] = v;
}

void Mesh :: setNormal(int i, const Vector& v) {
  normals[i] = v;
}

void Mesh :: setTexCoordinate(int i, const Vector& v) {
  texCoordinates[i] = v;
}

void Mesh :: setSize(int size) {
  DEBUG_STRING("Setting size to " << size);
  positions.clear();
  positions.assign(size, Vector());
  normals.clear();
  normals.assign(size, Vector());
  texCoordinates.clear();
  texCoordinates.assign(size, Vector());
  count = size;
  update();
}

int Mesh :: getSize() const {
  return positions.size();
}

int Mesh :: getCount() const {
  return count;
}

void Mesh :: setCount(int c) {
  count = c;
  DEBUG_STRING("Setting count to " << count);
}
    
void Mesh :: setPrimitiveType(int primitiveType) {
  primType = primitiveType;
}

int Mesh :: getPrimitiveType() const {
  return primType;
}
    
void Mesh :: update() {
  if (glAllocated) {
    DEBUG_STRING("Updating GL Buffer");
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, glVertexBuffer);

    // Update buffer
    float rawVertexBuffer[getSize() * (3 + 3 + 2)];
    int j = 0;
    for (int i = 0; i < getSize(); i++) {
      memcpy(&(rawVertexBuffer[j]), positions[i].getValues(), sizeof(float) * 3);
      j += 3;
      memcpy(&(rawVertexBuffer[j]), normals[i].getValues(), sizeof(float) * 3);
      j += 3;
      memcpy(&(rawVertexBuffer[j]), texCoordinates[i].getValues(), sizeof(float) * 2);
      j += 2;
    }
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, getSize() * (3 + 3 + 2) * sizeof(float), rawVertexBuffer, GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

void Mesh :: draw() const {
  if (glAllocated) {
//    DEBUG_STRING("Drawing GL Buffer");
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, glVertexBuffer);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(float) * (3 + 3 + 2), (GLvoid*) (0 * sizeof(float)));
    glNormalPointer(GL_FLOAT, sizeof(float) * (3 + 3 + 2), (GLvoid*) (3 * sizeof(float)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(float) * (3 + 3 + 2), (GLvoid*) ((3 + 3) * sizeof(float)));

    glDrawArrays(primType, 0, getCount());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  } else {
//    DEBUG_STRING("Drawing without GL Buffer");
    glBegin(primType);
     for (int i = 0; i < getCount(); i++) {
       glNormal3f(normals[i][0], normals[i][1], normals[i][2]);
       glMultiTexCoord2f(GL_TEXTURE0, texCoordinates[i][0], texCoordinates[i][1]);
       glVertex3f(positions[i][0], positions[i][1], positions[i][2]);
     }
    glEnd();
  }
}

void Mesh :: allocateGLBuffer() {
  if (!glAllocated) {
    DEBUG_STRING("Allocating GL Buffer");

    glAllocated = true;

    glGenBuffersARB(1, &glVertexBuffer);

    update();
  }
}

void Mesh :: freeGLBuffer() {
  if (glAllocated) {
    DEBUG_STRING("Freeing GL Buffer");

    glAllocated = false;

    glDeleteBuffersARB(1, &glVertexBuffer);
  }
}

void Mesh :: loadFromWavefront(const WavefrontMesh& wavefront) {
  DEBUG_STRING("Loading mesh from wavefront");
  vector<WaveFrontVertex> wfData = *(wavefront.getVertices());
  setSize(wfData.size());
  for (int i = 0; i < getSize(); i++) {
    WaveFrontVertex* vertex = &(wfData[i]);
    setPosition(i, Vector(vertex->coordinate[0], vertex->coordinate[1], vertex->coordinate[2]));
    setNormal(i, Vector(vertex->normal[0], vertex->normal[1], vertex->normal[2]));
    setTexCoordinate(i, Vector(vertex->textureCoordinate[0], vertex->textureCoordinate[1], vertex->textureCoordinate[2]));
  }
  
  update();
}

Mesh& Mesh :: operator=(const Mesh& other) {
  DEBUG_STRING("operator=");
  freeGLBuffer();

  positions = other.positions;
  normals = other.normals;
  texCoordinates = other.texCoordinates;
  if (other.glAllocated) {
    allocateGLBuffer();
  }
  
  update();
  
  return *this;
}

Mesh :: Mesh() {
  glAllocated = false;
  primType = GL_QUADS;
}

Mesh :: ~Mesh() {
  freeGLBuffer();
}



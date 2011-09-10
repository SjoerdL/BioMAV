#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#ifdef _WIN32
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

void activateTextureObjects(bool active);

class Texture { // tolua_export
  protected:
    GLuint* textureID;
  public:
    virtual GLuint getTextureID();

    Texture& operator= (const Texture& t);

    Texture(const char* fileName); // tolua_export 
    Texture(const Texture& tex); // tolua_export
    virtual ~Texture();
}; // tolua_export

#endif // TEXTURE_HPP_


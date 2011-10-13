#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#ifdef _WIN32
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

void activateTextureObjects(bool active);

/**
The texture class loads and holds a OpenGL (2D) texture. It also is able to dynamically unload its
OpenGL ressources as reaction to a call to {@link activateTextureObjects}.
*/
class Texture { // tolua_export
  protected:
    GLuint* textureID;
  public:
    /**
      Returns the OpenGL texture ID, if texture ressources are loaded.
      
      @return 
        If OpenGL ressources are initialized, the OpenGL texture handle.
        Otherwise 0.
        
      {@see activateTextureObjects}
    */
    virtual GLuint getTextureID();

    /**
      Copies the contents from one texture object into another
      
      @param t
        Texture to copy data from
      @result
        Reference to this texture object
    */
    Texture& operator= (const Texture& t);

    /**
      Creates a texture loading the contents from the specified file
      
      @param fileName
        File to load the texture from
    */
    Texture(const char* fileName); // tolua_export 
    
    /**
      Copy constructor for a texture
      
      @param tex
        Texture to be copied by this object
    */
    Texture(const Texture& tex); // tolua_export
    
    /**
      Destructor unloading and freeing all allocated texture ressources
    */
    virtual ~Texture();
}; // tolua_export

#endif // TEXTURE_HPP_


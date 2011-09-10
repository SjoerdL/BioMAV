#include "Texture.hpp"

#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

extern "C" {
#include <IL/il.h>
#include <IL/ilut.h>
}

#define MAX_TEXTURES 64

#ifdef DEBUG
#define STRINGIFY1(x) #x
#define STRINGIFY2(x) STRINGIFY1(x)
#define DEBUG_STRING(s) cerr << __FILE__ ":" STRINGIFY2(__LINE__) " " s << endl;
#else
#define DEBUG_STRING(s)
#endif 

using namespace std;

namespace TextureManagement {
  bool TextureAllocationActive = true;

  struct TextureAllocationData {
    int instanceCount;

    string fileName;
    GLuint glTexture;
  };

  vector<TextureAllocationData> allocatedTextures;

  void initTextureManagement() {
    if (allocatedTextures.size() < MAX_TEXTURES) {
      if (TextureAllocationActive) {
        for (vector<TextureAllocationData>::iterator it = allocatedTextures.begin(); it < allocatedTextures.end(); it++)
          if (!it->fileName.empty())
            glDeleteTextures(1, &(it->glTexture));
      }
      allocatedTextures.clear();

      for (int i = 0; i < MAX_TEXTURES; i++) {
        TextureAllocationData tad;
        tad.instanceCount = 0;
        tad.fileName = "";
        tad.glTexture = 0;

        allocatedTextures.push_back(tad);
      }
    }
  }

  void doLoadTexture(TextureAllocationData& data) {
    DEBUG_STRING("Allocating texture " << data.fileName);
    
    glEnable(GL_TEXTURE_2D);

    char tmpStr[data.fileName.length() + 1];
    strcpy(tmpStr, data.fileName.c_str());
    data.glTexture = ilutGLLoadImage(tmpStr); // TODO: Change... do not get any error message from this :(

    glBindTexture(GL_TEXTURE_2D, data.glTexture);
//    glGenerateMipmap(GL_TEXTURE_2D); - buggy on some machines!
    ilutGLBuildMipmaps();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glDisable(GL_TEXTURE_2D);
  }

  bool retrieveTextureID(const string& fileName, GLuint*& textureID) {
    initTextureManagement();

    // Search for already allocated texture...
    for (vector<TextureAllocationData>::iterator it = allocatedTextures.begin(); it < allocatedTextures.end(); it++) {
      if (it->fileName.compare(fileName) == 0) {
        textureID = &(it->glTexture);
        it->instanceCount++;
        return true;
      }
    }

    // Search for place to allocate texture...
    for (vector<TextureAllocationData>::iterator it = allocatedTextures.begin(); it < allocatedTextures.end(); it++) {
      if (it->instanceCount <= 0) {
        if (TextureAllocationActive) {
          if (!it->fileName.empty()) {
            glDeleteTextures(1, &(it->glTexture));
          }
        }

        it->fileName = fileName;

        if (TextureAllocationActive) {
          doLoadTexture(*it);
        } else {
          it->glTexture = 0;
        }

        it->instanceCount = 1;

        textureID = &(it->glTexture);
    
        return true;
      }
    }

    return false;
  }

  bool addTextureIDRef(const GLuint* const textureID) {
    for (vector<TextureAllocationData>::iterator it = allocatedTextures.begin(); it < allocatedTextures.end(); it++) {
      if ((textureID == &(it->glTexture)) && (it->instanceCount > 0)) {
        it->instanceCount++;
        return true;
      }
    }

    return false;
  }

  bool releaseTextureIDRef(const GLuint* const textureID) {
    for (vector<TextureAllocationData>::iterator it = allocatedTextures.begin(); it < allocatedTextures.end(); it++) {
      if ((textureID == &(it->glTexture)) && (it->instanceCount > 0)) {
        it->instanceCount--;
        return true;
      }
    }

    return false;
  }

  void reallocateTextureObjects() {
    for (vector<TextureAllocationData>::iterator it = allocatedTextures.begin(); it < allocatedTextures.end(); it++) {
      if ((it->instanceCount > 0) && (it->fileName.size() > 0)) {
        doLoadTexture(*it);
      }
    }
  }

  void deallocateTextureObjects() {
    for (vector<TextureAllocationData>::iterator it = allocatedTextures.begin(); it < allocatedTextures.end(); it++) {
      if ((it->instanceCount > 0) && (it->fileName.size() > 0)) {
        glDeleteTextures(1, &(it->glTexture));
        it->glTexture = 0;
      }
    }
  }
}

using namespace TextureManagement;

void activateTextureObjects(bool active) {
  if (TextureAllocationActive != active) {
    if (active) {
      reallocateTextureObjects();
    } else {
      deallocateTextureObjects();
    }
  }
  TextureAllocationActive = active;
}


GLuint Texture :: getTextureID() {
  DEBUG_STRING("Retrieving texture ID at " << textureID);
  if (textureID != NULL) {
    DEBUG_STRING(" -> " << *textureID);
    return *textureID;
  } else {
    return 0;
  }
}

Texture& Texture :: operator=(const Texture& t) {
  textureID = t.textureID;
  addTextureIDRef(textureID);
  return *this;
}

Texture :: Texture(const char* fileName) {
  textureID = NULL;
  DEBUG_STRING("Texture ID init.start: " << textureID);   
  if (!retrieveTextureID(fileName, textureID)) {
    cout << string("Cannot load texture ") + fileName << endl;
    throw string("Cannot load texture ") + fileName;
  }
  DEBUG_STRING("Texture ID init.end: " << textureID);   
}

Texture :: Texture(const Texture& tex) {
  *this = tex;
}

Texture :: ~Texture() {
  releaseTextureIDRef(textureID);
}

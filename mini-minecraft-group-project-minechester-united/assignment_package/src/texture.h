#ifndef TEXTURE_H
#define TEXTURE_H

#include "openglcontext.h"
#include "smartpointerhelp.h"
#include <GL/gl.h>
#endif // TEXTURE_H

class Texture {
private:
    OpenGLContext* context;
    GLuint m_textureHandle;
    uPtr<QImage> m_textureImage;

public:
    Texture(OpenGLContext *context);
    ~Texture();

    void create(const char *texturePath);
    void load(GLuint texSlot);
    void bind(GLuint texSlot);
};

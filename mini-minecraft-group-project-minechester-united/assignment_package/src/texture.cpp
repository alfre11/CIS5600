#include "texture.h"


Texture::Texture(OpenGLContext *context)
    : context(context), m_textureHandle(0), m_textureImage(nullptr)
{}

Texture::~Texture()
{}

// void Texture::create(const char *texturePath) {
//     context->printGLErrorLog();

//     QImage img(texturePath);
//     img.convertToFormat(QImage::Format_ARGB32);
//     img = img.mirrored();
//     m_textureImage = mkU<QImage>(img);

//     context->glGenTextures(1, &m_textureHandle);

//     context->printGLErrorLog();

// }

void Texture::load(GLuint texSlot = 0) {
    context->glActiveTexture(GL_TEXTURE0 + texSlot);
    context->glBindTexture(GL_TEXTURE_2D, m_textureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_textureImage->width(), m_textureImage->height(), 0
                          , GL_BGRA, GL_UNSIGNED_BYTE, m_textureImage->bits());

    context->printGLErrorLog();
}

void Texture::bind(GLuint texSlot = 0) {
    context->printGLErrorLog();
    context->glActiveTexture(GL_TEXTURE0 + texSlot);
    context->glBindTexture(GL_TEXTURE_2D, m_textureHandle);
}

void Texture::create(const char* texturePath) {
    if(texturePath) {
        QImage img(texturePath);

        if(img.isNull()) {
            throw std::runtime_error("Failed to load texture image: " + std::string(texturePath));
        }

        // Ensure the format is compatible
        img = img.convertToFormat(QImage::Format_ARGB32);
        img = img.mirrored();
        m_textureImage = mkU<QImage>(img);

        // Check for power-of-two dimensions
        int width = m_textureImage->width();
        int height = m_textureImage->height();

        if ((width & (width - 1)) != 0 || (height & (height - 1)) != 0) {
            qDebug() << "Warning: Texture atlas dimensions are not power-of-two.";
        }
    }

    // Generate OpenGL texture handle
    context->glGenTextures(1, &m_textureHandle);
}

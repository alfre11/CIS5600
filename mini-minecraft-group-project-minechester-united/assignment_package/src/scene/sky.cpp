#include "sky.h"
#include <vector>

Sky::Sky(OpenGLContext *context)
    : Drawable(context)
{}

Sky::~Sky()
{}

void Sky::createVBOdata() {
    std::vector<glm::vec4> glPos { glm::vec4(-1.f,-1.f,1.f, 1.f),
                                 glm::vec4( 1.f,-1.f,1.f,1.f),
                                 glm::vec4( 1.f, 1.f,1.f,1.f),
                                 glm::vec4(-1.f, 1.f,1.f,1.f) };

    std::vector<glm::vec2> glUV { glm::vec2(0,0),
                                glm::vec2(1,0),
                                glm::vec2(1,1),
                                glm::vec2(0,1) };

    std::vector<GLuint> glIndex {0,1,2,0,2,3};
    indexCounts[INDEX] = 6;

    generateBuffer(POSITION);
    bindBuffer(POSITION);
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * glPos.size(), glPos.data(), GL_STATIC_DRAW);

    generateBuffer(UV);
    bindBuffer(UV);
    mp_context->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * glUV.size(), glUV.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * glIndex.size(), glIndex.data(), GL_STATIC_DRAW);

}

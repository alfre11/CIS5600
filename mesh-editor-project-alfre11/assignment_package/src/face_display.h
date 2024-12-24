#ifndef FACE_DISPLAY_H
#define FACE_DISPLAY_H

#include <drawable.h>
#include <face.h>

class face_display : public Drawable {
protected:
    face* representedFace;

public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    std::vector<unsigned int> indices;
    face_display(OpenGLContext* context);
    void updateFace(face* f);
    void initializeAndBufferGeometryData() override;
    GLenum drawMode() override;
};

#endif // FACE_DISPLAY_H

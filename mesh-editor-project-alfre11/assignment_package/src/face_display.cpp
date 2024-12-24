#include "face_display.h"
#include <halfedge.h>


face_display::face_display(OpenGLContext* context)
    : Drawable(context), representedFace(nullptr) {}

void face_display::updateFace(face* f) {
    representedFace = f;
    initializeAndBufferGeometryData();
}

GLenum face_display::drawMode() {
    return GL_LINE_LOOP;
}

void face_display::initializeAndBufferGeometryData() {
    if (!representedFace) {
        return;
    } else {
        positions.clear();
        colors.clear();
        indices.clear();

        unsigned int index = 0;
        glm::vec3& color = representedFace->face_color;
        glm::vec3 new_color = glm::vec3(1.0f - color.r, 1.0f - color.g, 1.0f - color.b);

        halfedge* hee = representedFace->halfedge;
        do {
            indices.push_back(index);
            index += 1;
            hee = hee->NXT;
           positions.push_back(hee->vertex->vertex_position);
            colors.push_back(new_color);
        } while (hee != representedFace->halfedge);

        generateBuffer(POSITION);
        bindBuffer(POSITION);
        bufferData(POSITION, positions);

        generateBuffer(INDEX);
        bindBuffer(INDEX);
        bufferData(INDEX, indices);
        indexBufferLength = indices.size();

        generateBuffer(COLOR);
        bindBuffer(COLOR);
        bufferData(COLOR, colors);
}
}


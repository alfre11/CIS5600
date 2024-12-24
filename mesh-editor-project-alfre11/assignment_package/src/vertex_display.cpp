#include "vertex_display.h"

vertex_display::vertex_display(OpenGLContext* context)
    : Drawable(context), representedVertex(nullptr)
{}

void vertex_display::updateVertex(vertex* v) {
    representedVertex = v;
    if (v != nullptr) {
        initializeAndBufferGeometryData();
    }
}

GLenum vertex_display::drawMode() {
    return GL_POINTS;
}

void vertex_display::initializeAndBufferGeometryData() {
    if (!representedVertex) {
        return;
    } else {
        std::vector<glm::vec3> vertPos {
                                       representedVertex->vertex_position
        };
        std::vector<glm::vec3> vertColor {
            glm::vec3(1, 1, 1)
        };

        std::vector<unsigned int> indices;
        indices.push_back(0);

        generateBuffer(INDEX);
        bindBuffer(INDEX);
        bufferData(INDEX, indices);

        generateBuffer(POSITION);
        bindBuffer(POSITION);
        bufferData(POSITION, vertPos);

        indexBufferLength = indices.size();

        generateBuffer(COLOR);
        bindBuffer(COLOR);
        bufferData(COLOR, vertColor);

    }
}


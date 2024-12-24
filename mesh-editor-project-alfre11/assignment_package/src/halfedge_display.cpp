#include "halfedge_display.h"


halfedge_display::halfedge_display(OpenGLContext* context)
    : Drawable(context), representedHalfEdge(nullptr)
{}

void halfedge_display::updateHalfEdge(halfedge* he) {
    representedHalfEdge = he;
    if (he != nullptr) {
        initializeAndBufferGeometryData();
    }
}

GLenum halfedge_display::drawMode() {
    return GL_LINES;
}


void halfedge_display::initializeAndBufferGeometryData() {
    if (!representedHalfEdge) {
        return;
    } else {

        edge_pos.clear();
        edgeColors.clear();
        indices.clear();

        unsigned int index = 0;


        edge_pos.push_back(representedHalfEdge->vertex->vertex_position);
        edge_pos.push_back(representedHalfEdge->SYM->vertex->vertex_position);

        indices.push_back(index);
        index += 1;
        indices.push_back(index);
        index += 1;
        edgeColors.push_back(glm::vec3(1.f, 0, 0));
        edgeColors.push_back(glm::vec3(1.f, 1.f, 0));

        generateBuffer(POSITION);
        bindBuffer(POSITION);
        bufferData(POSITION, edge_pos);

        generateBuffer(INDEX);
        bindBuffer(INDEX);
        bufferData(INDEX, indices);
        indexBufferLength = indices.size();

        generateBuffer(COLOR);
        bindBuffer(COLOR);
        bufferData(COLOR, edgeColors);
    }
}


#include "vertex.h"


unsigned vertex::counter = 1;

vertex::vertex(const glm::vec3& position):vertex_position(position),
    halfedge(nullptr){
    code = counter;
    counter += 1;
    setText(QString::fromStdString("Vert ID -" + std::to_string(code)));
}

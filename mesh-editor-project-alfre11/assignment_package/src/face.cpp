#include "face.h"

unsigned face::counter = 1;

face::face(const glm::vec3& color):face_color(color),
    halfedge(nullptr) {
    code = counter;
    counter += 1;
    setText(QString::fromStdString("Face ID -" + std::to_string(code)));
}

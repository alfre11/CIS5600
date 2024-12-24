#ifndef FACE_H
#define FACE_H

#include <QListWidgetItem>
#include <glm/glm.hpp>

class halfedge;

class face : public QListWidgetItem{
public:
    static unsigned int counter;
    glm::vec3 face_color;
    halfedge* halfedge;
    unsigned int code;
    face(const glm::vec3& color);
};

#endif // FACE_H

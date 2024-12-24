#ifndef VERTEX_H
#define VERTEX_H

#include <QListWidgetItem>
#include <glm/glm.hpp>

class halfedge;

class vertex : public QListWidgetItem {
public:
    static unsigned int counter;
    halfedge* halfedge;
    unsigned int code;
    glm::vec3 vertex_position;

    vertex(const glm::vec3& position);
};

#endif // VERTEX_H

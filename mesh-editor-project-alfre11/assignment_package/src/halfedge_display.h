#ifndef HALFEDGE_DISPLAY_H
#define HALFEDGE_DISPLAY_H


#include <drawable.h>
#include <halfedge.h>

class halfedge_display : public Drawable {
protected:
    halfedge* representedHalfEdge;

public:

    std::vector<glm::vec3> edge_pos;
    std::vector<glm::vec3> edgeColors;
    std::vector<unsigned int> indices;
    halfedge_display(OpenGLContext* context);
    void updateHalfEdge(halfedge* he);
    void initializeAndBufferGeometryData() override;
    GLenum drawMode() override;
};

#endif // HALFEDGE_DISPLAY_H

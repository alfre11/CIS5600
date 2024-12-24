#ifndef VERTEX_DISPLAY_H
#define VERTEX_DISPLAY_H
#include <vertex.h>
#include <drawable.h>


class vertex_display : public Drawable
{
protected:
    vertex *representedVertex;
public:
    vertex_display(OpenGLContext*);
    void updateVertex(vertex*);
    GLenum drawMode() override;
    void initializeAndBufferGeometryData() override;
};


#endif // VERTEX_DISPLAY_H

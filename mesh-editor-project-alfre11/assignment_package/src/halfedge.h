#ifndef HALFEDGE_H
#define HALFEDGE_H
#include <vertex.h>
#include <face.h>


class halfedge: public QListWidgetItem {
public:
    static unsigned int counter;
    halfedge* NXT;
    halfedge* SYM;
    face* face;
    vertex* vertex;
    long long code;
    halfedge();
};

#endif // HALFEDGE_H

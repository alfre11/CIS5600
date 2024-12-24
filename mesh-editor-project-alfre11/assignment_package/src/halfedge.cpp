#include "halfedge.h"


unsigned int halfedge::counter = 1;

halfedge::halfedge(): NXT(nullptr), SYM(nullptr), face(nullptr), vertex(nullptr){
    code = counter;
    counter += 1;

    setText(QString::fromStdString("Half Edge ID -"  + std::to_string(code)));
}

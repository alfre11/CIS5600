#ifndef SEGMENT_H
#define SEGMENT_H

#include <glm/glm.hpp>

#endif // SEGMENT_H

class Segment {
private:
    glm::vec2 p1, p2;
    float slope;
    bool vertical;

public:
    Segment(glm::vec2 end1, glm::vec2 end2);

    bool getIntersection(float y, float* x);



};



#include <cmath>
#include <glm/glm.hpp>
#include <segment.h>

    // Constructor with initialization list
    Segment::Segment(glm::vec2 end1, glm::vec2 end2)
        : p1(end1), p2(end2), vertical(false){
        if (p2.x != p1.x) {
            slope = (p2.y - p1.y) / (p2.x - p1.x);
        } else {
            slope = 0;
            vertical = true;
        }
    }

    bool Segment::getIntersection(float y_coord, float* x_intersect)  {
        if (vertical) {
            if (y_coord >= std::min(p1.y, p2.y) && y_coord <= std::max(p1.y, p2.y)) {
                *x_intersect = p1.x;
                return true;
            }
            return false;
        }

        if(slope == 0) {
            return false;
        }

        if (y_coord < std::min(p1.y, p2.y) || y_coord > std::max(p1.y, p2.y)) {
            return false;
        }

        *x_intersect = p1.x + (y_coord - p1.y) / slope;
        return true;
    }

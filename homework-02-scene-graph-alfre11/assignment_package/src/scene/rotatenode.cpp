#include "node.h"
#include "rotatenode.h"

RotateNode::RotateNode(float angle, QString name, const glm::vec3 &color, Polygon2D *poly) : Node(poly, name), ang(angle) {
    setColor(color);
}

RotateNode::RotateNode(RotateNode const &r) : Node(r.pol, r.name), ang(r.ang) {
    setColor(r.color);
}

RotateNode& RotateNode::operator= (const RotateNode &other)
{
    if (this == &other) {
        return *this;
    }

    Node::operator=(other);
    ang = other.ang;
    return *this;
}

RotateNode::~RotateNode() {}

glm::mat3 RotateNode::getTransformationMatrix() const {

    return glm::rotate(glm::mat3(), glm::radians(ang));

}

float RotateNode::angle() {
    return ang;
}

void RotateNode::setAngle(float newAngle) {
    ang = newAngle;
}

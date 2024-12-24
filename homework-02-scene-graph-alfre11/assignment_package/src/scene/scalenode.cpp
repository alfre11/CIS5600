#include "node.h"
#include "scalenode.h"

ScaleNode::ScaleNode(float xScale, float yScale, QString name, const glm::vec3 &color, Polygon2D *poly) : Node(poly, name), xSc(xScale), ySc(yScale)
{
    setColor(color);
}

ScaleNode::ScaleNode(ScaleNode const &s) : Node(s.pol, s.name), xSc(s.xSc), ySc(s.ySc)
{}

ScaleNode& ScaleNode::operator= (const ScaleNode &other)
{
    if (this == &other) {
        return *this;
    }

    Node::operator=(other);
    xSc = other.xSc;
    ySc = other.ySc;
    return *this;
}

ScaleNode::~ScaleNode() {}

glm::mat3 ScaleNode::getTransformationMatrix() const {
    return glm::scale(glm::mat3(), glm::vec2(xSc, ySc));
}

float ScaleNode::getXsc() {
    return xSc;
}

void ScaleNode::setXsc(float newX) {
    xSc = newX;
}

float ScaleNode::getYsc() {
    return ySc;
}

void ScaleNode::setYsc(float newY) {
    ySc = newY;
}

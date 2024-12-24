#include "node.h"
#include "translatenode.h"

TranslateNode::TranslateNode(float xTrans, float yTrans, QString name, const glm::vec3 &color, Polygon2D *poly) : Node(poly, name), xTr(xTrans), yTr(yTrans)
{
    setColor(color);
}

TranslateNode::TranslateNode(TranslateNode const &t) : Node(t.pol, t.name), xTr(t.xTr), yTr(t.yTr)
{}

TranslateNode& TranslateNode::operator= (const TranslateNode& other) {
    if (this == &other) {
        return *this;
    }

    Node::operator=(other);
    xTr = other.xTr;
    yTr = other.yTr;
    return *this;
}

TranslateNode::~TranslateNode() {}

glm::mat3 TranslateNode::getTransformationMatrix() const {
    return glm::translate(glm::mat3(), glm::vec2(xTr, yTr));
}

float TranslateNode::getXt() {
    return xTr;
}

void TranslateNode::setXt(float newXt) {
    xTr = newXt;
}

float TranslateNode::getYt() {
    return yTr;
}

void TranslateNode::setYt(float newYt) {
    yTr = newYt;
}

#include "node.h"

Node::Node(Polygon2D* poly, const QString& name) : pol(poly), name(name) {
    setText(0, name);
}

Node::Node(const Node &n) : pol(n.pol), name(n.name)
{
    setColor(n.color);
    setText(0, name);
}

Node& Node::operator=(const Node& other) {
    if (this == &other) {
        return *this;
    }

    pol = other.pol;
    color = other.color;
    name = other.name;
    // children = other.children;

    return *this;
}

Node::~Node() {
    while(children.empty()) {
        children.pop_back();
    }
}

Node& Node::addChild(uPtr<Node> child) {
    Node& ptr = *child;
    QTreeWidgetItem::addChild(child.get());
    this->children.push_back(std::move(child));
    return ptr;
}

void Node::setColor(const glm::vec3& newColor) {
    color = newColor;
}

std::vector<uPtr<Node>>& Node::getChildren() {
    return this->children;
}

Node* Node::get() {
    return this;
}

Polygon2D* Node::getPolygon() {
    return this->pol;
}

void Node::setPolygon(Polygon2D* newPol) {
    pol = newPol;
}

glm::vec3 Node::getColor() {
    return this->color;
}

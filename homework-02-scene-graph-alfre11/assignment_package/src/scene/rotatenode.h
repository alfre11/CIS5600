#pragma once
#include "node.h"

#ifndef ROTATENODE_H
#define ROTATENODE_H

#endif // ROTATENODE_H

class RotateNode : public Node {
protected:
    float ang;
public:
    RotateNode(float angle, QString name, const glm::vec3& color, Polygon2D* poly);

    RotateNode(RotateNode const &r);

    RotateNode& operator= (const RotateNode &other);

    ~RotateNode();

    glm::mat3 getTransformationMatrix() const;

    float angle();

    void setAngle(float newAngle);
};

#pragma once
#include "node.h"

#ifndef SCALENODE_H
#define SCALENODE_H

#endif // SCALENODE_H

class ScaleNode : public Node {
protected:
    float xSc;
    float ySc;
public:
    ScaleNode(float xScale, float yScale, QString name, const glm::vec3 &color, Polygon2D* poly);

    ScaleNode(ScaleNode const &s);

    ScaleNode& operator= (const ScaleNode &other);

    ~ScaleNode();

    glm::mat3 getTransformationMatrix() const;

    float getXsc();

    void setXsc(float newX);

    float getYsc();

    void setYsc(float newY);
};

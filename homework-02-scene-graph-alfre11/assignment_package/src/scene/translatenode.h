#pragma once
#include "node.h"

#ifndef TRANSLATENODE_H
#define TRANSLATENODE_H

#endif // TRANSLATENODE_H

class TranslateNode : public Node {
protected:
    float xTr;
    float yTr;
public:
    TranslateNode(float xTrans, float yTrans, QString name, const glm::vec3 &color, Polygon2D* poly);

    TranslateNode(TranslateNode const &t);

    TranslateNode& operator= (const TranslateNode& other);

    ~TranslateNode();

    glm::mat3 getTransformationMatrix() const;

    float getXt();

    void setXt(float newXt);

    float getYt();

    void setYt(float newYt);
};

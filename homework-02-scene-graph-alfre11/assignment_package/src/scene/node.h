#pragma once
#include "scene/polygon.h"
#include <QTreeWidgetItem>
#include <smartpointerhelp.h>
#include "polygon.h"

class Node : public QTreeWidgetItem {
    protected:
        std::vector<uPtr<Node>> children;
        Polygon2D* pol;
        glm::vec3 color;
        QString name;
    public:
        Node(Polygon2D* poly, const QString& name);

        Node(Node const &n);

        Node& operator=(const Node& other);

        virtual ~Node() = 0;

        virtual glm::mat3 getTransformationMatrix() const = 0;

        Node& addChild(std::unique_ptr<Node> child);

        void setColor(const glm::vec3& newColor);

        std::vector<uPtr<Node>>& getChildren();

        Node* get();

        Polygon2D* getPolygon();

        void setPolygon(Polygon2D*);

        glm::vec3 getColor();
};

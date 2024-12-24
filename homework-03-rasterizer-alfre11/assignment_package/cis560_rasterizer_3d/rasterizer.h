#pragma once
#include "camera.h"
#include <polygon.h>
#include <QImage>

class Rasterizer
{
private:
    //This is the set of Polygons loaded from a JSON scene file
    std::vector<Polygon> m_polygons;
    // std::vector<float> zVals;
public:
    Camera cam;
    Rasterizer(const std::vector<Polygon>& polygons, Camera &cam);
    QImage RenderScene();
    void ClearScene();
    QColor findColor(float px, float py, const Polygon&p, int triIndex);
    void RenderPolygon(const Polygon& p, QImage &im, std::vector<float>& zVals);
    void RenderTriangle(const Polygon& p, int index, QImage &im, std::vector<float>& zVals);
    float findZ(float x, float y, const Polygon&p, int triIndex);
    float findZ3D(float x, float y, const Polygon&p, int triIndex);
    glm::vec2 findUV3D(float x, float y, const Polygon&p, int triIndex, float zp);
    glm::vec3 CalculateLambertianShading(const glm::vec4 &normal, const glm::vec3& surfaceColor);
    glm::vec4 find3DNorm(float x, float y, const Polygon&p, int triIndex, float zp);
};

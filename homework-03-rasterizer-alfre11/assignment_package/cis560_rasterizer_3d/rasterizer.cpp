#include "rasterizer.h"
#include "segment.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons, Camera& camera)
    : m_polygons(polygons), cam(camera)
{}

QImage Rasterizer::RenderScene()
{
    // std::cout << "rendering scene " << std::endl;
    QImage result(512, 512, QImage::Format_RGB32);
    std::vector<float> zVals(512*512, -1);
    // Fill the image with black pixels.
    // Note that qRgb creates a QColor,
    // and takes in values [0, 255] rather than [0, 1].
    result.fill(qRgb(0.f, 0.f, 0.f));
    // TODO: Complete the various components of code that make up this function.
    // It should return the rasterized image of the current scene.

    // Make liberal use of helper functions; writing your rasterizer as one
    // long RenderScene function will make it (a) hard to debug and
    // (b) hard to write without copy-pasting. Also, Adam will be sad when
    // he reads your code.

    // Also! As per the style requirements for this assignment, make sure you
    // use std::arrays to store things like your line segments, Triangles, and
    // vertex coordinates. This lets you easily use loops to perform operations
    // on your scene components, rather than copy-pasting operations three times
    // each!

    // Camera cam = Camera();
    std::vector<Polygon> polygonsTrans = m_polygons;

    for(Polygon &p : polygonsTrans) {



        for(Vertex &v : p.m_verts) {
            // std::cout << "new vertex" << std::endl;
            v.m_pos = cam.viewMat() * v.m_pos;
            v.m_pos = cam.perspProj() * v.m_pos;
            if(v.m_pos[3] != 0) {
                v.m_pos = v.m_pos / v.m_pos[3];
            }
            v.m_pos[0] = ((v.m_pos[0] + 1) / 2) * 512;
            v.m_pos[1] = ((1 - v.m_pos[1]) / 2) * 512;
            // std::cout << "vertex pos: " << v.m_pos[0] << ", " << v.m_pos[1] << std::endl;
        }
        // std::cout << "new polygon" << std::endl;
        RenderPolygon(p, result, zVals);
    }

    return result;
}

QColor Rasterizer::findColor(float px, float py, const Polygon&p, int triIndex) {
    Vertex p1 = p.m_verts[p.m_tris[triIndex].m_indices[0]];
    Vertex p2 = p.m_verts[p.m_tris[triIndex].m_indices[1]];
    Vertex p3 = p.m_verts[p.m_tris[triIndex].m_indices[2]];


    glm::vec3 p1p2 = glm::vec3(p1.m_pos[0] - p2.m_pos[0], p1.m_pos[1] - p2.m_pos[1], 0);
    glm::vec3 p3p2 = glm::vec3(p3.m_pos[0] - p2.m_pos[0], p3.m_pos[1] - p2.m_pos[1], 0);

    float totArea = 0.5 * glm::length(glm::cross(p1p2, p3p2));

    glm::vec3 pp2 = glm::vec3(px - p2.m_pos[0], py - p2.m_pos[1], 0);

    float s1 = 0.5 * glm::length(glm::cross(p3p2, pp2));

    float s3 = 0.5 * glm::length(glm::cross(p1p2, pp2));

    glm::vec3 p1p3 = glm::vec3(p1.m_pos[0] - p3.m_pos[0], p1.m_pos[1] - p3.m_pos[1], 0);
    glm::vec3 pp3 = glm::vec3(px - p3.m_pos[0], py - p3.m_pos[1], 0);

    float s2 = 0.5 * glm::length(glm::cross(p1p3, pp3));

    float c1 = (s1/totArea) * p1.m_color[0] + (s2/totArea) * p2.m_color[0] + (s3/totArea) * p3.m_color[0];
    float c2 = (s1/totArea) * p1.m_color[1] + (s2/totArea) * p2.m_color[1] + (s3/totArea) * p3.m_color[1];
    float c3 = (s1/totArea) * p1.m_color[2] + (s2/totArea) * p2.m_color[2] + (s3/totArea) * p3.m_color[2];

    return QColor(c1, c2, c3);
}


void Rasterizer::RenderPolygon(const Polygon& p, QImage& im, std::vector<float>& zVals) {
    int index = 0;
    for(Triangle t : p.m_tris) {
        // std::cout << "new triangle " << index << std::endl;
        RenderTriangle(p, index, im, zVals);
        ++index;
    }
}

void Rasterizer::RenderTriangle(const Polygon& p, int index, QImage &im, std::vector<float>& zVals) {
    // std::cout << "rendering triangle " << std::endl;
    //bounding box
    float minX;
    float minY;
    float maxX;
    float maxY;

    Triangle t = p.TriAt(index);

    p.getBoundBox(t, &minX, &minY, &maxX, &maxY);

    //check image edges
    if(minX < 0) {
        minX = 0;
    }
    if(minY < 0) {
        minY = 0;
    }
    if(maxX > 511) {
        maxX = 511;
    }
    if(maxY > 511) {
        maxY = 511;
    }

    Vertex p1 = p.m_verts[t.m_indices[0]];
    Vertex p2 = p.m_verts[t.m_indices[1]];
    Vertex p3 = p.m_verts[t.m_indices[2]];

    std::array<Segment, 3> segArray = {Segment(glm::vec2(p1.m_pos.x, p1.m_pos.y), glm::vec2(p2.m_pos.x, p2.m_pos.y)),
                                       Segment(glm::vec2(p2.m_pos.x, p2.m_pos.y), glm::vec2(p3.m_pos.x, p3.m_pos.y)),
                                       Segment(glm::vec2(p3.m_pos.x, p3.m_pos.y), glm::vec2(p1.m_pos.x, p1.m_pos.y))};


    // long county = 0;

    // std::cout << "minY: " << minY << std::endl;
    // std::cout << "maxY: " << maxY << std::endl;
    for(int i = std::ceil(minY); i < std::ceil(maxY); i++) {
        float xLeft = 512;
        float xRight = 0;
        for(int j = 0; j < 3; j++) {
            float x;
            bool intersec = segArray[j].getIntersection(i, &x);
            if(intersec) {
                xLeft = std::min(xLeft, x);
                xRight = std::max(xRight, x);
            }
        }
        // std::cout << "xLeft: " << xLeft;
        // std::cout << "xRight: " << xRight;
        if(xLeft < 0) {
            xLeft = 0;
        }
        if(xRight > 511) {
            xRight = 511;
        }
        for(int k = std::ceil(xLeft); k < std::ceil(xRight); k++) {
            // std::cout << "setting pixel " << county << std::endl;

            if(k <= 511 && k >= 0 && i <= 511 && i >= 0) {

                float newZ = findZ3D(k, i, p, index);
                glm::vec2 uv = findUV3D(k, i, p, index, newZ);
                if(uv[0] < 0 || uv[0] > 1 || uv[1] < 0 || uv[1] > 1) {
                    // std::cout << "uv out of bounds" << std::endl;
                } else {
                    glm::vec3 colVec = GetImageColor(uv, p.mp_texture);
                    // QColor col = QColor(colVec[0], colVec[1], colVec[2]);

                    glm::vec4 normal = find3DNorm(k, i, p, index, newZ);


                    //Lambertian Shading
                    glm::vec3 shadedColVec = CalculateLambertianShading(normal, colVec);
                    QColor shadedCol = QColor(shadedColVec[0], shadedColVec[1], shadedColVec[2]);

                    // std::cout << "uv X: " << uv[0] << ", uv Y: " << uv[1] << std::endl;
                    // glm::vec2 uv = glm::vec2(100,100);

                    //no overlap

                    if(zVals[k + 512*i] < 0 || zVals[k + 512*i] > newZ) {
                        zVals[k + 512*i] = newZ;

                        // im.setPixelColor(k, i, findColor(k, i, p, index));

                        // std::cout << "x: " << k << std::endl;
                        // std::cout << "y " << i << std::endl;
                        im.setPixelColor(k, i, shadedCol);
                    }
                }


            }

        }

    }
    // std::cout << county << std::endl;

}

glm::vec4 Rasterizer::find3DNorm(float x, float y, const Polygon&p, int triIndex, float zp) {
    Vertex p1 = p.m_verts[p.m_tris[triIndex].m_indices[0]];
    Vertex p2 = p.m_verts[p.m_tris[triIndex].m_indices[1]];
    Vertex p3 = p.m_verts[p.m_tris[triIndex].m_indices[2]];



    glm::vec3 p1p2 = glm::vec3(p1.m_pos[0] - p2.m_pos[0], p1.m_pos[1] - p2.m_pos[1], 0);
    glm::vec3 p3p2 = glm::vec3(p3.m_pos[0] - p2.m_pos[0], p3.m_pos[1] - p2.m_pos[1], 0);

    float totArea = 0.5 * glm::length(glm::cross(p1p2, p3p2));

    glm::vec3 pp2 = glm::vec3(x - p2.m_pos[0], y - p2.m_pos[1], 0);

    float s1 = 0.5 * glm::length(glm::cross(p3p2, pp2));

    float s3 = 0.5 * glm::length(glm::cross(p1p2, pp2));

    glm::vec3 p1p3 = glm::vec3(p1.m_pos[0] - p3.m_pos[0], p1.m_pos[1] - p3.m_pos[1], 0);
    glm::vec3 pp3 = glm::vec3(x - p3.m_pos[0], y - p3.m_pos[1], 0);

    float s2 = 0.5 * glm::length(glm::cross(p1p3, pp3));

    float NormP1 = zp * ((s1/totArea) * (p1.m_normal[0]/p1.m_pos[2]) + (s2/totArea) * (p2.m_normal[0]/p2.m_pos[2]) + (s3/totArea) * (p3.m_normal[0]/p3.m_pos[2]));
    float NormP2 = zp * ((s1/totArea) * (p1.m_normal[1]/p1.m_pos[2]) + (s2/totArea) * (p2.m_normal[1]/p2.m_pos[2]) + (s3/totArea) * (p3.m_normal[1]/p3.m_pos[2]));
    float NormP3 = zp * ((s1/totArea) * (p1.m_normal[2]/p1.m_pos[2]) + (s2/totArea) * (p2.m_normal[2]/p2.m_pos[2]) + (s3/totArea) * (p3.m_normal[2]/p3.m_pos[2]));
    float NormP4 = zp * ((s1/totArea) * (p1.m_normal[3]/p1.m_pos[2]) + (s2/totArea) * (p2.m_normal[3]/p2.m_pos[2]) + (s3/totArea) * (p3.m_normal[3]/p3.m_pos[2]));

    glm::vec4 Norm = glm::vec4(NormP1, NormP2, NormP3, NormP4);
    return Norm;
}

glm::vec3 Rasterizer::CalculateLambertianShading(const glm::vec4 &normal, const glm::vec3& surfaceColor) {

    glm::vec4 camFor = cam.getForward();
    // glm::vec3 lightVec = glm::vec3(camFor[0], camFor[1], camFor[2]);

    glm::vec4 lightDir = glm::normalize(camFor) * -1.0f;
    float ambient = 0.3f;

    // std::cout << "normal[0]: " << normal[0] << " normal[1]: " << normal[1] << " normal[2]: " << normal[2] << std::endl;
    // std::cout << "light[0]: " << lightDir[0] << " light[1]: " << lightDir[1] << " light[2]: " << lightDir[2] << std::endl;


    float diffuse = glm::dot(normal, lightDir);

    float intensity = ambient + diffuse;

    // std::cout << "intesity: " << intensity << std::endl;

    return surfaceColor * glm::clamp(intensity, 0.f, 1.f);
    // return surfaceColor * intensity;
}

// float Rasterizer::findZ(float x, float y, const Polygon&p, int triIndex) {
//     Vertex p1 = p.m_verts[p.m_tris[triIndex].m_indices[0]];
//     Vertex p2 = p.m_verts[p.m_tris[triIndex].m_indices[1]];
//     Vertex p3 = p.m_verts[p.m_tris[triIndex].m_indices[2]];

//     glm::vec3 p1p2 = glm::vec3(p1.m_pos[0] - p2.m_pos[0], p1.m_pos[1] - p2.m_pos[1], 0);
//     glm::vec3 p3p2 = glm::vec3(p3.m_pos[0] - p2.m_pos[0], p3.m_pos[1] - p2.m_pos[1], 0);

//     float totArea = 0.5 * glm::length(glm::cross(p1p2, p3p2));

//     glm::vec3 pp2 = glm::vec3(x - p2.m_pos[0], y - p2.m_pos[1], 0);

//     float s1 = 0.5 * glm::length(glm::cross(p3p2, pp2));

//     float s3 = 0.5 * glm::length(glm::cross(p1p2, pp2));

//     glm::vec3 p1p3 = glm::vec3(p1.m_pos[0] - p3.m_pos[0], p1.m_pos[1] - p3.m_pos[1], 0);
//     glm::vec3 pp3 = glm::vec3(x - p3.m_pos[0], y - p3.m_pos[1], 0);

//     float s2 = 0.5 * glm::length(glm::cross(p1p3, pp3));

//     float z = (s1/totArea) * p1.m_pos[2] + (s2/totArea) * p2.m_pos[2] + (s3/totArea) * p3.m_pos[2];
//     return z;
// }

float Rasterizer::findZ3D(float x, float y, const Polygon&p, int triIndex) {
    Vertex p1 = p.m_verts[p.m_tris[triIndex].m_indices[0]];
    Vertex p2 = p.m_verts[p.m_tris[triIndex].m_indices[1]];
    Vertex p3 = p.m_verts[p.m_tris[triIndex].m_indices[2]];



    glm::vec3 p1p2 = glm::vec3(p1.m_pos[0] - p2.m_pos[0], p1.m_pos[1] - p2.m_pos[1], 0);
    glm::vec3 p3p2 = glm::vec3(p3.m_pos[0] - p2.m_pos[0], p3.m_pos[1] - p2.m_pos[1], 0);

    float totArea = 0.5 * glm::length(glm::cross(p1p2, p3p2));

    glm::vec3 pp2 = glm::vec3(x - p2.m_pos[0], y - p2.m_pos[1], 0);

    float s1 = 0.5 * glm::length(glm::cross(p3p2, pp2));

    float s3 = 0.5 * glm::length(glm::cross(p1p2, pp2));

    glm::vec3 p1p3 = glm::vec3(p1.m_pos[0] - p3.m_pos[0], p1.m_pos[1] - p3.m_pos[1], 0);
    glm::vec3 pp3 = glm::vec3(x - p3.m_pos[0], y - p3.m_pos[1], 0);

    float s2 = 0.5 * glm::length(glm::cross(p1p3, pp3));

    float z = 1/((s1/totArea) * (1/p1.m_pos[2]) + (s2/totArea) * (1/p2.m_pos[2]) + (s3/totArea) * (1/p3.m_pos[2]));
    return z;
}

glm::vec2 Rasterizer::findUV3D(float x, float y, const Polygon&p, int triIndex, float zp) {
    Vertex p1 = p.m_verts[p.m_tris[triIndex].m_indices[0]];
    Vertex p2 = p.m_verts[p.m_tris[triIndex].m_indices[1]];
    Vertex p3 = p.m_verts[p.m_tris[triIndex].m_indices[2]];



    glm::vec3 p1p2 = glm::vec3(p1.m_pos[0] - p2.m_pos[0], p1.m_pos[1] - p2.m_pos[1], 0);
    glm::vec3 p3p2 = glm::vec3(p3.m_pos[0] - p2.m_pos[0], p3.m_pos[1] - p2.m_pos[1], 0);

    float totArea = 0.5 * glm::length(glm::cross(p1p2, p3p2));

    glm::vec3 pp2 = glm::vec3(x - p2.m_pos[0], y - p2.m_pos[1], 0);

    float s1 = 0.5 * glm::length(glm::cross(p3p2, pp2));

    float s3 = 0.5 * glm::length(glm::cross(p1p2, pp2));

    glm::vec3 p1p3 = glm::vec3(p1.m_pos[0] - p3.m_pos[0], p1.m_pos[1] - p3.m_pos[1], 0);
    glm::vec3 pp3 = glm::vec3(x - p3.m_pos[0], y - p3.m_pos[1], 0);

    float s2 = 0.5 * glm::length(glm::cross(p1p3, pp3));

    float uvX = zp * ((s1/totArea) * (p1.m_uv[0]/p1.m_pos[2]) + (s2/totArea) * (p2.m_uv[0]/p2.m_pos[2]) + (s3/totArea) * (p3.m_uv[0]/p3.m_pos[2]));
    float uvY = zp * ((s1/totArea) * (p1.m_uv[1]/p1.m_pos[2]) + (s2/totArea) * (p2.m_uv[1]/p2.m_pos[2]) + (s3/totArea) * (p3.m_uv[1]/p3.m_pos[2]));
    glm::vec2 uv = glm::vec2(uvX, uvY);
    return uv;
}

void Rasterizer::ClearScene()
{
    m_polygons.clear();
}

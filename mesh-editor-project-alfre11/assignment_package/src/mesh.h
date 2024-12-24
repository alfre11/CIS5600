#ifndef MESH_H
#define MESH_H
#include<drawable.h>
#include<halfedge.h>
#include<vertex.h>
#include<face.h>

class mesh : public Drawable{
public:
    std::vector<std::unique_ptr<face>> face_list;
    std::vector<std::unique_ptr<vertex>> vertex_list;
    std::vector<std::unique_ptr<halfedge>> hedge_list;

    std::vector<glm::vec3> init_positions;
    std::vector<glm::vec3> init_normals;
    std::vector<glm::vec3> init_colors;
    std::vector<unsigned int> init_indices;

    void initializeAndBufferGeometryData() override;
    mesh(OpenGLContext *graphicsContext) : Drawable(graphicsContext) {}
};
#endif // MESH_H

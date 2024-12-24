#include "mesh.h"

void mesh::initializeAndBufferGeometryData() {

    init_positions.clear();
    init_normals.clear();
    init_colors.clear();
    init_indices.clear();
    unsigned int index = 0;

    for (const auto &f : face_list) {
        std::vector<unsigned int> face_index;
        std::vector<glm::vec3> face_postions;
        halfedge *begin = f->halfedge;
        halfedge *atm = begin;
        do {
            init_normals.push_back(glm::normalize(glm::cross(
                atm->NXT->vertex->vertex_position - atm->vertex->vertex_position,
                atm->NXT->NXT->vertex->vertex_position - atm->NXT->vertex->vertex_position
                )));

            init_positions.push_back(atm->vertex->vertex_position);
            init_colors.push_back(f->face_color);

            face_index.push_back(index);
            index += 1;

            atm = atm->NXT;
        } while (atm != begin);

        for (unsigned int i = 1; i < face_index.size(); i++) {
            if (i == face_index.size() - 1) {
                continue;
            }
            init_indices.push_back(face_index[0]);
            init_indices.push_back(face_index[i]);
            init_indices.push_back(face_index[i + 1]);
        }
    }

    generateBuffer(BufferType::POSITION);
    bindBuffer(BufferType::POSITION);
    bufferData(BufferType::POSITION, init_positions);
    generateBuffer(BufferType::COLOR);
    bindBuffer(BufferType::COLOR);
    bufferData(BufferType::COLOR, init_colors);
    generateBuffer(BufferType::NORMAL);
    bindBuffer(BufferType::NORMAL);
    bufferData(BufferType::NORMAL, init_normals);
    generateBuffer(BufferType::INDEX);
    bindBuffer(BufferType::INDEX);
    bufferData(BufferType::INDEX, init_indices);
    indexBufferLength = init_indices.size();
}

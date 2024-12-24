#include "glm/gtx/transform.hpp"
#include <camera.h>
#include <glm/glm.hpp>

Camera::Camera()
    : forward(glm::vec4(0.f,0.f,-1.f,0.f)), right(glm::vec4(1.f,0.f,0.f,0.f)), up(glm::vec4(0.f,1.f,0.f,0.f)), fov(45.f), pos(glm::vec4(0.f,0.f,10.f,1.f)), nearClipPlane(0.01f), farClipPlane(100.0f), aspectRatio(1.0f)
{}

glm::mat4 Camera::viewMat() {

    glm::vec4 col1 = glm::vec4(this->right[0], this->up[0], this->forward[0], 0.f);
    glm::vec4 col2 = glm::vec4(this->right[1], this->up[1], this->forward[1], 0.f);
    glm::vec4 col3 = glm::vec4(this->right[2], this->up[2], this->forward[2], 0.f);
    glm::vec4 col4 = glm::vec4(0.f, 0.f, 0.f, 1.f);

    glm::mat4 o = glm::mat4(col1, col2, col3, col4);

    glm::vec4 tcol1 = glm::vec4(1.f, 0.f, 0.f, 0.f);
    glm::vec4 tcol2 = glm::vec4(0.f, 1.f, 0.f, 0.f);
    glm::vec4 tcol3 = glm::vec4(0.f, 0.f, 1.f, 0.f);
    glm::vec4 tcol4 = glm::vec4(-pos[0], -pos[1], -pos[2], 1.f);

    glm::mat4 t = glm::mat4(tcol1, tcol2, tcol3, tcol4);
    glm::mat4 v = o * t;
    return v;
}

glm::mat4 Camera::perspProj() {
    float s = 1 / (std::tan(this->fov / 2));
    float p = this->farClipPlane/(this->farClipPlane - this->nearClipPlane);
    float q = -1 * this->farClipPlane * this->nearClipPlane / (this->farClipPlane - this->nearClipPlane);

    glm::vec4 col1 = glm::vec4(s / this->aspectRatio, 0.f, 0.f, 0.f);
    glm::vec4 col2 = glm::vec4(0.f, s, 0.f, 0.f);
    glm::vec4 col3 = glm::vec4(0.f, 0.f, p, 1.f);
    glm::vec4 col4 = glm::vec4(0.f, 0.f, q, 0.f);

    return glm::mat4(col1, col2, col3, col4);
}

void Camera::translateZ(float change) {
    this->pos += change * this->forward;
}

void Camera::translateX(float change) {
    this->pos += change * this->right;
}

void Camera::translateY(float change) {
    this->pos += change * this->up;
}

void Camera::rotateZ(float angle) {
    glm::vec3 for3 = glm::vec3(this->forward[0], this->forward[1], this->forward[2]);

    glm::mat4 rot = glm::rotate(glm::mat4(), angle, for3);
    this->right = rot * this->right;
    this->up = rot * this->up;

}

void Camera::rotateX(float angle) {
    glm::vec3 r3 = glm::vec3(this->right[0], this->right[1], this->right[2]);

    glm::mat4 rot = glm::rotate(glm::mat4(), angle, r3);
    this->forward = rot * this->forward;
    this->up = rot * this->up;
}

void Camera::rotateY(float angle) {
    glm::vec3 up3 = glm::vec3(this->up[0], this->up[1], this->up[2]);

    glm::mat4 rot = glm::rotate(glm::mat4(), angle, up3);
    this->right = rot * this->right;
    this->forward = rot * this->forward;
}

glm::vec4 Camera::getForward() {
    return this->forward;
}

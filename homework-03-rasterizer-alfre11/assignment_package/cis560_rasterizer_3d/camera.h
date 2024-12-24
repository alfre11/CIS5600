#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#endif // CAMERA_H

class Camera {
private:
    glm::vec4 forward;
    glm::vec4 right;
    glm::vec4 up;
    float fov;
    glm::vec4 pos;
    float nearClipPlane;
    float farClipPlane;
    float aspectRatio;

public:
    Camera();

    glm::mat4 viewMat();

    glm::mat4 perspProj();

    void translateZ(float change);

    void translateX(float change);

    void translateY(float change);

    void rotateZ(float angle);

    void rotateX(float angle);

    void rotateY(float angle);

    glm::vec4 getForward();
};

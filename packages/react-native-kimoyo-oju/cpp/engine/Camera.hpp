#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace kimoyooju {

class Camera {
public:
    Camera();

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::quat& rotation);
    void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));

    void setFov(float fov) { fov_ = fov; }
    void setNearClip(float near) { nearClip_ = near; }
    void setFarClip(float far) { farClip_ = far; }

    glm::vec3 getPosition() const { return position_; }
    glm::quat getRotation() const { return rotation_; }
    float getFov() const { return fov_; }
    float getNearClip() const { return nearClip_; }
    float getFarClip() const { return farClip_; }

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;

private:
    glm::vec3 position_{0.0f, 0.0f, 0.0f};
    glm::quat rotation_{1.0f, 0.0f, 0.0f, 0.0f};

    float fov_ = 60.0f;
    float nearClip_ = 0.1f;
    float farClip_ = 1000.0f;
};

} // namespace kimoyooju

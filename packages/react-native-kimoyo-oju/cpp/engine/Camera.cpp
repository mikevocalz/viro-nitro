#include "Camera.hpp"
#include <glm/gtc/quaternion.hpp>

namespace kimoyooju {

Camera::Camera() = default;

void Camera::setPosition(const glm::vec3& position) {
    position_ = position;
}

void Camera::setRotation(const glm::quat& rotation) {
    rotation_ = rotation;
}

void Camera::lookAt(const glm::vec3& target, const glm::vec3& up) {
    glm::vec3 direction = glm::normalize(target - position_);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    glm::vec3 newUp = glm::cross(right, direction);
    
    glm::mat3 rotationMatrix(right, newUp, -direction);
    rotation_ = glm::quat_cast(rotationMatrix);
}

glm::mat4 Camera::getViewMatrix() const {
    glm::mat4 view(1.0f);
    view = glm::mat4_cast(glm::conjugate(rotation_));
    view = glm::translate(view, -position_);
    return view;
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov_), aspectRatio, nearClip_, farClip_);
}

glm::vec3 Camera::getForward() const {
    return rotation_ * glm::vec3(0, 0, -1);
}

glm::vec3 Camera::getRight() const {
    return rotation_ * glm::vec3(1, 0, 0);
}

glm::vec3 Camera::getUp() const {
    return rotation_ * glm::vec3(0, 1, 0);
}

} // namespace kimoyooju

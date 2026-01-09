#include "SceneNode.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace kimoyooju {

SceneNode::SceneNode(uint32_t id, NodeType type)
    : id_(id), type_(type) {}

void SceneNode::setPosition(const glm::vec3& position) {
    position_ = position;
    transformDirty_ = true;
}

void SceneNode::setRotation(const glm::quat& rotation) {
    rotation_ = rotation;
    transformDirty_ = true;
}

void SceneNode::setScale(const glm::vec3& scale) {
    scale_ = scale;
    transformDirty_ = true;
}

glm::mat4 SceneNode::getLocalTransform() const {
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position_);
    transform = transform * glm::mat4_cast(rotation_);
    transform = glm::scale(transform, scale_);
    return transform;
}

glm::mat4 SceneNode::getWorldTransform() const {
    glm::mat4 local = getLocalTransform();
    auto parent = parent_.lock();
    if (parent) {
        return parent->getWorldTransform() * local;
    }
    return local;
}

void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
    if (!child) return;
    child->parent_ = shared_from_this();
    children_.push_back(child);
}

void SceneNode::removeChild(std::shared_ptr<SceneNode> child) {
    if (!child) return;
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->parent_.reset();
        children_.erase(it);
    }
}

void SceneNode::setParent(std::shared_ptr<SceneNode> parent) {
    auto currentParent = parent_.lock();
    if (currentParent) {
        currentParent->removeChild(shared_from_this());
    }
    if (parent) {
        parent->addChild(shared_from_this());
    }
}

void SceneNode::updateTransform() {
    if (transformDirty_) {
        localTransform_ = getLocalTransform();
        transformDirty_ = false;
    }
    worldTransform_ = getWorldTransform();
    for (auto& child : children_) {
        child->updateTransform();
    }
}

} // namespace kimoyooju

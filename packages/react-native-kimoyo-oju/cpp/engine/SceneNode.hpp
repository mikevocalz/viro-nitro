#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace kimoyooju {

class Geometry;

enum class NodeType {
    Group,
    Mesh,
    Light,
    Camera,
    Text,
    Model
};

class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    SceneNode(uint32_t id, NodeType type = NodeType::Group);
    virtual ~SceneNode() = default;

    uint32_t getId() const { return id_; }
    NodeType getType() const { return type_; }

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::quat& rotation);
    void setScale(const glm::vec3& scale);

    glm::vec3 getPosition() const { return position_; }
    glm::quat getRotation() const { return rotation_; }
    glm::vec3 getScale() const { return scale_; }

    glm::mat4 getLocalTransform() const;
    glm::mat4 getWorldTransform() const;

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

    void setColor(const glm::vec4& color) { color_ = color; }
    glm::vec4 getColor() const { return color_; }

    void setGeometry(std::shared_ptr<Geometry> geometry) { geometry_ = geometry; }
    std::shared_ptr<Geometry> getGeometry() const { return geometry_; }

    void addChild(std::shared_ptr<SceneNode> child);
    void removeChild(std::shared_ptr<SceneNode> child);
    void setParent(std::shared_ptr<SceneNode> parent);
    std::shared_ptr<SceneNode> getParent() const { return parent_.lock(); }
    const std::vector<std::shared_ptr<SceneNode>>& getChildren() const { return children_; }

    void updateTransform();

protected:
    uint32_t id_;
    NodeType type_;

    glm::vec3 position_{0.0f, 0.0f, 0.0f};
    glm::quat rotation_{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale_{1.0f, 1.0f, 1.0f};

    glm::mat4 localTransform_{1.0f};
    glm::mat4 worldTransform_{1.0f};
    bool transformDirty_ = true;

    bool visible_ = true;
    glm::vec4 color_{1.0f, 1.0f, 1.0f, 1.0f};

    std::shared_ptr<Geometry> geometry_;
    std::weak_ptr<SceneNode> parent_;
    std::vector<std::shared_ptr<SceneNode>> children_;
};

} // namespace kimoyooju

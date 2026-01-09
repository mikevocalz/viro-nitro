#pragma once

#include "kimoyooju/Types.hpp"
#include "kimoyooju/MemoryCounters.hpp"

#include <memory>
#include <vector>
#include <string>
#include <optional>

namespace kimoyooju {

class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    explicit SceneNode(NodeType type);
    virtual ~SceneNode();
    
    SceneNode(const SceneNode&) = delete;
    SceneNode& operator=(const SceneNode&) = delete;
    
    NodeType getType() const { return type_; }
    Handle getHandle() const { return handle_; }
    void setHandle(Handle h) { handle_ = h; }
    
    const Transform& getLocalTransform() const { return localTransform_; }
    void setLocalTransform(const Transform& t) { localTransform_ = t; markDirty(); }
    
    void setPosition(const Vec3& pos) { localTransform_.position = pos; markDirty(); }
    void setRotation(const Quat& rot) { localTransform_.rotation = rot; markDirty(); }
    void setScale(const Vec3& scale) { localTransform_.scale = scale; markDirty(); }
    
    bool isVisible() const { return visible_; }
    void setVisible(bool v) { visible_ = v; }
    
    std::weak_ptr<SceneNode> getParent() const { return parent_; }
    const std::vector<std::shared_ptr<SceneNode>>& getChildren() const { return children_; }
    
    void addChild(std::shared_ptr<SceneNode> child);
    void removeChild(const std::shared_ptr<SceneNode>& child);
    void removeFromParent();
    
    void reparent(std::shared_ptr<SceneNode> newParent);
    
    Mat4 getWorldTransform() const;
    void updateWorldTransform();
    
    bool isDirty() const { return dirty_; }
    void markDirty();
    void clearDirty() { dirty_ = false; }
    
    template<typename Visitor>
    void traverse(Visitor&& visitor) {
        visitor(*this);
        for (auto& child : children_) {
            child->traverse(std::forward<Visitor>(visitor));
        }
    }

protected:
    NodeType type_;
    Handle handle_;
    
    Transform localTransform_;
    Mat4 worldTransform_;
    
    bool visible_ = true;
    bool dirty_ = true;
    
    std::weak_ptr<SceneNode> parent_;
    std::vector<std::shared_ptr<SceneNode>> children_;
    
    ScopedNodeCounter nodeCounter_;
};

class MeshNode : public SceneNode {
public:
    MeshNode();
    
    void setGeometry(const std::string& type, const Vec3& dimensions);
    void setMaterial(const Color& diffuse, const Color& specular, float shininess);
    
    const std::string& getGeometryType() const { return geometryType_; }
    const Vec3& getDimensions() const { return dimensions_; }
    const Color& getDiffuseColor() const { return diffuseColor_; }

private:
    std::string geometryType_;
    Vec3 dimensions_ = {1.0f, 1.0f, 1.0f};
    Color diffuseColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    Color specularColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
    float shininess_ = 32.0f;
    Handle diffuseTexture_;
    Handle normalTexture_;
};

class LightNode : public SceneNode {
public:
    LightNode();
    
    void setLightType(LightType type) { lightType_ = type; }
    void setColor(const Color& c) { color_ = c; }
    void setIntensity(float i) { intensity_ = i; }
    void setRange(float r) { range_ = r; }
    void setConeAngles(float inner, float outer) { innerCone_ = inner; outerCone_ = outer; }
    
    LightType getLightType() const { return lightType_; }
    const Color& getColor() const { return color_; }
    float getIntensity() const { return intensity_; }

private:
    LightType lightType_ = LightType::POINT;
    Color color_ = {1.0f, 1.0f, 1.0f, 1.0f};
    float intensity_ = 1.0f;
    float range_ = 10.0f;
    float innerCone_ = 0.0f;
    float outerCone_ = 45.0f;
};

class CameraNode : public SceneNode {
public:
    CameraNode();
    
    void setFOV(float fov) { fov_ = fov; }
    void setClipPlanes(float near, float far) { nearClip_ = near; farClip_ = far; }
    
    float getFOV() const { return fov_; }
    float getNearClip() const { return nearClip_; }
    float getFarClip() const { return farClip_; }
    
    Mat4 getViewMatrix() const;
    Mat4 getProjectionMatrix(float aspectRatio) const;

private:
    float fov_ = 60.0f;
    float nearClip_ = 0.1f;
    float farClip_ = 1000.0f;
};

class TextNode : public SceneNode {
public:
    TextNode();
    
    void setText(const std::string& text) { text_ = text; }
    void setFontSize(float size) { fontSize_ = size; }
    void setColor(const Color& c) { color_ = c; }
    void setFontFamily(const std::string& family) { fontFamily_ = family; }
    
    const std::string& getText() const { return text_; }
    float getFontSize() const { return fontSize_; }

private:
    std::string text_;
    float fontSize_ = 16.0f;
    Color color_ = {1.0f, 1.0f, 1.0f, 1.0f};
    std::string fontFamily_ = "system";
};

class SceneGraph {
public:
    SceneGraph();
    ~SceneGraph();
    
    SceneGraph(const SceneGraph&) = delete;
    SceneGraph& operator=(const SceneGraph&) = delete;
    
    std::shared_ptr<SceneNode> getRoot() const { return root_; }
    
    std::shared_ptr<SceneNode> createNode(NodeType type);
    
    void updateTransforms();
    
    void clear();
    
    size_t getNodeCount() const;
    
    template<typename Visitor>
    void traverse(Visitor&& visitor) {
        if (root_) {
            root_->traverse(std::forward<Visitor>(visitor));
        }
    }

private:
    std::shared_ptr<SceneNode> root_;
};

} // namespace kimoyooju

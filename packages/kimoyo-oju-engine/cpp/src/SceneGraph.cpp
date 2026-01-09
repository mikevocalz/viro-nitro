#include "kimoyooju/SceneGraph.hpp"
#include <algorithm>
#include <cmath>

namespace kimoyooju {

namespace {
    Mat4 identity() {
        return {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    }
    
    Mat4 multiply(const Mat4& a, const Mat4& b) {
        Mat4 result = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i * 4 + j] = 
                    a[i * 4 + 0] * b[0 * 4 + j] +
                    a[i * 4 + 1] * b[1 * 4 + j] +
                    a[i * 4 + 2] * b[2 * 4 + j] +
                    a[i * 4 + 3] * b[3 * 4 + j];
            }
        }
        return result;
    }
    
    Mat4 fromTransform(const Transform& t) {
        // Build TRS matrix
        Mat4 result = identity();
        
        // Quaternion to rotation matrix
        float x = t.rotation[0], y = t.rotation[1], z = t.rotation[2], w = t.rotation[3];
        float x2 = x + x, y2 = y + y, z2 = z + z;
        float xx = x * x2, xy = x * y2, xz = x * z2;
        float yy = y * y2, yz = y * z2, zz = z * z2;
        float wx = w * x2, wy = w * y2, wz = w * z2;
        
        result[0] = (1 - (yy + zz)) * t.scale[0];
        result[1] = (xy + wz) * t.scale[0];
        result[2] = (xz - wy) * t.scale[0];
        
        result[4] = (xy - wz) * t.scale[1];
        result[5] = (1 - (xx + zz)) * t.scale[1];
        result[6] = (yz + wx) * t.scale[1];
        
        result[8] = (xz + wy) * t.scale[2];
        result[9] = (yz - wx) * t.scale[2];
        result[10] = (1 - (xx + yy)) * t.scale[2];
        
        result[12] = t.position[0];
        result[13] = t.position[1];
        result[14] = t.position[2];
        result[15] = 1;
        
        return result;
    }
}

SceneNode::SceneNode(NodeType type) : type_(type) {}

SceneNode::~SceneNode() {
    // Children will be released when shared_ptr destructs
    children_.clear();
}

void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
    if (!child) return;
    
    // Remove from current parent if any
    if (auto oldParent = child->parent_.lock()) {
        oldParent->removeChild(child);
    }
    
    child->parent_ = weak_from_this();
    children_.push_back(std::move(child));
    markDirty();
}

void SceneNode::removeChild(const std::shared_ptr<SceneNode>& child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->parent_.reset();
        children_.erase(it);
    }
}

void SceneNode::removeFromParent() {
    if (auto parent = parent_.lock()) {
        parent->removeChild(shared_from_this());
    }
}

void SceneNode::reparent(std::shared_ptr<SceneNode> newParent) {
    removeFromParent();
    if (newParent) {
        newParent->addChild(shared_from_this());
    }
}

Mat4 SceneNode::getWorldTransform() const {
    return worldTransform_;
}

void SceneNode::updateWorldTransform() {
    Mat4 localMatrix = fromTransform(localTransform_);
    
    if (auto parent = parent_.lock()) {
        worldTransform_ = multiply(parent->worldTransform_, localMatrix);
    } else {
        worldTransform_ = localMatrix;
    }
    
    dirty_ = false;
    
    for (auto& child : children_) {
        child->updateWorldTransform();
    }
}

void SceneNode::markDirty() {
    if (dirty_) return;
    dirty_ = true;
    for (auto& child : children_) {
        child->markDirty();
    }
}

MeshNode::MeshNode() : SceneNode(NodeType::MESH) {}

void MeshNode::setGeometry(const std::string& type, const Vec3& dimensions) {
    geometryType_ = type;
    dimensions_ = dimensions;
}

void MeshNode::setMaterial(const Color& diffuse, const Color& specular, float shininess) {
    diffuseColor_ = diffuse;
    specularColor_ = specular;
    shininess_ = shininess;
}

LightNode::LightNode() : SceneNode(NodeType::LIGHT) {}

CameraNode::CameraNode() : SceneNode(NodeType::CAMERA) {}

Mat4 CameraNode::getViewMatrix() const {
    // Inverse of world transform
    // Simplified - real implementation would properly invert
    Mat4 result = identity();
    result[12] = -localTransform_.position[0];
    result[13] = -localTransform_.position[1];
    result[14] = -localTransform_.position[2];
    return result;
}

Mat4 CameraNode::getProjectionMatrix(float aspectRatio) const {
    Mat4 result = {};
    
    float fovRad = fov_ * 3.14159265f / 180.0f;
    float tanHalfFov = std::tan(fovRad / 2.0f);
    
    result[0] = 1.0f / (aspectRatio * tanHalfFov);
    result[5] = 1.0f / tanHalfFov;
    result[10] = -(farClip_ + nearClip_) / (farClip_ - nearClip_);
    result[11] = -1.0f;
    result[14] = -(2.0f * farClip_ * nearClip_) / (farClip_ - nearClip_);
    
    return result;
}

TextNode::TextNode() : SceneNode(NodeType::TEXT) {}

SceneGraph::SceneGraph() {
    root_ = std::make_shared<SceneNode>(NodeType::GROUP);
}

SceneGraph::~SceneGraph() {
    clear();
}

std::shared_ptr<SceneNode> SceneGraph::createNode(NodeType type) {
    switch (type) {
        case NodeType::MESH:
            return std::make_shared<MeshNode>();
        case NodeType::LIGHT:
            return std::make_shared<LightNode>();
        case NodeType::CAMERA:
            return std::make_shared<CameraNode>();
        case NodeType::TEXT:
            return std::make_shared<TextNode>();
        case NodeType::GROUP:
        case NodeType::MODEL:
        default:
            return std::make_shared<SceneNode>(type);
    }
}

void SceneGraph::updateTransforms() {
    if (root_ && root_->isDirty()) {
        root_->updateWorldTransform();
    }
}

void SceneGraph::clear() {
    if (root_) {
        // Recursively clear children
        for (auto& child : root_->getChildren()) {
            child->removeFromParent();
        }
    }
}

size_t SceneGraph::getNodeCount() const {
    size_t count = 0;
    if (root_) {
        root_->traverse([&count](SceneNode&) { ++count; });
    }
    return count;
}

} // namespace kimoyooju

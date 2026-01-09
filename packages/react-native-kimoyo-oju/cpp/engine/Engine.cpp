#include "Engine.hpp"
#include "Renderer.hpp"
#include "SceneNode.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Geometry.hpp"
#include <android/log.h>
#include <nlohmann/json.hpp>

#define LOG_TAG "KimoyoOjuEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using json = nlohmann::json;

namespace kimoyooju {

Engine& Engine::getInstance() {
    static Engine instance;
    return instance;
}

Engine::Engine() {
    renderer_ = std::make_unique<Renderer>();
    camera_ = std::make_shared<Camera>();
    camera_->setPosition(glm::vec3(0, 0, 0));
}

Engine::~Engine() {
    shutdown();
}

bool Engine::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ != EngineState::Created && state_ != EngineState::Destroyed) {
        return true;
    }
    
    LOGI("Engine initializing...");
    state_ = EngineState::Created;
    return true;
}

void Engine::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    LOGI("Engine shutting down...");
    
    nodeMap_.clear();
    lightMap_.clear();
    rootNodes_.clear();
    lights_.clear();
    
    if (renderer_) {
        renderer_->shutdown();
    }
    
    state_ = EngineState::Destroyed;
}

void Engine::onSurfaceCreated(int width, int height) {
    std::lock_guard<std::mutex> lock(mutex_);
    LOGI("Surface created: %dx%d", width, height);
    
    surfaceWidth_ = width;
    surfaceHeight_ = height;
    
    if (renderer_) {
        renderer_->initialize();
        renderer_->setViewport(width, height);
    }
    
    state_ = EngineState::Running;
}

void Engine::onSurfaceChanged(int width, int height) {
    std::lock_guard<std::mutex> lock(mutex_);
    LOGI("Surface changed: %dx%d", width, height);
    
    surfaceWidth_ = width;
    surfaceHeight_ = height;
    
    if (renderer_) {
        renderer_->setViewport(width, height);
    }
}

void Engine::onSurfaceDestroyed() {
    std::lock_guard<std::mutex> lock(mutex_);
    LOGI("Surface destroyed");
    state_ = EngineState::SurfaceLost;
}

void Engine::drawFrame() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (state_ != EngineState::Running || !renderer_) return;

    // Use transparent background for AR mode so camera shows through
    if (xrMode_ == XRModeType::ImmersiveMR) {
        renderer_->clear(0.0f, 0.0f, 0.0f, 0.0f);
    } else {
        renderer_->clear(0.1f, 0.1f, 0.15f, 1.0f);
    }
    renderer_->beginFrame();
    
    if (xrMode_ == XRModeType::ImmersiveVR || xrMode_ == XRModeType::ImmersiveMR) {
        // Stereoscopic VR rendering
        const float ipd = 0.064f; // Interpupillary distance in meters (64mm average)
        const float halfIPD = ipd / 2.0f;
        
        glm::vec3 cameraPos = camera_->getPosition();
        glm::vec3 cameraRight = camera_->getRight();
        
        // Render left eye (left half of screen)
        glm::vec3 leftEyePos = cameraPos - cameraRight * halfIPD;
        camera_->setPosition(leftEyePos);
        renderer_->setViewport(0, 0, surfaceWidth_ / 2, surfaceHeight_);
        renderer_->render(rootNodes_, camera_, lights_);
        
        // Render right eye (right half of screen)
        glm::vec3 rightEyePos = cameraPos + cameraRight * halfIPD;
        camera_->setPosition(rightEyePos);
        renderer_->setViewport(surfaceWidth_ / 2, 0, surfaceWidth_ / 2, surfaceHeight_);
        renderer_->render(rootNodes_, camera_, lights_);
        
        // Restore camera position and full viewport
        camera_->setPosition(cameraPos);
        renderer_->setViewport(0, 0, surfaceWidth_, surfaceHeight_);
    } else {
        // Flat mode - single view
        renderer_->render(rootNodes_, camera_, lights_);
    }
    
    renderer_->endFrame();
}

void Engine::pause() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ == EngineState::Running) {
        state_ = EngineState::Paused;
    }
}

void Engine::resume() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ == EngineState::Paused) {
        state_ = EngineState::Running;
    }
}

uint32_t Engine::allocateHandle() {
    return nextHandleId_.fetch_add(1);
}

void Engine::destroyHandle(uint32_t handleId) {
    std::lock_guard<std::mutex> lock(mutex_);
    processDestroyNode(handleId);
}

MemoryStats Engine::getMemoryStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    MemoryStats stats;
    stats.liveNodes = static_cast<uint32_t>(nodeMap_.size());
    stats.liveTextures = 0;
    stats.liveBuffers = 0;
    stats.liveSwapchains = 0;
    stats.liveShaders = 1;
    stats.gpuMemoryBytes = stats.liveNodes * 1024;
    stats.cpuMemoryBytes = stats.liveNodes * 512;
    return stats;
}

bool Engine::processCommandBuffer(const std::string& jsonCommands) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        json buffer = json::parse(jsonCommands);
        
        if (!buffer.contains("commands") || !buffer["commands"].is_array()) {
            LOGE("Invalid command buffer: missing commands array");
            return false;
        }

        for (const auto& cmd : buffer["commands"]) {
            std::string type = cmd.value("type", "");
            
            if (type == "CREATE_NODE") {
                uint32_t handleId = static_cast<uint32_t>(cmd["handle"]["id"].get<double>());
                std::string nodeType = cmd.value("nodeType", "group");
                int parentId = -1;
                if (cmd.contains("parentHandle") && !cmd["parentHandle"].is_null()) {
                    parentId = static_cast<int>(cmd["parentHandle"]["id"].get<double>());
                }
                processCreateNode(handleId, nodeType, parentId);
            }
            else if (type == "DESTROY_NODE") {
                uint32_t handleId = static_cast<uint32_t>(cmd["handle"]["id"].get<double>());
                processDestroyNode(handleId);
            }
            else if (type == "SET_TRANSFORM") {
                uint32_t handleId = static_cast<uint32_t>(cmd["handle"]["id"].get<double>());
                auto pos = cmd["position"];
                auto rot = cmd["rotation"];
                auto scl = cmd["scale"];
                processSetTransform(handleId,
                    pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>(),
                    rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>(), rot[3].get<float>(),
                    scl[0].get<float>(), scl[1].get<float>(), scl[2].get<float>());
            }
            else if (type == "SET_VISIBILITY") {
                uint32_t handleId = static_cast<uint32_t>(cmd["handle"]["id"].get<double>());
                bool visible = cmd.value("visible", true);
                processSetVisibility(handleId, visible);
            }
            else if (type == "SET_MATERIAL") {
                uint32_t handleId = static_cast<uint32_t>(cmd["handle"]["id"].get<double>());
                auto color = cmd["diffuseColor"];
                processSetMaterial(handleId,
                    color[0].get<float>(), color[1].get<float>(),
                    color[2].get<float>(), color[3].get<float>());
            }
            else if (type == "SET_GEOMETRY") {
                uint32_t handleId = static_cast<uint32_t>(cmd["handle"]["id"].get<double>());
                std::string geomType = cmd.value("geometryType", "box");
                auto dims = cmd["dimensions"];
                processSetGeometry(handleId, geomType,
                    dims[0].get<float>(), dims[1].get<float>(), dims[2].get<float>());
            }
            else if (type == "SET_LIGHT") {
                uint32_t handleId = static_cast<uint32_t>(cmd["handle"]["id"].get<double>());
                std::string lightType = cmd.value("lightType", "directional");
                auto color = cmd["color"];
                float intensity = cmd.value("intensity", 1.0f);
                processSetLight(handleId, lightType,
                    color[0].get<float>(), color[1].get<float>(),
                    color[2].get<float>(), intensity);
            }
            else if (type == "SET_CAMERA") {
                uint32_t handleId = static_cast<uint32_t>(cmd["handle"]["id"].get<double>());
                float fov = cmd.value("fov", 60.0f);
                float nearClip = cmd.value("nearClip", 0.1f);
                float farClip = cmd.value("farClip", 1000.0f);
                processSetCamera(handleId, fov, nearClip, farClip);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        LOGE("Error processing command buffer: %s", e.what());
        return false;
    }
}

void Engine::processCreateNode(uint32_t handleId, const std::string& nodeType, int parentId) {
    NodeType type = NodeType::Group;
    if (nodeType == "mesh") type = NodeType::Mesh;
    else if (nodeType == "light") type = NodeType::Light;
    else if (nodeType == "camera") type = NodeType::Camera;
    else if (nodeType == "text") type = NodeType::Text;
    else if (nodeType == "model") type = NodeType::Model;

    auto node = std::make_shared<SceneNode>(handleId, type);
    nodeMap_[handleId] = node;

    if (parentId > 0 && nodeMap_.count(parentId)) {
        nodeMap_[parentId]->addChild(node);
    } else {
        rootNodes_.push_back(node);
    }
    
    LOGI("Created node %u of type %s", handleId, nodeType.c_str());
}

void Engine::processDestroyNode(uint32_t handleId) {
    auto it = nodeMap_.find(handleId);
    if (it == nodeMap_.end()) return;

    auto node = it->second;
    auto parent = node->getParent();
    if (parent) {
        parent->removeChild(node);
    } else {
        rootNodes_.erase(
            std::remove(rootNodes_.begin(), rootNodes_.end(), node),
            rootNodes_.end());
    }
    
    nodeMap_.erase(it);
    LOGI("Destroyed node %u", handleId);
}

void Engine::processSetTransform(uint32_t handleId, float px, float py, float pz,
                                  float rx, float ry, float rz, float rw,
                                  float sx, float sy, float sz) {
    auto it = nodeMap_.find(handleId);
    if (it == nodeMap_.end()) return;

    it->second->setPosition(glm::vec3(px, py, pz));
    it->second->setRotation(glm::quat(rw, rx, ry, rz));
    it->second->setScale(glm::vec3(sx, sy, sz));
}

void Engine::processSetVisibility(uint32_t handleId, bool visible) {
    auto it = nodeMap_.find(handleId);
    if (it == nodeMap_.end()) return;
    it->second->setVisible(visible);
}

void Engine::processSetMaterial(uint32_t handleId, float r, float g, float b, float a) {
    auto it = nodeMap_.find(handleId);
    if (it == nodeMap_.end()) return;
    it->second->setColor(glm::vec4(r, g, b, a));
    LOGI("Set material on node %u: color(%.2f, %.2f, %.2f, %.2f)", handleId, r, g, b, a);
}

void Engine::processSetGeometry(uint32_t handleId, const std::string& geometryType,
                                 float dimX, float dimY, float dimZ) {
    auto it = nodeMap_.find(handleId);
    if (it == nodeMap_.end()) return;

    std::shared_ptr<Geometry> geometry;
    if (geometryType == "box") {
        geometry = Geometry::createBox(dimX, dimY, dimZ);
    } else if (geometryType == "sphere") {
        geometry = Geometry::createSphere(dimX);
    } else if (geometryType == "plane") {
        geometry = Geometry::createPlane(dimX, dimY);
    }
    
    if (geometry) {
        it->second->setGeometry(geometry);
        LOGI("Set geometry %s on node %u", geometryType.c_str(), handleId);
    }
}

void Engine::processSetLight(uint32_t handleId, const std::string& lightType,
                              float r, float g, float b, float intensity) {
    LightType type = LightType::Directional;
    if (lightType == "ambient") type = LightType::Ambient;
    else if (lightType == "point") type = LightType::Point;
    else if (lightType == "spot") type = LightType::Spot;

    auto light = std::make_shared<Light>(type);
    light->setColor(glm::vec3(r, g, b));
    light->setIntensity(intensity);
    
    lightMap_[handleId] = light;
    lights_.push_back(light);
    
    LOGI("Created light %u of type %s", handleId, lightType.c_str());
}

void Engine::processSetCamera(uint32_t handleId, float fov, float nearClip, float farClip) {
    camera_->setFov(fov);
    camera_->setNearClip(nearClip);
    camera_->setFarClip(farClip);
    LOGI("Updated camera: fov=%.1f, near=%.2f, far=%.1f", fov, nearClip, farClip);
}

// Touch/interaction handling
Engine::HitResult Engine::handleTouch(float screenX, float screenY, int action) {
    std::lock_guard<std::mutex> lock(mutex_);
    HitResult result;
    
    // Action: 0=DOWN, 1=UP, 2=MOVE
    if (action == 0) { // Touch down
        glm::vec3 rayDir = screenToWorldRay(screenX, screenY);
        glm::vec3 rayOrigin = camera_->getPosition();
        
        float closestDist = std::numeric_limits<float>::max();
        uint32_t closestNode = 0;
        
        // Test all nodes for intersection
        for (const auto& pair : nodeMap_) {
            auto& node = pair.second;
            if (!node->isVisible() || !node->getGeometry()) continue;
            
            float dist;
            if (rayIntersectsNode(rayOrigin, rayDir, node, dist)) {
                if (dist < closestDist) {
                    closestDist = dist;
                    closestNode = pair.first;
                }
            }
        }
        
        if (closestNode > 0) {
            selectedNodeId_ = closestNode;
            result.hit = true;
            result.nodeId = closestNode;
            result.distance = closestDist;
            
            glm::vec3 hitPoint = rayOrigin + rayDir * closestDist;
            result.worldX = hitPoint.x;
            result.worldY = hitPoint.y;
            result.worldZ = hitPoint.z;
            
            LOGI("Touch hit node %u at distance %.2f", closestNode, closestDist);
        }
        
        lastTouchX_ = screenX;
        lastTouchY_ = screenY;
    }
    else if (action == 2 && selectedNodeId_ > 0) { // Touch move
        float deltaX = screenX - lastTouchX_;
        float deltaY = screenY - lastTouchY_;
        moveSelectedNode(deltaX, deltaY);
        lastTouchX_ = screenX;
        lastTouchY_ = screenY;
        result.hit = true;
        result.nodeId = selectedNodeId_;
    }
    else if (action == 1) { // Touch up
        LOGI("Touch released, was on node %u", selectedNodeId_);
        result.nodeId = selectedNodeId_;
        result.hit = selectedNodeId_ > 0;
        selectedNodeId_ = 0;
    }
    
    return result;
}

void Engine::setSelectedNode(uint32_t nodeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    selectedNodeId_ = nodeId;
}

void Engine::moveSelectedNode(float deltaX, float deltaY) {
    if (selectedNodeId_ == 0) return;
    
    auto it = nodeMap_.find(selectedNodeId_);
    if (it == nodeMap_.end()) return;
    
    auto& node = it->second;
    glm::vec3 pos = node->getPosition();
    
    // Convert screen delta to world movement
    // Scale factor based on distance from camera
    float depth = glm::abs(pos.z - camera_->getPosition().z);
    float scale = depth * 0.002f; // Adjust sensitivity
    
    pos.x += deltaX * scale;
    pos.y -= deltaY * scale; // Invert Y for screen coordinates
    
    node->setPosition(pos);
}

glm::vec3 Engine::screenToWorldRay(float screenX, float screenY) {
    // Normalize screen coordinates to [-1, 1]
    float ndcX = (2.0f * screenX / surfaceWidth_) - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY / surfaceHeight_); // Flip Y
    
    // Get inverse projection and view matrices
    float aspect = static_cast<float>(surfaceWidth_) / static_cast<float>(surfaceHeight_);
    glm::mat4 projection = camera_->getProjectionMatrix(aspect);
    glm::mat4 view = camera_->getViewMatrix();
    
    glm::mat4 invVP = glm::inverse(projection * view);
    
    // Create ray in clip space and transform to world
    glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 rayWorld = invVP * rayClip;
    rayWorld /= rayWorld.w;
    
    glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld) - camera_->getPosition());
    return rayDir;
}

bool Engine::rayIntersectsNode(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                const std::shared_ptr<SceneNode>& node, float& distance) {
    // Simple AABB (axis-aligned bounding box) intersection test
    glm::vec3 nodePos = node->getPosition();
    glm::vec3 nodeScale = node->getScale();
    
    // Approximate bounding box (works for boxes, rough for spheres)
    glm::vec3 halfExtents = nodeScale * 0.5f;
    glm::vec3 minBound = nodePos - halfExtents;
    glm::vec3 maxBound = nodePos + halfExtents;
    
    // Ray-AABB intersection (slab method)
    glm::vec3 invDir = 1.0f / rayDir;
    glm::vec3 t1 = (minBound - rayOrigin) * invDir;
    glm::vec3 t2 = (maxBound - rayOrigin) * invDir;
    
    glm::vec3 tMin = glm::min(t1, t2);
    glm::vec3 tMax = glm::max(t1, t2);
    
    float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);
    
    if (tNear > tFar || tFar < 0) return false;
    
    distance = tNear > 0 ? tNear : tFar;
    return true;
}

} // namespace kimoyooju

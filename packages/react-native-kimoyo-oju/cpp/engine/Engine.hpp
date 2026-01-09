#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <glm/glm.hpp>

namespace kimoyooju {

class Renderer;
class SceneNode;
class Camera;
class Light;
class Geometry;

enum class EngineState {
    Created,
    Running,
    Paused,
    SurfaceLost,
    Destroyed
};

enum class XRModeType {
    Flat,
    ImmersiveVR,
    ImmersiveMR
};

struct MemoryStats {
    uint32_t liveNodes = 0;
    uint32_t liveTextures = 0;
    uint32_t liveBuffers = 0;
    uint32_t liveSwapchains = 0;
    uint32_t liveShaders = 0;
    uint64_t gpuMemoryBytes = 0;
    uint64_t cpuMemoryBytes = 0;
};

class Engine {
public:
    static Engine& getInstance();

    bool initialize();
    void shutdown();

    void onSurfaceCreated(int width, int height);
    void onSurfaceChanged(int width, int height);
    void onSurfaceDestroyed();
    void drawFrame();

    void pause();
    void resume();

    bool processCommandBuffer(const std::string& jsonCommands);

    uint32_t allocateHandle();
    void destroyHandle(uint32_t handleId);

    EngineState getState() const { return state_; }
    XRModeType getXRMode() const { return xrMode_; }
    void setXRMode(XRModeType mode) { xrMode_ = mode; }

    MemoryStats getMemoryStats() const;
    
    // Touch/interaction handling
    struct HitResult {
        uint32_t nodeId = 0;
        float distance = 0;
        float worldX = 0, worldY = 0, worldZ = 0;
        bool hit = false;
    };
    
    HitResult handleTouch(float screenX, float screenY, int action);
    void setSelectedNode(uint32_t nodeId);
    uint32_t getSelectedNode() const { return selectedNodeId_; }
    void moveSelectedNode(float deltaX, float deltaY);

private:
    Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    void processCreateNode(uint32_t handleId, const std::string& nodeType, int parentId);
    void processDestroyNode(uint32_t handleId);
    void processSetTransform(uint32_t handleId, float px, float py, float pz,
                             float rx, float ry, float rz, float rw,
                             float sx, float sy, float sz);
    void processSetVisibility(uint32_t handleId, bool visible);
    void processSetMaterial(uint32_t handleId, float r, float g, float b, float a);
    void processSetGeometry(uint32_t handleId, const std::string& geometryType,
                           float dimX, float dimY, float dimZ);
    void processSetLight(uint32_t handleId, const std::string& lightType,
                        float r, float g, float b, float intensity);
    void processSetCamera(uint32_t handleId, float fov, float nearClip, float farClip);

    std::unique_ptr<Renderer> renderer_;
    std::shared_ptr<Camera> camera_;
    std::vector<std::shared_ptr<Light>> lights_;
    std::vector<std::shared_ptr<SceneNode>> rootNodes_;
    std::unordered_map<uint32_t, std::shared_ptr<SceneNode>> nodeMap_;
    std::unordered_map<uint32_t, std::shared_ptr<Light>> lightMap_;

    std::atomic<uint32_t> nextHandleId_{1};
    std::atomic<EngineState> state_{EngineState::Created};
    XRModeType xrMode_ = XRModeType::Flat;

    int surfaceWidth_ = 0;
    int surfaceHeight_ = 0;
    
    // Interaction state
    uint32_t selectedNodeId_ = 0;
    float lastTouchX_ = 0;
    float lastTouchY_ = 0;

    mutable std::mutex mutex_;
    
    // Raycasting helpers
    glm::vec3 screenToWorldRay(float screenX, float screenY);
    bool rayIntersectsNode(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                           const std::shared_ptr<SceneNode>& node, float& distance);
};

} // namespace kimoyooju

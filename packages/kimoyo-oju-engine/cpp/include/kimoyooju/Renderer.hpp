#pragma once

#include "kimoyooju/Types.hpp"
#include "kimoyooju/SceneGraph.hpp"
#include "kimoyooju/GraphicsDevice.hpp"

#include <memory>
#include <mutex>
#include <atomic>

namespace kimoyooju {

class XRSession;

class Renderer {
public:
    struct Config {
        XRMode mode = XRMode::FLAT;
        uint32_t width = 0;
        uint32_t height = 0;
        bool enableMSAA = true;
        uint32_t msaaSamples = 4;
    };
    
    static std::unique_ptr<Renderer> create(const Config& config);
    ~Renderer();
    
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    
    Result<void> attachSurface(void* nativeSurface, uint32_t width, uint32_t height);
    Result<void> detachSurface();
    
    Result<void> pause();
    Result<void> resume();
    
    Result<void> enterXR(XRMode mode);
    Result<void> exitXR();
    
    void renderFrame();
    
    RendererState getState() const;
    XRMode getMode() const;
    
    SceneGraph& getSceneGraph();
    const SceneGraph& getSceneGraph() const;

private:
    explicit Renderer(const Config& config);
    
    bool transitionTo(RendererState newState);
    bool isValidTransition(RendererState from, RendererState to) const;
    
    void renderFlatFrame();
    void renderXRFrame();
    
    void handleSurfaceLost();
    
private:
    Config config_;
    
    mutable std::mutex stateMutex_;
    std::atomic<RendererState> state_{RendererState::CREATED};
    XRMode mode_{XRMode::FLAT};
    
    std::unique_ptr<GraphicsDevice> graphicsDevice_;
    std::unique_ptr<XRSession> xrSession_;
    std::unique_ptr<SceneGraph> sceneGraph_;
    
    void* currentSurface_ = nullptr;
    uint32_t surfaceWidth_ = 0;
    uint32_t surfaceHeight_ = 0;
    
    bool surfaceValid_ = false;
};

} // namespace kimoyooju

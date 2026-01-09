#include "kimoyooju/Renderer.hpp"
#include "kimoyooju/XRSession.hpp"

namespace kimoyooju {

std::unique_ptr<Renderer> Renderer::create(const Config& config) {
    return std::unique_ptr<Renderer>(new Renderer(config));
}

Renderer::Renderer(const Config& config) 
    : config_(config)
    , mode_(config.mode)
    , sceneGraph_(std::make_unique<SceneGraph>()) {
}

Renderer::~Renderer() {
    // Ensure proper teardown order
    if (state_ != RendererState::DESTROYED) {
        // Force transition to destroyed
        state_ = RendererState::DESTROYED;
        
        // 1. Stop XR session first
        xrSession_.reset();
        
        // 2. Clear scene graph
        sceneGraph_.reset();
        
        // 3. Destroy graphics device
        graphicsDevice_.reset();
    }
}

Result<void> Renderer::attachSurface(void* nativeSurface, uint32_t width, uint32_t height) {
    std::lock_guard lock(stateMutex_);
    
    RendererState currentState = state_.load();
    if (currentState == RendererState::DESTROYED) {
        return Result<void>::failure("INVALID_STATE", "Renderer is destroyed");
    }
    
    currentSurface_ = nativeSurface;
    surfaceWidth_ = width;
    surfaceHeight_ = height;
    
    // Create graphics device if needed
    if (!graphicsDevice_) {
        GraphicsDevice::Config gfxConfig;
        gfxConfig.enableMSAA = config_.enableMSAA;
        gfxConfig.msaaSamples = config_.msaaSamples;
        
        graphicsDevice_ = GraphicsDevice::create(gfxConfig);
        if (!graphicsDevice_) {
            return Result<void>::failure("GRAPHICS_ERROR", "Failed to create graphics device");
        }
    }
    
    auto result = graphicsDevice_->initialize(nativeSurface, width, height);
    if (!result.ok) {
        return result;
    }
    
    surfaceValid_ = true;
    
    if (!transitionTo(RendererState::RUNNING)) {
        return Result<void>::failure("INVALID_STATE", "Failed to transition to RUNNING");
    }
    
    return Result<void>::success();
}

Result<void> Renderer::detachSurface() {
    std::lock_guard lock(stateMutex_);
    
    surfaceValid_ = false;
    currentSurface_ = nullptr;
    
    if (graphicsDevice_) {
        graphicsDevice_->shutdown();
    }
    
    transitionTo(RendererState::SURFACE_LOST);
    
    return Result<void>::success();
}

Result<void> Renderer::pause() {
    std::lock_guard lock(stateMutex_);
    
    RendererState currentState = state_.load();
    if (currentState != RendererState::RUNNING) {
        return Result<void>::success(); // Already paused or invalid state
    }
    
    // Set state FIRST to prevent reentrancy
    if (!transitionTo(RendererState::PAUSED)) {
        return Result<void>::failure("INVALID_STATE", "Cannot pause from current state");
    }
    
    return Result<void>::success();
}

Result<void> Renderer::resume() {
    std::lock_guard lock(stateMutex_);
    
    RendererState currentState = state_.load();
    if (currentState != RendererState::PAUSED) {
        return Result<void>::success(); // Not paused
    }
    
    if (!surfaceValid_) {
        return Result<void>::failure("SURFACE_INVALID", "Cannot resume without valid surface");
    }
    
    if (!transitionTo(RendererState::RUNNING)) {
        return Result<void>::failure("INVALID_STATE", "Cannot resume from current state");
    }
    
    return Result<void>::success();
}

Result<void> Renderer::enterXR(XRMode mode) {
    std::lock_guard lock(stateMutex_);
    
    if (mode == XRMode::FLAT) {
        return Result<void>::failure("INVALID_MODE", "Use exitXR to return to flat mode");
    }
    
    if (!graphicsDevice_ || !graphicsDevice_->isValid()) {
        return Result<void>::failure("GRAPHICS_ERROR", "Graphics device not ready");
    }
    
    XRSession::Config xrConfig;
    xrConfig.mode = mode;
    
    xrSession_ = XRSession::create(xrConfig);
    if (!xrSession_) {
        return Result<void>::failure("XR_ERROR", "Failed to create XR session");
    }
    
    auto result = xrSession_->initialize(graphicsDevice_->getGraphicsBinding());
    if (!result.ok) {
        xrSession_.reset();
        return result;
    }
    
    result = xrSession_->begin();
    if (!result.ok) {
        xrSession_.reset();
        return result;
    }
    
    mode_ = mode;
    return Result<void>::success();
}

Result<void> Renderer::exitXR() {
    std::lock_guard lock(stateMutex_);
    
    if (!xrSession_) {
        return Result<void>::success(); // Already in flat mode
    }
    
    xrSession_->end();
    xrSession_.reset();
    
    mode_ = XRMode::FLAT;
    return Result<void>::success();
}

void Renderer::renderFrame() {
    RendererState currentState = state_.load();
    if (currentState != RendererState::RUNNING) {
        return;
    }
    
    // Check surface validity BEFORE any rendering
    if (!surfaceValid_ || !graphicsDevice_ || !graphicsDevice_->isValid()) {
        handleSurfaceLost();
        return;
    }
    
    if (mode_ == XRMode::FLAT) {
        renderFlatFrame();
    } else {
        renderXRFrame();
    }
}

void Renderer::renderFlatFrame() {
    if (!graphicsDevice_) return;
    
    graphicsDevice_->beginFrame();
    
    graphicsDevice_->setViewport(0, 0, surfaceWidth_, surfaceHeight_);
    graphicsDevice_->setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    graphicsDevice_->clear(true, true, false);
    
    // Update scene transforms
    if (sceneGraph_) {
        sceneGraph_->updateTransforms();
        
        // Traverse and render
        sceneGraph_->traverse([this](SceneNode& node) {
            if (!node.isVisible()) return;
            // Render node based on type
            // This would dispatch to mesh/light/etc rendering
        });
    }
    
    graphicsDevice_->endFrame();
    
    // Check surface again before swap
    if (!surfaceValid_) {
        handleSurfaceLost();
        return;
    }
    
    graphicsDevice_->present();
}

void Renderer::renderXRFrame() {
    if (!xrSession_ || !xrSession_->isSessionRunning()) {
        return;
    }
    
    auto waitResult = xrSession_->waitFrame();
    if (!waitResult.ok) {
        return;
    }
    
    auto beginResult = xrSession_->beginFrame();
    if (!beginResult.ok) {
        return;
    }
    
    if (xrSession_->shouldRender()) {
        auto views = xrSession_->getViews();
        
        for (size_t i = 0; i < views.size(); ++i) {
            auto acquireResult = xrSession_->acquireSwapchainImage(static_cast<uint32_t>(i));
            if (!acquireResult.ok) continue;
            
            auto waitImageResult = xrSession_->waitSwapchainImage(static_cast<uint32_t>(i));
            if (!waitImageResult.ok) {
                xrSession_->releaseSwapchainImage(static_cast<uint32_t>(i));
                continue;
            }
            
            // Render to this view
            graphicsDevice_->setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            graphicsDevice_->clear(true, true, false);
            
            if (sceneGraph_) {
                sceneGraph_->updateTransforms();
                // Render scene with view/projection from views[i]
            }
            
            xrSession_->releaseSwapchainImage(static_cast<uint32_t>(i));
        }
    }
    
    xrSession_->endFrame();
}

RendererState Renderer::getState() const {
    return state_.load();
}

XRMode Renderer::getMode() const {
    return mode_;
}

SceneGraph& Renderer::getSceneGraph() {
    return *sceneGraph_;
}

const SceneGraph& Renderer::getSceneGraph() const {
    return *sceneGraph_;
}

bool Renderer::transitionTo(RendererState newState) {
    RendererState currentState = state_.load();
    
    if (!isValidTransition(currentState, newState)) {
        return false;
    }
    
    state_ = newState;
    return true;
}

bool Renderer::isValidTransition(RendererState from, RendererState to) const {
    // DESTROYED is terminal - no transitions from it
    if (from == RendererState::DESTROYED) {
        return false;
    }
    
    // Any state can transition to DESTROYED
    if (to == RendererState::DESTROYED) {
        return true;
    }
    
    switch (from) {
        case RendererState::CREATED:
            return to == RendererState::RUNNING;
            
        case RendererState::RUNNING:
            return to == RendererState::PAUSED || to == RendererState::SURFACE_LOST;
            
        case RendererState::PAUSED:
            return to == RendererState::RUNNING || to == RendererState::SURFACE_LOST;
            
        case RendererState::SURFACE_LOST:
            return to == RendererState::RUNNING;
            
        default:
            return false;
    }
}

void Renderer::handleSurfaceLost() {
    std::lock_guard lock(stateMutex_);
    
    surfaceValid_ = false;
    
    // Exit XR if active
    if (xrSession_) {
        xrSession_->end();
        xrSession_.reset();
        mode_ = XRMode::FLAT;
    }
    
    transitionTo(RendererState::SURFACE_LOST);
}

} // namespace kimoyooju

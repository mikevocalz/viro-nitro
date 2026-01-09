#include "HybridKimoyoOjuEngine.hpp"
#include "engine/Engine.hpp"

namespace margelo::nitro::kimoyooju {

// ============================================================================
// HybridKimoyoOjuEngine Implementation
// ============================================================================

HybridKimoyoOjuEngine::HybridKimoyoOjuEngine() : HybridObject("KimoyoOjuEngine") {
    ::kimoyooju::Engine::getInstance().initialize();
}

HybridKimoyoOjuEngine::~HybridKimoyoOjuEngine() {
    // Engine is a singleton, don't shutdown here
}

VoidResult HybridKimoyoOjuEngine::submitJson(const std::string& bufferJson) {
    bool success = ::kimoyooju::Engine::getInstance().processCommandBuffer(bufferJson);
    if (success) {
        return VoidResult(true, "", "");
    }
    return VoidResult(false, "SUBMIT_FAILED", "Failed to process command buffer");
}

HandleResult HybridKimoyoOjuEngine::allocateHandle() {
    uint32_t id = ::kimoyooju::Engine::getInstance().allocateHandle();
    uint32_t gen = handleGen_.load();
    
    Handle handle(static_cast<double>(id), static_cast<double>(gen));
    return HandleResult(true, "", "", handle);
}

RendererState HybridKimoyoOjuEngine::getState() {
    auto state = ::kimoyooju::Engine::getInstance().getState();
    switch (state) {
        case ::kimoyooju::EngineState::Created: return RendererState::CREATED;
        case ::kimoyooju::EngineState::Running: return RendererState::RUNNING;
        case ::kimoyooju::EngineState::Paused: return RendererState::PAUSED;
        case ::kimoyooju::EngineState::SurfaceLost: return RendererState::SURFACE_LOST;
        case ::kimoyooju::EngineState::Destroyed: return RendererState::DESTROYED;
        default: return RendererState::CREATED;
    }
}

XRMode HybridKimoyoOjuEngine::getMode() {
    auto mode = ::kimoyooju::Engine::getInstance().getXRMode();
    switch (mode) {
        case ::kimoyooju::XRModeType::Flat: return XRMode::FLAT;
        case ::kimoyooju::XRModeType::ImmersiveVR: return XRMode::IMMERSIVE_VR;
        case ::kimoyooju::XRModeType::ImmersiveMR: return XRMode::IMMERSIVE_MR;
        default: return XRMode::FLAT;
    }
}

MemoryCounters HybridKimoyoOjuEngine::getMemoryCounters() {
    auto stats = ::kimoyooju::Engine::getInstance().getMemoryStats();
    return MemoryCounters(
        static_cast<double>(stats.liveNodes),
        static_cast<double>(stats.liveTextures),
        static_cast<double>(stats.liveBuffers),
        static_cast<double>(stats.liveSwapchains),
        static_cast<double>(stats.liveShaders),
        static_cast<double>(stats.gpuMemoryBytes),
        static_cast<double>(stats.cpuMemoryBytes)
    );
}

VoidResult HybridKimoyoOjuEngine::pause() {
    ::kimoyooju::Engine::getInstance().pause();
    return VoidResult(true, "", "");
}

VoidResult HybridKimoyoOjuEngine::resume() {
    ::kimoyooju::Engine::getInstance().resume();
    return VoidResult(true, "", "");
}

VoidResult HybridKimoyoOjuEngine::enterXR(XRMode mode) {
    ::kimoyooju::XRModeType nativeMode;
    switch (mode) {
        case XRMode::IMMERSIVE_VR: nativeMode = ::kimoyooju::XRModeType::ImmersiveVR; break;
        case XRMode::IMMERSIVE_MR: nativeMode = ::kimoyooju::XRModeType::ImmersiveMR; break;
        default: nativeMode = ::kimoyooju::XRModeType::Flat; break;
    }
    ::kimoyooju::Engine::getInstance().setXRMode(nativeMode);
    return VoidResult(true, "", "");
}

VoidResult HybridKimoyoOjuEngine::exitXR() {
    ::kimoyooju::Engine::getInstance().setXRMode(::kimoyooju::XRModeType::Flat);
    return VoidResult(true, "", "");
}

VoidResult HybridKimoyoOjuEngine::destroyHandle(const Handle& handle) {
    uint32_t handleId = static_cast<uint32_t>(handle.id);
    ::kimoyooju::Engine::getInstance().destroyHandle(handleId);
    return VoidResult(true, "", "");
}

void HybridKimoyoOjuEngine::onSurfaceCreated(void* surface, int width, int height) {
    ::kimoyooju::Engine::getInstance().onSurfaceCreated(width, height);
}

void HybridKimoyoOjuEngine::onSurfaceChanged(int width, int height) {
    ::kimoyooju::Engine::getInstance().onSurfaceChanged(width, height);
}

void HybridKimoyoOjuEngine::onSurfaceDestroyed() {
    ::kimoyooju::Engine::getInstance().onSurfaceDestroyed();
}

} // namespace margelo::nitro::kimoyooju

#pragma once

#include "HybridKimoyoOjuEngineSpec.hpp"
#include <memory>
#include <atomic>

// Forward declarations
namespace kimoyooju {
    class Engine;
}

namespace margelo::nitro::kimoyooju {

/**
 * HybridKimoyoOjuEngine - Concrete implementation of the generated spec
 */
class HybridKimoyoOjuEngine : public HybridKimoyoOjuEngineSpec {
public:
    HybridKimoyoOjuEngine();
    ~HybridKimoyoOjuEngine() override;

    // HybridKimoyoOjuEngineSpec interface implementation
    VoidResult submitJson(const std::string& bufferJson) override;
    HandleResult allocateHandle() override;
    RendererState getState() override;
    XRMode getMode() override;
    MemoryCounters getMemoryCounters() override;
    VoidResult pause() override;
    VoidResult resume() override;
    VoidResult enterXR(XRMode mode) override;
    VoidResult exitXR() override;
    VoidResult destroyHandle(const Handle& handle) override;

    // Surface management (called from native view)
    void onSurfaceCreated(void* surface, int width, int height);
    void onSurfaceChanged(int width, int height);
    void onSurfaceDestroyed();

private:
    std::shared_ptr<::kimoyooju::Engine> engine_;
    std::atomic<uint32_t> nextHandleId_{1};
    std::atomic<uint32_t> handleGen_{1};
    RendererState currentState_ = RendererState::CREATED;
    XRMode currentMode_ = XRMode::FLAT;
};

} // namespace margelo::nitro::kimoyooju

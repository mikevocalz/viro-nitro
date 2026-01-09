#pragma once

#include "kimoyooju/Types.hpp"
#include "kimoyooju/CommandBuffer.hpp"
#include "kimoyooju/Renderer.hpp"
#include "kimoyooju/HandleRegistry.hpp"
#include "kimoyooju/MemoryCounters.hpp"

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>

namespace kimoyooju {

class Engine final {
public:
    struct Config {
        bool enableValidation = true;
        bool enableMemoryTracking = true;
        uint32_t maxNodes = 100000;
        uint32_t maxTextures = 1000;
    };

    static std::unique_ptr<Engine> create(const Config& config);
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    Result<void> submit(CommandBuffer buffer);
    
    Result<Handle> createNode(NodeType type);
    Result<void> destroyNode(Handle handle);
    
    Result<void> attachSurface(void* nativeSurface, uint32_t width, uint32_t height);
    Result<void> detachSurface();
    
    Result<void> pause();
    Result<void> resume();
    
    Result<void> enterXR(XRMode mode);
    Result<void> exitXR();
    
    RendererState getState() const;
    MemoryCounters getMemoryCounters() const;
    
    void enqueueCleanup(Handle handle);
    
    using EventCallback = std::function<void(EngineEvent)>;
    void setEventCallback(EventCallback callback);

private:
    explicit Engine(const Config& config);
    
    void renderThreadMain();
    void processCommandQueue();
    void processCleanupQueue();
    
    bool isRenderThread() const;
    void assertRenderThread() const;
    
    Result<void> executeCommand(const Command& cmd);

private:
    Config config_;
    
    std::unique_ptr<Renderer> renderer_;
    
    std::thread renderThread_;
    std::thread::id renderThreadId_;
    std::atomic<bool> running_{false};
    std::atomic<bool> shouldExit_{false};
    
    std::mutex commandMutex_;
    std::condition_variable commandCv_;
    std::queue<CommandBuffer> commandQueue_;
    
    std::mutex cleanupMutex_;
    std::vector<Handle> pendingCleanups_;
    
    HandleRegistry<std::shared_ptr<SceneNode>> nodeRegistry_;
    HandleRegistry<std::shared_ptr<Texture>> textureRegistry_;
    
    EventCallback eventCallback_;
    std::mutex callbackMutex_;
    
    mutable std::mutex stateMutex_;
};

} // namespace kimoyooju

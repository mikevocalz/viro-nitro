#include "kimoyooju/Engine.hpp"
#include "kimoyooju/CommandBuffer.hpp"

#include <chrono>
#include <cassert>

namespace kimoyooju {

std::unique_ptr<Engine> Engine::create(const Config& config) {
    return std::unique_ptr<Engine>(new Engine(config));
}

Engine::Engine(const Config& config) : config_(config) {
    nodeRegistry_ = HandleRegistry<std::shared_ptr<SceneNode>>(config.maxNodes);
    textureRegistry_ = HandleRegistry<std::shared_ptr<Texture>>(config.maxTextures);
}

Engine::~Engine() {
    shouldExit_ = true;
    running_ = false;
    
    commandCv_.notify_all();
    
    if (renderThread_.joinable()) {
        renderThread_.join();
    }
    
    renderer_.reset();
    nodeRegistry_.clear();
    textureRegistry_.clear();
}

Result<void> Engine::submit(CommandBuffer buffer) {
    if (config_.enableValidation) {
        CommandBufferValidator validator;
        auto result = validator.validate(buffer);
        if (!result.valid) {
            return Result<void>::failure(result.errorCode, result.errorMessage);
        }
    }
    
    {
        std::lock_guard lock(commandMutex_);
        commandQueue_.push(std::move(buffer));
    }
    commandCv_.notify_one();
    
    return Result<void>::success();
}

Result<Handle> Engine::createNode(NodeType type) {
    auto node = std::make_shared<SceneNode>(type);
    Handle handle = nodeRegistry_.allocate(std::move(node));
    
    if (!handle.isValid()) {
        return Result<Handle>::failure("OUT_OF_HANDLES", "Node registry is full");
    }
    
    return Result<Handle>::success(handle);
}

Result<void> Engine::destroyNode(Handle handle) {
    enqueueCleanup(handle);
    return Result<void>::success();
}

Result<void> Engine::attachSurface(void* nativeSurface, uint32_t width, uint32_t height) {
    if (!renderer_) {
        Renderer::Config config;
        config.width = width;
        config.height = height;
        renderer_ = Renderer::create(config);
    }
    
    auto result = renderer_->attachSurface(nativeSurface, width, height);
    if (!result.ok) {
        return result;
    }
    
    if (!running_) {
        running_ = true;
        renderThread_ = std::thread(&Engine::renderThreadMain, this);
        renderThreadId_ = renderThread_.get_id();
    }
    
    return Result<void>::success();
}

Result<void> Engine::detachSurface() {
    if (renderer_) {
        return renderer_->detachSurface();
    }
    return Result<void>::success();
}

Result<void> Engine::pause() {
    if (renderer_) {
        return renderer_->pause();
    }
    return Result<void>::success();
}

Result<void> Engine::resume() {
    if (renderer_) {
        return renderer_->resume();
    }
    return Result<void>::success();
}

Result<void> Engine::enterXR(XRMode mode) {
    if (!renderer_) {
        return Result<void>::failure("INVALID_STATE", "Renderer not initialized");
    }
    return renderer_->enterXR(mode);
}

Result<void> Engine::exitXR() {
    if (!renderer_) {
        return Result<void>::failure("INVALID_STATE", "Renderer not initialized");
    }
    return renderer_->exitXR();
}

RendererState Engine::getState() const {
    if (!renderer_) {
        return RendererState::CREATED;
    }
    return renderer_->getState();
}

MemoryCounters Engine::getMemoryCounters() const {
    return globalMemoryCounters();
}

void Engine::enqueueCleanup(Handle handle) {
    std::lock_guard lock(cleanupMutex_);
    pendingCleanups_.push_back(handle);
}

void Engine::setEventCallback(EventCallback callback) {
    std::lock_guard lock(callbackMutex_);
    eventCallback_ = std::move(callback);
}

void Engine::renderThreadMain() {
    while (!shouldExit_) {
        if (renderer_ && renderer_->getState() == RendererState::RUNNING) {
            processCommandQueue();
            processCleanupQueue();
            renderer_->renderFrame();
        } else {
            std::unique_lock lock(commandMutex_);
            commandCv_.wait_for(lock, std::chrono::milliseconds(16));
        }
    }
}

void Engine::processCommandQueue() {
    assertRenderThread();
    
    std::queue<CommandBuffer> toProcess;
    {
        std::lock_guard lock(commandMutex_);
        std::swap(toProcess, commandQueue_);
    }
    
    while (!toProcess.empty()) {
        CommandBuffer& buffer = toProcess.front();
        
        for (const auto& cmd : buffer.commands) {
            auto result = executeCommand(cmd);
            if (!result.ok) {
                // Log error but continue processing
            }
        }
        
        toProcess.pop();
    }
}

void Engine::processCleanupQueue() {
    assertRenderThread();
    
    std::vector<Handle> toClean;
    {
        std::lock_guard lock(cleanupMutex_);
        std::swap(toClean, pendingCleanups_);
    }
    
    for (Handle h : toClean) {
        if (auto nodePtr = nodeRegistry_.get(h)) {
            auto& node = *nodePtr;
            node->removeFromParent();
            nodeRegistry_.release(h);
        }
    }
}

bool Engine::isRenderThread() const {
    return std::this_thread::get_id() == renderThreadId_;
}

void Engine::assertRenderThread() const {
    assert(isRenderThread() && "Must be called from render thread");
}

Result<void> Engine::executeCommand(const Command& cmd) {
    return std::visit([this](const auto& c) -> Result<void> {
        using T = std::decay_t<decltype(c)>;
        
        if constexpr (std::is_same_v<T, CreateNodeCmd>) {
            auto nodePtr = nodeRegistry_.get(c.handle);
            if (!nodePtr) {
                return Result<void>::failure("INVALID_HANDLE", "Node handle not found");
            }
            
            if (c.parentHandle.isValid()) {
                auto parentPtr = nodeRegistry_.get(c.parentHandle);
                if (parentPtr) {
                    (*parentPtr)->addChild(*nodePtr);
                }
            } else if (renderer_) {
                renderer_->getSceneGraph().getRoot()->addChild(*nodePtr);
            }
            return Result<void>::success();
        }
        else if constexpr (std::is_same_v<T, DestroyNodeCmd>) {
            enqueueCleanup(c.handle);
            return Result<void>::success();
        }
        else if constexpr (std::is_same_v<T, SetTransformCmd>) {
            auto nodePtr = nodeRegistry_.get(c.handle);
            if (!nodePtr) {
                return Result<void>::failure("INVALID_HANDLE", "Node handle not found");
            }
            (*nodePtr)->setPosition(c.position);
            (*nodePtr)->setRotation(c.rotation);
            (*nodePtr)->setScale(c.scale);
            return Result<void>::success();
        }
        else if constexpr (std::is_same_v<T, SetVisibilityCmd>) {
            auto nodePtr = nodeRegistry_.get(c.handle);
            if (!nodePtr) {
                return Result<void>::failure("INVALID_HANDLE", "Node handle not found");
            }
            (*nodePtr)->setVisible(c.visible);
            return Result<void>::success();
        }
        else if constexpr (std::is_same_v<T, ReparentCmd>) {
            auto nodePtr = nodeRegistry_.get(c.handle);
            if (!nodePtr) {
                return Result<void>::failure("INVALID_HANDLE", "Node handle not found");
            }
            
            std::shared_ptr<SceneNode> newParent;
            if (c.newParentHandle.isValid()) {
                auto parentPtr = nodeRegistry_.get(c.newParentHandle);
                if (parentPtr) {
                    newParent = *parentPtr;
                }
            }
            (*nodePtr)->reparent(newParent);
            return Result<void>::success();
        }
        else {
            // Handle other command types
            return Result<void>::success();
        }
    }, cmd);
}

} // namespace kimoyooju

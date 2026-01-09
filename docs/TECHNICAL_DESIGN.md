# Kimoyo Oju Technical Design Document

## 1. Executive Summary

This document describes the architecture for porting the KimoyoOju renderer to Nitro Modules, targeting:
- **Meta Quest / Horizon OS** (Android) via OpenXR
- **Android phones/tablets** (flat 3D rendering)
- **iOS phones/tablets** (flat 3D rendering, optional ARKit path)

The design prioritizes **stability, memory safety, and crash-free operation** over feature completeness.

---

## 2. Threading Model

### 2.1 Thread Roles

```
┌─────────────────────────────────────────────────────────────────────┐
│                         THREAD ARCHITECTURE                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                       │
│  ┌─────────────┐     CommandBuffer      ┌─────────────────────────┐ │
│  │   JS THREAD │ ─────────────────────► │     RENDER THREAD       │ │
│  │             │   (immutable, queued)  │                         │ │
│  │  - React    │                        │  - OpenXR session       │ │
│  │  - Diffing  │     Result<T>          │  - GPU context (GL/VK)  │ │
│  │  - Commands │ ◄───────────────────── │  - Swapchain            │ │
│  │             │   (async callback)     │  - Scene graph exec     │ │
│  └─────────────┘                        └─────────────────────────┘ │
│         │                                          ▲                 │
│         │ Asset requests                           │ GPU upload      │
│         ▼                                          │                 │
│  ┌─────────────┐                                   │                 │
│  │WORKER THREAD│ ──────────────────────────────────┘                 │
│  │  (pool)     │   Decoded asset data                                │
│  │  - Decode   │   (CPU memory only)                                 │
│  │  - Decompress│                                                    │
│  └─────────────┘                                                     │
│                                                                       │
└─────────────────────────────────────────────────────────────────────┘
```

### 2.2 Thread Safety Rules

| Resource Type | Owner Thread | Access Pattern |
|--------------|--------------|----------------|
| OpenXR Instance/Session | Render | Exclusive |
| Swapchain Images | Render | Exclusive |
| GL/Metal Context | Render | Exclusive |
| Scene Graph (live) | Render | Exclusive |
| Command Buffers | JS (create) → Render (consume) | Transfer ownership |
| Asset Decode | Worker | Exclusive during decode |
| GPU Upload | Render | Exclusive |
| Handle Registry | Render (write) / JS (read via copy) | Mutex-protected |

### 2.3 Critical Invariant

**JS thread NEVER touches:**
- Live GPU state (textures, buffers, shaders)
- OpenXR handles (session, swapchain, spaces)
- Active scene graph nodes

**JS thread CAN:**
- Build immutable command buffers
- Read handle IDs (not dereference)
- Query async results via callbacks

---

## 3. Ownership Model

### 3.1 RAII Wrappers

All native resources use RAII wrappers that guarantee cleanup:

```cpp
// Example: OpenXR handle wrapper
template<typename T, auto DestroyFn>
class XrHandle {
    T handle_ = XR_NULL_HANDLE;
public:
    XrHandle() = default;
    explicit XrHandle(T h) : handle_(h) {}
    ~XrHandle() { reset(); }
    
    XrHandle(XrHandle&& o) noexcept : handle_(std::exchange(o.handle_, XR_NULL_HANDLE)) {}
    XrHandle& operator=(XrHandle&& o) noexcept {
        if (this != &o) { reset(); handle_ = std::exchange(o.handle_, XR_NULL_HANDLE); }
        return *this;
    }
    
    XrHandle(const XrHandle&) = delete;
    XrHandle& operator=(const XrHandle&) = delete;
    
    void reset() {
        if (handle_ != XR_NULL_HANDLE) {
            DestroyFn(handle_);
            handle_ = XR_NULL_HANDLE;
        }
    }
    
    T get() const { return handle_; }
    T release() { return std::exchange(handle_, XR_NULL_HANDLE); }
    explicit operator bool() const { return handle_ != XR_NULL_HANDLE; }
};

// Specializations
using XrInstanceHandle = XrHandle<XrInstance, xrDestroyInstance>;
using XrSessionHandle = XrHandle<XrSession, xrDestroySession>;
using XrSpaceHandle = XrHandle<XrSpace, xrDestroySpace>;
using XrSwapchainHandle = XrHandle<XrSwapchain, xrDestroySwapchain>;
```

### 3.2 Scene Graph Ownership

```
Parent ◄──── weak_ptr ──── Child
  │                          ▲
  └──── shared_ptr ──────────┘
```

- **Parent → Child**: `std::vector<std::shared_ptr<Node>>`
- **Child → Parent**: `std::weak_ptr<Node>`
- **Root Scene**: `std::shared_ptr<Node>` owned by Renderer

This prevents reference cycles while allowing child-to-parent traversal.

### 3.3 Handle System (Generation Counters)

JS receives opaque handles, not raw pointers:

```cpp
struct Handle {
    uint32_t id;         // Slot index
    uint32_t generation; // Increment on reuse to detect stale refs
};

template<typename T>
class HandleRegistry {
    struct Slot {
        std::optional<T> value;
        uint32_t generation = 0;
    };
    std::vector<Slot> slots_;
    std::vector<uint32_t> freeList_;
    std::mutex mutex_;
    
public:
    Handle allocate(T&& value);
    std::optional<T*> get(Handle h);  // Returns nullopt if stale
    bool release(Handle h);           // Returns false if already released
};
```

**Key Safety**: A stale handle (generation mismatch) returns `nullopt`, never a dangling pointer.

---

## 4. Lifecycle State Machine

### 4.1 Renderer States

```
                    ┌─────────────────────────────────────────────────────┐
                    │              RENDERER STATE MACHINE                  │
                    └─────────────────────────────────────────────────────┘

    ┌───────────┐                                           ┌───────────┐
    │  CREATED  │ ─────── attachSurface() ─────────────────►│  RUNNING  │
    └───────────┘                                           └───────────┘
         ▲                                                       │ │
         │                                                       │ │
         │ detachSurface()              onPause() ───────────────┘ │
         │ (surface destroyed)                                     │
         │                         ┌───────────┐                   │
         └─────────────────────────│  PAUSED   │◄──────────────────┘
                                   └───────────┘
                                        │ │
                     onResume() ────────┘ │
                     (to RUNNING)         │
                                          │ surfaceLost()
                                          ▼
                                   ┌───────────────┐
                                   │ SURFACE_LOST  │
                                   └───────────────┘
                                          │
                     attachSurface() ─────┘ (back to RUNNING)

    ANY STATE ─────── destroy() ─────────► DESTROYED (terminal)
```

### 4.2 XR Session States (OpenXR)

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    OPENXR SESSION STATES                         │
    │   (mapped from XrSessionState enum)                              │
    └─────────────────────────────────────────────────────────────────┘

    IDLE ──► READY ──► SYNCHRONIZED ──► VISIBLE ──► FOCUSED
      ▲                                                  │
      │                                                  │
      └──────────────── STOPPING ◄── LOSS_PENDING ◄─────┘
                            │
                            ▼
                        EXITING
```

### 4.3 State Transition Table

| Current State | Event | Action | Next State |
|--------------|-------|--------|------------|
| CREATED | attachSurface(surface) | Create GL context, init swapchain | RUNNING |
| RUNNING | onPause() | Pause render loop, release optional resources | PAUSED |
| PAUSED | onResume() | Resume render loop | RUNNING |
| PAUSED | surfaceLost() | Mark surface invalid | SURFACE_LOST |
| RUNNING | surfaceLost() | Stop rendering, mark invalid | SURFACE_LOST |
| SURFACE_LOST | attachSurface(surface) | Recreate context/swapchain | RUNNING |
| ANY | destroy() | Full teardown (idempotent) | DESTROYED |
| DESTROYED | * | No-op | DESTROYED |

### 4.4 XR Session Loss Handling

```cpp
void Renderer::handleXrSessionStateChange(XrSessionState newState) {
    switch (newState) {
        case XR_SESSION_STATE_LOSS_PENDING:
            // Runtime is about to lose session (e.g., guardian boundary)
            // Stop submitting frames, prepare for session end
            xrSession_.beginSessionEnd();
            break;
            
        case XR_SESSION_STATE_STOPPING:
            // Must call xrEndSession
            xrSession_.end();
            transitionTo(RendererState::PAUSED);
            break;
            
        case XR_SESSION_STATE_EXITING:
            // App should exit XR mode
            notifyJS(Event::XR_SESSION_ENDED);
            transitionTo(RendererState::SURFACE_LOST);
            break;
            
        case XR_SESSION_STATE_IDLE:
            // Session created but not ready
            break;
            
        case XR_SESSION_STATE_READY:
            // Can call xrBeginSession
            xrSession_.begin(viewConfigType_);
            break;
            
        case XR_SESSION_STATE_SYNCHRONIZED:
        case XR_SESSION_STATE_VISIBLE:
        case XR_SESSION_STATE_FOCUSED:
            // Normal operation states
            transitionTo(RendererState::RUNNING);
            break;
    }
}
```

---

## 5. Command Buffer Schema

### 5.1 Structure

```typescript
interface CommandBuffer {
    version: 1;                    // Schema version for forward compat
    txId: number;                  // Transaction ID for tracking
    timestamp: number;             // Creation time (monotonic)
    commands: Command[];           // Ordered list of commands
}

type Command = 
    | CreateNodeCmd
    | DestroyNodeCmd
    | SetTransformCmd
    | SetMaterialCmd
    | ReparentCmd
    | LoadAssetCmd
    | SetVisibilityCmd
    | SetLightCmd
    | SetCameraCmd;

interface CreateNodeCmd {
    type: 'CREATE_NODE';
    handle: Handle;               // Pre-allocated handle from JS
    nodeType: NodeType;           // 'mesh' | 'light' | 'camera' | 'group' | 'text'
    parentHandle: Handle | null;
}

interface SetTransformCmd {
    type: 'SET_TRANSFORM';
    handle: Handle;
    position: [number, number, number];
    rotation: [number, number, number, number];  // Quaternion
    scale: [number, number, number];
}

interface LoadAssetCmd {
    type: 'LOAD_ASSET';
    handle: Handle;
    uri: string;
    assetType: 'model' | 'texture' | 'audio';
}
```

### 5.2 Validation Rules

```cpp
class CommandBufferValidator {
public:
    struct ValidationResult {
        bool valid;
        std::string errorCode;
        std::string errorMessage;
        size_t errorCommandIndex;
    };
    
    ValidationResult validate(const CommandBuffer& buffer) {
        // 1. Version check
        if (buffer.version != CURRENT_VERSION) {
            return {false, "INVALID_VERSION", 
                    fmt::format("Expected version {}, got {}", CURRENT_VERSION, buffer.version), 0};
        }
        
        // 2. Size limits
        if (buffer.commands.size() > MAX_COMMANDS_PER_BUFFER) {
            return {false, "TOO_MANY_COMMANDS",
                    fmt::format("Max {} commands, got {}", MAX_COMMANDS_PER_BUFFER, buffer.commands.size()), 0};
        }
        
        // 3. Per-command validation
        for (size_t i = 0; i < buffer.commands.size(); ++i) {
            auto result = validateCommand(buffer.commands[i], i);
            if (!result.valid) return result;
        }
        
        return {true, "", "", 0};
    }
    
private:
    ValidationResult validateCommand(const Command& cmd, size_t index);
    bool validateHandle(Handle h);
    bool validateFloat(float f);  // Rejects NaN, Infinity
    bool validateString(std::string_view s, size_t maxLen);
    bool validateArray(const auto& arr, size_t expectedLen);
};
```

---

## 6. OpenXR Frame Loop

### 6.1 Frame Structure

```cpp
void XRSession::frameLoop() {
    while (running_) {
        // 1. Wait for frame timing signal from runtime
        XrFrameWaitInfo waitInfo{XR_TYPE_FRAME_WAIT_INFO};
        XrFrameState frameState{XR_TYPE_FRAME_STATE};
        XrResult result = xrWaitFrame(session_, &waitInfo, &frameState);
        
        if (XR_FAILED(result)) {
            handleXrError(result);
            continue;
        }
        
        // 2. Begin frame
        XrFrameBeginInfo beginInfo{XR_TYPE_FRAME_BEGIN_INFO};
        result = xrBeginFrame(session_, &beginInfo);
        if (XR_FAILED(result)) {
            handleXrError(result);
            continue;
        }
        
        // 3. Process any pending command buffers
        processCommandQueue();
        
        // 4. Render if session is visible/focused
        std::vector<XrCompositionLayerBaseHeader*> layers;
        if (frameState.shouldRender) {
            renderFrame(frameState.predictedDisplayTime, layers);
        }
        
        // 5. End frame (submit layers)
        XrFrameEndInfo endInfo{XR_TYPE_FRAME_END_INFO};
        endInfo.displayTime = frameState.predictedDisplayTime;
        endInfo.enkimoyoojunmentBlendMode = XR_ENKIMOYOOJUNMENT_BLEND_MODE_OPAQUE;
        endInfo.layerCount = static_cast<uint32_t>(layers.size());
        endInfo.layers = layers.data();
        
        result = xrEndFrame(session_, &endInfo);
        if (XR_FAILED(result)) {
            handleXrError(result);
        }
    }
}

void XRSession::renderFrame(XrTime predictedTime, 
                            std::vector<XrCompositionLayerBaseHeader*>& outLayers) {
    // Locate views (eye positions)
    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCount = 0;
    std::vector<XrView> views(viewCount_);
    
    XrViewLocateInfo locateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    locateInfo.viewConfigurationType = viewConfigType_;
    locateInfo.displayTime = predictedTime;
    locateInfo.space = localSpace_;
    
    xrLocateViews(session_, &locateInfo, &viewState, viewCount_, &viewCount, views.data());
    
    // Acquire swapchain images for each view
    for (uint32_t i = 0; i < viewCount; ++i) {
        uint32_t imageIndex;
        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        xrAcquireSwapchainImage(swapchains_[i], &acquireInfo, &imageIndex);
        
        XrSwapchainImageWaitInfo waitImageInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitImageInfo.timeout = XR_INFINITE_DURATION;
        xrWaitSwapchainImage(swapchains_[i], &waitImageInfo);
        
        // Render to this image
        renderer_->renderView(i, views[i], swapchainImages_[i][imageIndex]);
        
        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        xrReleaseSwapchainImage(swapchains_[i], &releaseInfo);
    }
    
    // Build projection layer
    projectionLayer_.space = localSpace_;
    projectionLayer_.viewCount = viewCount;
    projectionLayer_.views = projectionViews_.data();
    outLayers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&projectionLayer_));
}
```

### 6.2 Swapchain Image Lifecycle

```
    ┌─────────────────────────────────────────────────────────┐
    │              SWAPCHAIN IMAGE LIFECYCLE                   │
    └─────────────────────────────────────────────────────────┘
    
         xrAcquireSwapchainImage()
                │
                ▼
    ┌───────────────────┐
    │     ACQUIRED      │  (image index returned)
    └───────────────────┘
                │
                │ xrWaitSwapchainImage()
                ▼
    ┌───────────────────┐
    │      READY        │  (safe to render)
    └───────────────────┘
                │
                │ (GPU rendering)
                ▼
    ┌───────────────────┐
    │    RENDERING      │
    └───────────────────┘
                │
                │ xrReleaseSwapchainImage()
                ▼
    ┌───────────────────┐
    │    RELEASED       │  (back to runtime)
    └───────────────────┘
    
    CRITICAL: Must release ALL acquired images before xrEndFrame()
              Failure to release causes deadlock or crash
```

---

## 7. Disposal Semantics

### 7.1 JS-Side Disposal

```typescript
class NativeHandle implements Disposable {
    private _handle: Handle;
    private _disposed = false;
    
    dispose(): void {
        if (this._disposed) return;  // Idempotent
        this._disposed = true;
        
        // Enqueue destroy command
        KimoyoOjuEngine.enqueueCommand({
            type: 'DESTROY_NODE',
            handle: this._handle
        });
    }
    
    // Called by GC via FinalizationRegistry
    static _pointerCaptureRegistry = new FinalizationRegistry((handle: Handle) => {
        // Backstop: if dispose() wasn't called, clean up anyway
        KimoyoOjuEngine.enqueueCleanup(handle);
    });
}
```

### 7.2 Native-Side Cleanup

```cpp
class Engine {
    void processCleanupQueue() {
        // MUST run on render thread
        assert(isRenderThread());
        
        std::vector<Handle> toClean;
        {
            std::lock_guard lock(cleanupMutex_);
            std::swap(toClean, pendingCleanups_);
        }
        
        for (Handle h : toClean) {
            // Safe: generation check prevents double-free
            if (auto* node = nodeRegistry_.get(h)) {
                // Detach from parent
                if (auto parent = node->parent().lock()) {
                    parent->removeChild(node->shared_from_this());
                }
                // Release from registry (destructor runs here)
                nodeRegistry_.release(h);
            }
        }
    }
};
```

### 7.3 Teardown Order (CRITICAL)

```cpp
void Renderer::destroy() {
    if (state_ == RendererState::DESTROYED) return;  // Idempotent
    
    // 1. Stop render loop first
    running_ = false;
    if (renderThread_.joinable()) {
        renderThread_.join();
    }
    
    // 2. Destroy scene graph (releases all node GPU resources)
    scene_.reset();
    
    // 3. Destroy XR session (releases swapchains, spaces)
    xrSession_.reset();
    
    // 4. Destroy graphics context
    graphicsDevice_.reset();
    
    // 5. Destroy XR instance
    xrInstance_.reset();
    
    // 6. Mark state
    state_ = RendererState::DESTROYED;
}
```

---

## 8. Error Handling Strategy

### 8.1 Result Type (Never throw across JSI)

```cpp
template<typename T>
struct Result {
    bool ok;
    std::variant<T, Error> data;
    
    static Result success(T value) {
        return {true, std::move(value)};
    }
    
    static Result failure(std::string code, std::string message) {
        return {false, Error{std::move(code), std::move(message)}};
    }
};

struct Error {
    std::string code;
    std::string message;
};
```

### 8.2 Error Codes

| Code | Meaning |
|------|---------|
| `INVALID_HANDLE` | Handle doesn't exist or generation mismatch |
| `INVALID_STATE` | Operation invalid in current renderer state |
| `VALIDATION_FAILED` | Command buffer validation failed |
| `XR_ERROR` | OpenXR error (includes XrResult code) |
| `GRAPHICS_ERROR` | GL/Vulkan/Metal error |
| `ASSET_LOAD_FAILED` | Asset decode/load failed |
| `OUT_OF_MEMORY` | Memory allocation failed |
| `TIMEOUT` | Operation timed out |

### 8.3 Exception Boundary

```cpp
// All Nitro-exposed methods use this wrapper
#define NITRO_SAFE_CALL(expr) \
    try { \
        return expr; \
    } catch (const std::exception& e) { \
        return Result<decltype(expr)::value_type>::failure( \
            "INTERNAL_ERROR", e.what()); \
    } catch (...) { \
        return Result<decltype(expr)::value_type>::failure( \
            "UNKNOWN_ERROR", "Unknown exception"); \
    }
```

---

## 9. Memory Tracking

### 9.1 Counters

```cpp
struct MemoryCounters {
    std::atomic<uint32_t> liveNodes{0};
    std::atomic<uint32_t> liveTextures{0};
    std::atomic<uint32_t> liveBuffers{0};
    std::atomic<uint32_t> liveSwapchains{0};
    std::atomic<uint64_t> gpuMemoryBytes{0};
    std::atomic<uint64_t> cpuMemoryBytes{0};
};

// Global instance
inline MemoryCounters g_memCounters;

// Usage in RAII wrappers
class Texture {
public:
    Texture() { g_memCounters.liveTextures++; }
    ~Texture() { g_memCounters.liveTextures--; }
};
```

### 9.2 Leak Detection

```cpp
void Engine::assertNoLeaks() {
    assert(g_memCounters.liveNodes == 0);
    assert(g_memCounters.liveTextures == 0);
    assert(g_memCounters.liveBuffers == 0);
    assert(g_memCounters.liveSwapchains == 0);
}
```

---

## 10. Platform Integration

### 10.1 Android (Quest + Phones)

```
┌─────────────────────────────────────────────────────────────┐
│                    ANDROID INTEGRATION                       │
└─────────────────────────────────────────────────────────────┘

  React Native
       │
       ▼
  KimoyoOjuViewManager (Nitro HybridView)
       │
       │ createView() / dropView()
       ▼
  KimoyoOjuSurfaceView (extends SurfaceView)
       │
       │ surfaceCreated / surfaceDestroyed / surfaceChanged
       ▼
  Native Engine (via JNI)
       │
       ├──► EGL Context (phones)
       │
       └──► OpenXR with XR_KHR_opengl_es_enable (Quest)
```

### 10.2 iOS

```
┌─────────────────────────────────────────────────────────────┐
│                      IOS INTEGRATION                         │
└─────────────────────────────────────────────────────────────┘

  React Native
       │
       ▼
  KimoyoOjuViewManager (Nitro HybridView)
       │
       │ createView() / dropView()
       ▼
  KimoyoOjuMetalView (UIView with CAMetalLayer)
       │
       │ layoutSubviews / didMoveToWindow
       ▼
  Native Engine
       │
       └──► Metal Context
```

### 10.3 Lifecycle Event Mapping

| Platform Event | Engine Action |
|---------------|---------------|
| Android: `surfaceCreated` | `attachSurface()` |
| Android: `surfaceDestroyed` | `detachSurface()` |
| Android: `onPause` | `pause()` |
| Android: `onResume` | `resume()` |
| iOS: `didMoveToWindow (window != nil)` | `attachSurface()` |
| iOS: `didMoveToWindow (window == nil)` | `detachSurface()` |
| iOS: `applicationWillResignActive` | `pause()` |
| iOS: `applicationDidBecomeActive` | `resume()` |
| Quest: `XR_SESSION_STATE_STOPPING` | `pause()` + `endSession()` |
| Quest: `XR_SESSION_STATE_READY` | `beginSession()` + `resume()` |

---

## 11. Edge Cases and Crash Prevention

### 11.1 Surface Loss During Rendering

```cpp
void Renderer::renderFrame() {
    // Check surface validity BEFORE any GL calls
    if (!surface_.isValid()) {
        // Surface was destroyed between frame start and now
        return;  // Skip frame, don't crash
    }
    
    // ... rendering code ...
    
    // Check again before swap
    if (!surface_.isValid()) {
        return;  // Surface lost mid-frame, skip swap
    }
    
    surface_.swapBuffers();
}
```

### 11.2 Pause/Resume Reentrancy

```cpp
void Renderer::pause() {
    std::lock_guard lock(lifecycleMutex_);
    
    if (state_ != RendererState::RUNNING) {
        return;  // Already paused or in invalid state
    }
    
    // Set state FIRST to prevent reentrancy
    state_ = RendererState::PAUSED;
    
    // Then do actual pause work
    pauseRenderLoop();
}

void Renderer::resume() {
    std::lock_guard lock(lifecycleMutex_);
    
    if (state_ != RendererState::PAUSED) {
        return;  // Not paused, or surface lost
    }
    
    state_ = RendererState::RUNNING;
    resumeRenderLoop();
}
```

### 11.3 XR Session Loss

```cpp
void XRSession::handleSessionLoss() {
    // 1. Stop submitting frames immediately
    shouldRender_ = false;
    
    // 2. Release all acquired swapchain images
    for (auto& swapchain : swapchains_) {
        if (swapchain.imageAcquired) {
            xrReleaseSwapchainImage(swapchain.handle, nullptr);
            swapchain.imageAcquired = false;
        }
    }
    
    // 3. End session if running
    if (sessionRunning_) {
        xrEndSession(session_);
        sessionRunning_ = false;
    }
    
    // 4. Notify JS layer
    notifySessionLost();
    
    // 5. Transition to appropriate state
    // Do NOT destroy session here - wait for EXITING state
}
```

### 11.4 Swapchain Timeout

```cpp
XrResult XRSession::waitSwapchainImage(XrSwapchain swapchain) {
    XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
    waitInfo.timeout = 100'000'000;  // 100ms timeout, not infinite
    
    XrResult result = xrWaitSwapchainImage(swapchain, &waitInfo);
    
    if (result == XR_TIMEOUT_EXPIRED) {
        // Swapchain stalled - likely GPU hang or runtime issue
        // Release the image to prevent deadlock
        xrReleaseSwapchainImage(swapchain, nullptr);
        return result;
    }
    
    return result;
}
```

---

## 12. Performance Considerations

### 12.1 Command Batching

- All prop updates collected during React commit phase
- Single `submit(CommandBuffer)` call per frame
- Native side processes commands in batch, not one at a time

### 12.2 Diff Calculation

```typescript
// In KimoyoOjuReconciler
function commitUpdate(handle: Handle, oldProps: Props, newProps: Props): Command[] {
    const commands: Command[] = [];
    
    // Only emit commands for changed props
    if (!vec3Equal(oldProps.position, newProps.position) ||
        !quatEqual(oldProps.rotation, newProps.rotation) ||
        !vec3Equal(oldProps.scale, newProps.scale)) {
        commands.push({
            type: 'SET_TRANSFORM',
            handle,
            position: newProps.position ?? [0, 0, 0],
            rotation: newProps.rotation ?? [0, 0, 0, 1],
            scale: newProps.scale ?? [1, 1, 1]
        });
    }
    
    // ... other prop diffs ...
    
    return commands;
}
```

### 12.3 Asset Pipeline

```
  JS Request (uri)
       │
       ▼
  Worker Thread: Fetch + Decode
       │
       │ (CPU memory only)
       ▼
  Render Thread: GPU Upload
       │
       ▼
  Handle Ready Callback to JS
```

---

## 13. Appendix: Key Constants

```cpp
namespace kimoyooju::constants {
    constexpr uint32_t MAX_COMMANDS_PER_BUFFER = 10000;
    constexpr uint32_t MAX_STRING_LENGTH = 4096;
    constexpr uint32_t MAX_NODES = 100000;
    constexpr uint32_t MAX_TEXTURES = 1000;
    constexpr uint32_t COMMAND_BUFFER_VERSION = 1;
    constexpr int64_t SWAPCHAIN_WAIT_TIMEOUT_NS = 100'000'000;  // 100ms
}
```

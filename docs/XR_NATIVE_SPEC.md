# Kimoyo Oju: Native Architecture Specification

## Module Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         React Native JS                          │
│   KimoyoOjuXR Module  │  Components  │  Hooks  │  Types               │
├─────────────────────────────────────────────────────────────────┤
│                    Nitro Module Bridge                           │
│                    (HybridKimoyoOjuXR.hpp)                            │
├─────────────────────────────────────────────────────────────────┤
│                      C++ Core Layer                              │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐       │
│  │XRSession │ Anchor   │ SceneMesh│  Hand    │  Depth   │       │
│  │ Manager  │ Manager  │ Manager  │ Tracker  │ Provider │       │
│  └────┬─────┴────┬─────┴────┬─────┴────┬─────┴────┬─────┘       │
│       └──────────┴──────────┴──────────┴──────────┘             │
│                    Platform Abstraction                          │
│  ┌──────────┬──────────┬──────────┬──────────┐                  │
│  │  Quest   │  ARCore  │  ARKit   │  OpenXR  │                  │
│  │  Impl    │  Impl    │  Impl    │  Impl    │                  │
│  └──────────┴──────────┴──────────┴──────────┘                  │
├─────────────────────────────────────────────────────────────────┤
│                      Render Pipeline                             │
│  Camera BG → Depth Pass → Scene Mesh → Virtual → Composite      │
└─────────────────────────────────────────────────────────────────┘
```

## Threading Model

### Thread Responsibilities

| Thread | Responsibility | Blocking Allowed |
|--------|---------------|------------------|
| Main/UI | JS execution, event dispatch | NO |
| Render | GL/Metal/Vulkan commands | NO |
| XR Session | Platform updates, poses | Brief |
| Worker Pool | Mesh processing, assets | YES |

### Synchronization

- **Lock-free queues** between threads
- **Double/triple buffering** for frame data
- **Atomic reference counting** for resources
- **Event coalescing** to prevent JS flooding

```cpp
// Command queue between threads
template<typename T>
class CommandQueue {
    std::mutex mutex_;
    std::deque<T> queue_;
    static constexpr size_t MAX_SIZE = 256;
public:
    void push(T&& cmd);
    bool tryPop(T& out);
    void drain(std::function<void(T&)> handler);
};

// Double buffer for frame data
template<typename T>
class DoubleBuffer {
    std::array<T, 2> buffers_;
    std::atomic<int> readIndex_{0};
public:
    T& writeBuffer() { return buffers_[1 - readIndex_.load()]; }
    const T& readBuffer() const { return buffers_[readIndex_.load()]; }
    void swap() { readIndex_.store(1 - readIndex_.load()); }
};
```

## Memory Management

### Ownership Rules

1. **Native owns all GPU resources** (textures, buffers, meshes)
2. **JS receives handles (IDs)**, never raw pointers
3. **Reference counting** for shared resources
4. **Explicit disposal** for persistent resources
5. **Automatic cleanup** on session end

### Resource Pools

```cpp
template<typename T>
class ResourcePool {
public:
    struct Handle { uint32_t index; uint32_t generation; };
    
    Handle allocate();
    void release(Handle h);
    T* get(Handle h);
    
    size_t activeCount() const;
    size_t peakUsage() const;
    
private:
    std::vector<Slot> slots_;
    std::vector<uint32_t> freeList_;
};

// Pre-allocated pools
ResourcePool<MeshBlock> meshPool_{1024};      // 1024 mesh blocks
ResourcePool<XRAnchor> anchorPool_{256};      // 256 anchors
ResourcePool<EventData> eventPool_{512};      // 512 events
```

### Memory Budget

```cpp
class MemoryBudget {
    size_t cpuLimit_ = 256 * 1024 * 1024;  // 256 MB
    size_t gpuLimit_ = 512 * 1024 * 1024;  // 512 MB
    std::atomic<size_t> cpuUsed_{0};
    std::atomic<size_t> gpuUsed_{0};
    
public:
    bool canAllocate(size_t cpu, size_t gpu) const;
    void track(size_t cpu, size_t gpu);
    void untrack(size_t cpu, size_t gpu);
    void enforceLimit(std::function<void()> evict);
};
```

## Resource Lifecycle

### State Machine

```
CREATED → INITIALIZING → READY ⟷ PAUSED → DESTROYING → DESTROYED
                           ↓
                      RESUMING
```

### Lifecycle Events

| Event | Action |
|-------|--------|
| onCreate | Allocate pools, init managers |
| onStart | Request permissions |
| onResume | Resume XR session, restore resources |
| onPause | Pause XR, release non-essential resources |
| onStop | Save persistent anchors |
| onDestroy | Release all resources, cleanup |
| onPermissionRevoked | Degrade gracefully |

### Automatic Cleanup

```cpp
class ResourceRegistry {
public:
    uint64_t registerResource(CleanupFn fn, LifecycleState cleanupOn);
    void unregisterResource(uint64_t id);
    void onLifecycleChange(LifecycleState newState);
};
```

## Event Dispatch

### Throttling & Coalescing

```cpp
class EventDispatcher {
    std::unordered_map<std::string, uint32_t> throttleMs_;
    std::unordered_map<std::string, uint64_t> lastDispatch_;
    std::unordered_map<std::string, json> pending_;
    
public:
    void setThrottle(const std::string& event, uint32_t ms);
    void queue(const std::string& event, json data);
    void flush(); // Call from main thread
};
```

### Default Throttles

| Event | Throttle | Reason |
|-------|----------|--------|
| depthFrame | 33ms | 30 FPS max |
| sceneMeshUpdated | 100ms | Heavy data |
| handTrackingUpdated | 16ms | 60 FPS |
| anchorUpdated | 50ms | State changes |
| interaction | 0ms | Immediate |

## Platform Implementations

### Quest/Horizon OS

```cpp
class QuestXRProvider : public XRProvider {
    XrInstance instance_;
    XrSession session_;
    XrSpace appSpace_;
    
    // Extensions
    XrEnkimoyoojunmentDepthSwapchainMETA depthSwapchain_;
    XrSceneMSFT scene_;
    XrHandTrackerEXT handTrackers_[2];
    XrSpatialAnchorMSFT* anchors_;
    
public:
    bool initialize() override;
    void processFrame() override;
    
    // Feature providers
    DepthFrame acquireDepthFrame();
    SceneMeshUpdate getSceneMeshUpdate();
    HandData getHandData(Handedness hand);
    AnchorResult createAnchor(const Pose& pose);
};
```

### ARCore

```cpp
class ARCoreProvider : public XRProvider {
    ArSession* session_;
    ArFrame* frame_;
    ArConfig* config_;
    
public:
    bool initialize() override;
    void processFrame() override;
    
    DepthFrame acquireDepthFrame();  // ArFrame_acquireDepthImage
    std::vector<Plane> getPlanes();  // AR_TRACKABLE_PLANE
    LightEstimate getLightEstimate();
    AnchorResult createAnchor(const Pose& pose);
};
```

### ARKit

```cpp
class ARKitProvider : public XRProvider {
    ARSession* session_;
    ARFrame* currentFrame_;
    ARWorldTrackingConfiguration* config_;
    
public:
    bool initialize() override;
    void processFrame() override;
    
    DepthFrame acquireDepthFrame();  // sceneDepth
    SceneMeshUpdate getSceneMesh();  // ARMeshAnchor
    HandData getHandData();          // VNDetectHumanHandPoseRequest
    AnchorResult createAnchor(const Pose& pose);
};
```

## Render Pipeline

### MR Composition Layers

```
Layer 0: Camera Background (passthrough)
Layer 1: Depth/Occlusion mask
Layer 2: Scene mesh (invisible, for shadows/collisions)
Layer 3: Virtual opaque objects
Layer 4: Virtual transparent objects
Layer 5: UI overlay
```

### Occlusion Shader

```glsl
// Fragment shader for depth-based occlusion
uniform sampler2D u_depthTexture;
uniform sampler2D u_virtualDepth;
uniform mat4 u_depthProjection;

void main() {
    float realDepth = texture(u_depthTexture, v_texCoord).r;
    float virtualDepth = texture(u_virtualDepth, v_texCoord).r;
    
    // Discard virtual fragments behind real geometry
    if (virtualDepth > realDepth + 0.01) {
        discard;
    }
    
    fragColor = texture(u_virtualColor, v_texCoord);
}
```

## Performance Strategy

### Zero-Copy Where Possible

- Depth texture: GPU-only, shader sampling
- Scene mesh: Ring buffer, incremental updates
- Hand tracking: Shared memory with platform

### Pooling

- Mesh vertex buffers: Pre-allocated, reused
- Event objects: Pool of 512
- Transform matrices: Stack allocator

### Budgets

| Metric | Budget | Action if Exceeded |
|--------|--------|-------------------|
| Frame time | 11ms (90 FPS) | Reduce mesh quality |
| CPU time | 4ms/frame | Skip semantics |
| GPU memory | 512MB | Evict old meshes |
| Event queue | 256 items | Drop oldest |

## Error Handling

### Graceful Degradation

```cpp
enum class FeatureFallback {
    DEPTH_TO_NO_OCCLUSION,
    MESH_TO_PLANES_ONLY,
    HANDS_TO_CONTROLLERS,
    ANCHORS_TO_SESSION_ONLY
};

void onFeatureUnavailable(Feature f) {
    switch (f) {
        case Feature::DEPTH:
            setOcclusionMode(OcclusionMode::DISABLED);
            emitEvent("occlusionStateChanged", {{"active", false}});
            break;
        // ...
    }
}
```

### Crash Protection

- Watchdog timer for XR session (5s timeout)
- ANR protection: No blocking on main thread
- Exception boundaries around platform calls
- Automatic session recovery on failure

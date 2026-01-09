# Kimoyo Oju: Test Specification & Acceptance Criteria

## Test Categories

### Unit Tests (Native C++)

```cpp
// Test: Anchor state machine transitions
TEST(AnchorManager, StateTransitions) {
    AnchorManager mgr;
    auto handle = mgr.createAnchor(Pose::identity());
    
    EXPECT_EQ(mgr.getState(handle), AnchorState::PENDING);
    
    mgr.simulateLocated(handle);
    EXPECT_EQ(mgr.getState(handle), AnchorState::LOCATED);
    
    mgr.simulateLost(handle);
    EXPECT_EQ(mgr.getState(handle), AnchorState::LOST);
}

// Test: Resource pool allocation/deallocation
TEST(ResourcePool, AllocateRelease) {
    ResourcePool<MeshBlock> pool(100);
    
    auto h1 = pool.allocate();
    auto h2 = pool.allocate();
    EXPECT_NE(h1.index, h2.index);
    
    pool.release(h1);
    auto h3 = pool.allocate();
    EXPECT_EQ(h1.index, h3.index);
    EXPECT_NE(h1.generation, h3.generation);
}

// Test: Event throttling
TEST(EventDispatcher, Throttling) {
    EventDispatcher dispatcher;
    dispatcher.setThrottle("depthFrame", 33);
    
    int callCount = 0;
    dispatcher.setCallback([&](auto, auto) { callCount++; });
    
    // Queue 10 events rapidly
    for (int i = 0; i < 10; i++) {
        dispatcher.queue("depthFrame", {});
    }
    dispatcher.flush();
    
    // Should coalesce to 1
    EXPECT_EQ(callCount, 1);
}

// Test: Memory budget enforcement
TEST(MemoryBudget, EnforceLimits) {
    MemoryBudget budget;
    budget.setLimit(100, 200); // 100 CPU, 200 GPU
    
    EXPECT_TRUE(budget.canAllocate(50, 100));
    budget.track(50, 100);
    
    EXPECT_TRUE(budget.canAllocate(50, 100));
    EXPECT_FALSE(budget.canAllocate(51, 101));
}
```

### Unit Tests (TypeScript)

```typescript
// Test: Feature detection
describe('KimoyoOjuXR.getFeatureSupport', () => {
  it('returns valid feature flags', async () => {
    const features = await KimoyoOjuXR.getFeatureSupport();
    expect(typeof features.depthSensing).toBe('boolean');
    expect(typeof features.sceneMesh).toBe('boolean');
    expect(typeof features.handTracking).toBe('boolean');
  });
});

// Test: Anchor creation
describe('KimoyoOjuXR.createAnchor', () => {
  it('creates anchor with valid pose', async () => {
    const pose = { position: [0, 0, -1], orientation: [0, 0, 0, 1], timestamp: Date.now() };
    const anchor = await KimoyoOjuXR.createAnchor(pose);
    
    expect(anchor.id).toBeDefined();
    expect(anchor.state).toBe('pending');
  });
  
  it('rejects invalid pose', async () => {
    await expect(KimoyoOjuXR.createAnchor(null)).rejects.toThrow();
  });
});

// Test: Event subscription
describe('KimoyoOjuXR.addEventListener', () => {
  it('receives events and can unsubscribe', async () => {
    const events: any[] = [];
    const unsub = KimoyoOjuXR.addEventListener('anchorUpdated', (e) => events.push(e));
    
    // Trigger anchor creation
    await KimoyoOjuXR.createAnchor({ position: [0, 0, 0], orientation: [0, 0, 0, 1], timestamp: 0 });
    
    // Wait for event
    await new Promise(r => setTimeout(r, 100));
    expect(events.length).toBeGreaterThan(0);
    
    unsub();
  });
});
```

### Integration Tests

```typescript
// Test: Full anchor lifecycle
describe('Anchor Lifecycle', () => {
  it('create → save → load → delete', async () => {
    // Create
    const pose = { position: [0, 1, -2], orientation: [0, 0, 0, 1], timestamp: Date.now() };
    const anchor = await KimoyoOjuXR.createAnchor(pose);
    expect(anchor.state).toBe('pending');
    
    // Wait for located
    await waitForState(anchor.id, 'located', 5000);
    
    // Save
    const uuid = await KimoyoOjuXR.saveAnchor(anchor.id);
    expect(uuid).toBeDefined();
    
    // End session, start new one
    await KimoyoOjuXR.endSession();
    await KimoyoOjuXR.requestSession('immersive-mr');
    
    // Load
    const loaded = await KimoyoOjuXR.loadAnchors([uuid]);
    expect(loaded.length).toBe(1);
    expect(loaded[0].uuid).toBe(uuid);
    
    // Delete
    await KimoyoOjuXR.deleteAnchor(uuid);
    const remaining = await KimoyoOjuXR.getSavedAnchorUuids();
    expect(remaining).not.toContain(uuid);
  });
});

// Test: Scene mesh updates
describe('Scene Mesh', () => {
  it('receives incremental updates', async () => {
    const updates: SceneMeshUpdate[] = [];
    
    await KimoyoOjuXR.enableSceneMesh({ updateThrottleMs: 100 });
    KimoyoOjuXR.addEventListener('sceneMeshUpdated', (e) => updates.push(e.update));
    
    // Wait for updates
    await new Promise(r => setTimeout(r, 2000));
    
    expect(updates.length).toBeGreaterThan(0);
    expect(updates[0].added.length + updates[0].updated.length).toBeGreaterThan(0);
  });
});

// Test: Depth occlusion
describe('Depth Occlusion', () => {
  it('configures and activates', async () => {
    const success = await KimoyoOjuXR.configureDepth({
      enabled: true,
      quality: 'medium',
      occlusionMode: 'depth',
      nearClip: 0.1,
      farClip: 100
    });
    
    expect(success).toBe(true);
    
    let active = false;
    KimoyoOjuXR.addEventListener('occlusionStateChanged', (e) => { active = e.isActive; });
    KimoyoOjuXR.setOcclusionEnabled(true);
    
    await new Promise(r => setTimeout(r, 500));
    expect(active).toBe(true);
  });
});
```

## On-Device Test Scenarios

### Quest/Horizon OS Test Matrix

| Scenario | Steps | Expected Result | Priority |
|----------|-------|-----------------|----------|
| **Depth Occlusion** | Place virtual object behind real table | Object occluded by table | P0 |
| **Scene Mesh** | Walk around room | Mesh updates incrementally | P0 |
| **Persistent Anchor** | Place anchor, restart app | Anchor relocates | P0 |
| **Hand Tracking** | Pinch gesture | Event fires, strength > 0.8 | P0 |
| **Passthrough** | View real world | Clear, low latency | P0 |
| **Multi-Anchor** | Place 10 anchors | All track simultaneously | P1 |
| **Session Recovery** | Remove/replace headset | Session recovers | P1 |
| **Permission Revoke** | Deny spatial data | Graceful degradation | P1 |

### Android ARCore Test Matrix

| Scenario | Steps | Expected Result | Priority |
|----------|-------|-----------------|----------|
| **Plane Detection** | Point at floor/wall | Planes detected, classified | P0 |
| **Depth API** | Enable depth occlusion | Objects occluded correctly | P0 |
| **Hit Test** | Tap on detected plane | Hit result with pose | P0 |
| **Cloud Anchors** | Save/load anchor | Anchor persists across sessions | P1 |
| **Light Estimation** | Move to different lighting | Intensity/color updates | P1 |
| **Low Light** | Dim enkimoyoojunment | Tracking degrades gracefully | P1 |

### iOS ARKit Test Matrix

| Scenario | Steps | Expected Result | Priority |
|----------|-------|-----------------|----------|
| **LiDAR Depth** | iPhone 12+ with LiDAR | Depth occlusion works | P0 |
| **Scene Mesh** | iPad Pro with LiDAR | Mesh reconstructed | P0 |
| **World Map** | Save/load world map | Relocalization succeeds | P1 |
| **People Occlusion** | Person walks in front | Virtual objects occluded | P1 |

## Performance Budgets

### Frame Timing (90 FPS target = 11.1ms)

| Component | Budget | Action if Exceeded |
|-----------|--------|-------------------|
| XR Session Update | 2ms | Skip semantics |
| Scene Mesh Update | 3ms | Reduce update rate |
| Render (all passes) | 5ms | Reduce quality |
| JS Event Dispatch | 1ms | Drop events |
| **Total** | **11ms** | |

### Memory Limits

| Resource | Limit | Action if Exceeded |
|----------|-------|-------------------|
| Scene Mesh | 100MB | Evict distant blocks |
| Depth Textures | 50MB | Reduce resolution |
| Anchor Data | 10MB | Limit anchor count |
| Event Queue | 1MB | Drop oldest events |
| **GPU Total** | **512MB** | |
| **CPU Total** | **256MB** | |

### Startup Time

| Phase | Budget |
|-------|--------|
| Module load | < 100ms |
| Session request | < 500ms |
| First frame | < 1000ms |
| Feature ready (depth/mesh) | < 2000ms |

## Instrumentation & Logging

### Log Levels

```cpp
enum class LogLevel { VERBOSE, DEBUG, INFO, WARN, ERROR };

// Performance markers
KIMOYOOJU_PERF_BEGIN("scene_mesh_update");
// ... work ...
KIMOYOOJU_PERF_END("scene_mesh_update"); // Logs duration

// Feature state
KIMOYOOJU_LOG_INFO("Depth", "Occlusion enabled, quality=%s", qualityStr);

// Errors
KIMOYOOJU_LOG_ERROR("Anchor", "Failed to save: %s", error.message());
```

### Metrics Collection

```typescript
interface XRMetrics {
  fps: number;
  frameTimeMs: number;
  cpuTimeMs: number;
  gpuTimeMs: number;
  memoryUsedMB: number;
  anchorCount: number;
  meshBlockCount: number;
  trackingState: string;
  lastError?: string;
}

// Collect every 1s
KimoyoOjuXR.getMetrics(): Promise<XRMetrics>;
```

## Crash Protection

### Watchdog Timer

```cpp
class XRWatchdog {
    std::thread watchdogThread_;
    std::atomic<uint64_t> lastFrameTime_{0};
    
    void watchdogLoop() {
        while (running_) {
            auto now = currentTimeMs();
            if (now - lastFrameTime_.load() > 5000) {
                KIMOYOOJU_LOG_ERROR("Watchdog", "XR session unresponsive, recovering...");
                recoverSession();
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
public:
    void onFrameComplete() { lastFrameTime_ = currentTimeMs(); }
};
```

### ANR Protection

- No blocking calls on UI thread
- Async permission requests
- Background thread for heavy operations
- Timeout on all platform API calls

### Exception Boundaries

```cpp
template<typename F>
auto safeCall(F&& fn, const char* context) -> decltype(fn()) {
    try {
        return fn();
    } catch (const std::exception& e) {
        KIMOYOOJU_LOG_ERROR(context, "Exception: %s", e.what());
        return decltype(fn()){}; // Default value
    } catch (...) {
        KIMOYOOJU_LOG_ERROR(context, "Unknown exception");
        return decltype(fn()){}; 
    }
}
```

## Acceptance Criteria Summary

### P0 Features (Must Pass)

- [ ] Depth occlusion renders correctly on Quest
- [ ] Scene mesh updates without memory leaks
- [ ] Persistent anchors survive app restart
- [ ] Hand tracking events fire with correct poses
- [ ] Passthrough composition is visually correct
- [ ] No crashes during 30-minute session
- [ ] Memory stays under 512MB GPU / 256MB CPU
- [ ] Frame rate maintains 90 FPS average

### P1 Features (Should Pass)

- [ ] Scene semantics labels are accurate
- [ ] Shared anchors sync between devices
- [ ] Controller input events work
- [ ] Session recovers from suspend/resume
- [ ] Graceful degradation when features unavailable

### Release Criteria

- [ ] All P0 tests pass on Quest 3
- [ ] All P0 tests pass on Quest 2
- [ ] Memory leak tests pass (4-hour soak)
- [ ] No ANRs in 1000 session starts
- [ ] Documentation complete
- [ ] Example app demonstrates all features

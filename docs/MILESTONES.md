# Kimoyo Oju Milestone Plan

## Overview

This plan builds stability first, adding features incrementally. Each milestone has clear exit criteria focused on crash-free operation.

---

## M1: Flat Hello Triangle via Nitro (4-6 weeks)

### Goal
Render a single colored triangle on Android and iOS using Nitro bindings, proving the core pipeline works end-to-end.

### Deliverables

1. **C++ Engine Core**
   - [ ] Engine singleton with render thread
   - [ ] Basic Renderer with state machine (CREATED → RUNNING → PAUSED → DESTROYED)
   - [ ] OpenGL ES 3.0 graphics device (Android)
   - [ ] Metal graphics device stub (iOS)
   - [ ] Handle registry with generation counters
   - [ ] Memory counters for leak detection

2. **Nitro Module**
   - [ ] HybridObject: KimoyoOjuEngine
   - [ ] Basic submit() with empty command buffer
   - [ ] getState() / getMemoryCounters()
   - [ ] pause() / resume()

3. **Platform Integration**
   - [ ] Android: SurfaceView with JNI bridge
   - [ ] Android: Lifecycle callbacks (onCreate, onPause, onResume, onDestroy)
   - [ ] iOS: UIView with CAMetalLayer
   - [ ] iOS: Lifecycle callbacks

4. **React Components**
   - [ ] `<KimoyoOjuView mode="flat">` - renders solid color
   - [ ] KimoyoOjuProvider context

5. **Tests**
   - [ ] Unit tests for HandleRegistry
   - [ ] Unit tests for state machine transitions
   - [ ] Basic soak test: pause/resume 100x
   - [ ] Memory leak check after teardown

### Exit Criteria
- [ ] Triangle renders on Android emulator
- [ ] Triangle renders on iOS simulator
- [ ] No crashes during 100 pause/resume cycles
- [ ] Memory counters return to zero after Engine destruction
- [ ] ASAN build passes all tests

---

## M2: Scene Graph + Command Batching (4-6 weeks)

### Goal
Full scene graph with batched command buffer updates. Multiple primitives rendering with transforms.

### Deliverables

1. **Scene Graph**
   - [ ] SceneNode base class with RAII
   - [ ] MeshNode, LightNode, CameraNode
   - [ ] Parent-child hierarchy (weak parent, strong children)
   - [ ] World transform calculation
   - [ ] Dirty flag propagation

2. **Command Buffer System**
   - [ ] Full command types (CREATE, DESTROY, SET_TRANSFORM, etc.)
   - [ ] CommandBufferValidator with NaN/Infinity rejection
   - [ ] Render thread command queue with mutex
   - [ ] Command execution on render thread

3. **Rendering**
   - [ ] Basic shader for colored geometry
   - [ ] Box, Sphere, Plane primitives
   - [ ] Ambient + directional lighting
   - [ ] Camera with configurable FOV

4. **React Components**
   - [ ] `<KimoyoOjuScene>`
   - [ ] `<KimoyoOjuNode>` with transform props
   - [ ] `<KimoyoOjuBox>`, `<KimoyoOjuSphere>`
   - [ ] `<KimoyoOjuLight type="ambient|directional">`
   - [ ] `<KimoyoOjuCamera>`
   - [ ] Automatic command batching per React commit

5. **Tests**
   - [ ] Command buffer validation tests
   - [ ] Scene graph parent/child tests
   - [ ] Soak test: create/destroy 1000 nodes 100x
   - [ ] Performance: measure submit latency

### Exit Criteria
- [ ] Scene with 100 nodes renders at 60fps
- [ ] No memory growth after 10000 node create/destroy cycles
- [ ] All command validation edge cases covered
- [ ] Works on Android phone + iOS phone

---

## M3: OpenXR Enter/Exit (6-8 weeks)

### Goal
Successfully enter and exit VR mode on Meta Quest without crashes or leaks.

### Deliverables

1. **OpenXR Integration**
   - [ ] XrInstance creation with Meta extensions
   - [ ] XrSession lifecycle state machine
   - [ ] Reference space creation (LOCAL, VIEW)
   - [ ] Swapchain creation and management
   - [ ] RAII wrappers for all XR handles

2. **XR Frame Loop**
   - [ ] xrWaitFrame / xrBeginFrame / xrEndFrame
   - [ ] View location for stereo rendering
   - [ ] Swapchain image acquire/wait/release
   - [ ] Projection layer submission

3. **Session State Handling**
   - [ ] IDLE → READY → SYNCHRONIZED → VISIBLE → FOCUSED
   - [ ] STOPPING → session end
   - [ ] LOSS_PENDING → graceful recovery
   - [ ] Guardian boundary handling

4. **JS API**
   - [ ] enterXR('immersive-vr')
   - [ ] exitXR()
   - [ ] XR session events (started, ended, lost)

5. **Tests**
   - [ ] Soak test: enterXR/exitXR 500x
   - [ ] Session loss simulation
   - [ ] Swapchain timeout handling
   - [ ] Memory check: no leaks after XR exit

### Exit Criteria
- [ ] VR mode works on Quest 2/3/Pro
- [ ] Clean exit from VR (no black screen, no hang)
- [ ] No crashes during 500 enter/exit cycles
- [ ] Handles guardian boundary gracefully
- [ ] Handles headset remove/replace

---

## M4: Assets & Models (4-6 weeks)

### Goal
Load and render 3D models (glTF) and textures from files and network.

### Deliverables

1. **Asset Pipeline**
   - [ ] Worker thread pool for decode
   - [ ] glTF 2.0 parser (embedded + external buffers)
   - [ ] Texture decode (PNG, JPEG, basis)
   - [ ] GPU upload on render thread only

2. **Model Rendering**
   - [ ] PBR shader (metallic-roughness)
   - [ ] Normal mapping
   - [ ] Skinned mesh animation
   - [ ] Multiple meshes per model

3. **Texture Management**
   - [ ] Texture registry with handles
   - [ ] Mipmap generation
   - [ ] Compressed texture support
   - [ ] Async load with placeholder

4. **React Components**
   - [ ] `<KimoyoOjuModel source={{uri: '...'}}>` 
   - [ ] `<KimoyoOjuImage source={{uri: '...'}}>`
   - [ ] Loading states and error callbacks
   - [ ] `<KimoyoOjuAnimatedModel>` for skeletal animation

5. **Tests**
   - [ ] Load 100 models sequentially
   - [ ] Load 10 models in parallel
   - [ ] Cancel in-flight loads
   - [ ] Memory tracking for textures

### Exit Criteria
- [ ] glTF 2.0 sample models render correctly
- [ ] Textures load from file and network
- [ ] No memory leaks after model unload
- [ ] Animation plays smoothly

---

## M5: Meta Quest Extensions (4-6 weeks)

### Goal
Leverage Quest-specific features for best-in-class XR experience.

### Deliverables

1. **Passthrough (MR Mode)**
   - [ ] XR_FB_passthrough extension
   - [ ] Passthrough layer composition
   - [ ] enterXR('immersive-mr')

2. **Foveated Rendering**
   - [ ] XR_FB_foveation extension
   - [ ] Fixed foveated rendering
   - [ ] Dynamic foveation (if eye tracking available)

3. **Hand Tracking**
   - [ ] XR_EXT_hand_tracking extension
   - [ ] Hand skeleton data to JS
   - [ ] `<KimoyoOjuHand>` component

4. **Controller Input**
   - [ ] XR_EXT_input_actions
   - [ ] Controller pose and button state
   - [ ] Haptic feedback

5. **Spatial Anchors** (stretch)
   - [ ] XR_FB_spatial_entity
   - [ ] Anchor persistence

### Exit Criteria
- [ ] Passthrough works on Quest 3
- [ ] Foveated rendering reduces GPU load by 20%+
- [ ] Hand tracking provides usable joint positions
- [ ] Controllers work with standard input handling

---

## Timeline Summary

| Milestone | Duration | Cumulative |
|-----------|----------|------------|
| M1: Hello Triangle | 4-6 weeks | 4-6 weeks |
| M2: Scene Graph | 4-6 weeks | 8-12 weeks |
| M3: OpenXR | 6-8 weeks | 14-20 weeks |
| M4: Assets | 4-6 weeks | 18-26 weeks |
| M5: Meta Extensions | 4-6 weeks | 22-32 weeks |

**Total: ~6-8 months to full feature parity**

---

## Risk Mitigation

### High-Risk Areas

1. **OpenXR Session Loss**
   - Mitigation: Extensive soak testing, defensive coding
   - Fallback: Graceful degradation to flat mode

2. **Memory Leaks in Native Code**
   - Mitigation: RAII everywhere, generation counters, ASAN in CI
   - Fallback: Memory tracking alerts before OOM

3. **Thread Safety Issues**
   - Mitigation: Clear thread ownership, no shared mutable state
   - Fallback: Mutex protection where needed

4. **Platform Fragmentation**
   - Mitigation: Abstract graphics API, test on multiple devices
   - Fallback: Feature detection and graceful fallback

### Dependencies

- React Native >= 0.73 (for Nitro support)
- Meta OpenXR SDK (for Quest extensions)
- Khronos OpenXR Loader
- GLM (math library)
- cgltf (glTF parsing)
- stb_image (texture decode)

---

## Success Metrics

### Stability
- Zero crashes in 1000 enter/exit cycles
- Zero memory leaks after full teardown
- < 1% ANR rate in production

### Performance
- 72fps sustained on Quest 2
- 90fps sustained on Quest 3
- < 100ms cold start to first frame
- < 16ms command buffer processing

### Developer Experience
- API similar to KimoyoOjuReact (easy migration)
- Clear error messages with actionable info
- Comprehensive documentation

# Kimoyo Oju Platform Backends Architecture

## Overview

Kimoyo Oju uses a **multi-backend architecture** that selects the optimal rendering and XR backend for each platform automatically.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         React Components Layer                               │
│    KimoyoOjuView, KimoyoOjuScene, KimoyoOjuNode, KimoyoOjuBox, etc.         │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                          Nitro HybridObjects
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Platform Abstraction Layer                              │
│                                                                              │
│   ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────────────────┐│
│   │ IRenderBackend  │  │  IXRBackend     │  │     IAssetLoader            ││
│   └─────────────────┘  └─────────────────┘  └─────────────────────────────┘│
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
        ┌───────────────────────────┼───────────────────────────┐
        │                           │                           │
        ▼                           ▼                           ▼
┌───────────────────┐    ┌───────────────────┐    ┌───────────────────────────┐
│   Apple Platforms │    │  Android Phones   │    │     XR Platforms          │
│                   │    │                   │    │                           │
│ ┌───────────────┐ │    │ ┌───────────────┐ │    │ ┌───────────────────────┐ │
│ │  RealityKit   │ │    │ │    Vulkan     │ │    │ │   Meta Quest (OpenXR) │ │
│ │  (Swift)      │ │    │ │               │ │    │ │   - OpenGL ES         │ │
│ └───────────────┘ │    │ └───────────────┘ │    │ │   - OpenXR Runtime    │ │
│        │          │    │        │          │    │ └───────────────────────┘ │
│        ▼          │    │        ▼          │    │                           │
│ ┌───────────────┐ │    │ ┌───────────────┐ │    │ ┌───────────────────────┐ │
│ │    ARKit      │ │    │ │    ARCore     │ │    │ │   Android XR (OpenXR) │ │
│ │  - iOS/iPadOS │ │    │ │               │ │    │ │   - Vulkan            │ │
│ │  - visionOS   │ │    │ └───────────────┘ │    │ │   - OpenXR Runtime    │ │
│ └───────────────┘ │    │                   │    │ └───────────────────────┘ │
│                   │    │                   │    │                           │
│ ┌───────────────┐ │    │                   │    │ ┌───────────────────────┐ │
│ │  Metal        │ │    │                   │    │ │   Apple Vision Pro   │ │
│ │  (Fallback)   │ │    │                   │    │ │   - RealityKit        │ │
│ └───────────────┘ │    │                   │    │ │   - ARKit (Spatial)   │ │
└───────────────────┘    └───────────────────┘    └───────────────────────────┘
```

---

## Platform Matrix

| Platform | Render Backend | XR Backend | Language | Status |
|----------|---------------|------------|----------|--------|
| **iOS/iPadOS** | RealityKit | ARKit | Swift | 🔄 Planned |
| **iOS/iPadOS (Legacy)** | Metal | ARKit | C++/ObjC++ | ✅ Exists |
| **visionOS** | RealityKit | ARKit (Spatial) | Swift | 🔄 Planned |
| **Android Phones** | Vulkan | ARCore | C++ | 🔄 Planned |
| **Android Phones (Legacy)** | OpenGL ES | ARCore | C++ | ✅ Exists |
| **Meta Quest** | OpenGL ES | OpenXR | C++ | ✅ Exists |
| **Android XR** | Vulkan | OpenXR | C++ | 🔄 Planned |
| **Windows PC VR** | D3D11 | OpenXR | C++ | 🔄 Planned |
| **Linux PC VR** | Vulkan | OpenXR | C++ | 🔄 Planned |

---

## 1. Apple Platforms (RealityKit + ARKit)

### 1.1 Architecture

```swift
// Swift Nitro HybridObject
┌─────────────────────────────────────────────────────────────┐
│              HybridKimoyoOjuRealityEngine (Swift)           │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │   ARView    │  │   Scene     │  │  Entity Registry    │ │
│  │             │  │ (RealityKit)│  │  (Handle → Entity)  │ │
│  └─────────────┘  └─────────────┘  └─────────────────────┘ │
│                                                             │
│  Methods:                                                   │
│  - createEntity(type, name) → Handle                        │
│  - destroyEntity(handle)                                    │
│  - setTransform(handle, position, rotation, scale)          │
│  - setMaterial(handle, pbr)                                 │
│  - loadModel(url) async → Handle                            │
│  - enableAR(config)                                         │
│  - getHandTracking() → HandData                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 RealityKit Backend Implementation

```swift
// packages/react-native-kimoyo-oju/ios/KimoyoOjuRealityEngine.swift

import NitroModules
import RealityKit
import ARKit
import Combine

/// Handle for tracking entities across JS/Native boundary
public struct EntityHandle: Hashable, Codable {
    public let id: UInt64
    public let generation: UInt32
}

/// Main RealityKit engine - Nitro HybridObject
public class HybridKimoyoOjuRealityEngine: HybridKimoyoOjuEngineSpec {
    
    // MARK: - Properties
    private var arView: ARView?
    private var rootAnchor: AnchorEntity
    private var entityRegistry: [EntityHandle: Entity] = [:]
    private var nextId: UInt64 = 1
    private var cancellables = Set<AnyCancellable>()
    
    // MARK: - Initialization
    public init() {
        rootAnchor = AnchorEntity(world: .zero)
    }
    
    // MARK: - View Management
    public func attachView(_ view: ARView) {
        self.arView = view
        view.scene.addAnchor(rootAnchor)
        
        // Configure default lighting
        view.environment.lighting.intensityExponent = 1.0
    }
    
    public func detachView() {
        arView?.scene.anchors.removeAll()
        arView = nil
    }
    
    // MARK: - Entity Creation
    public func createEntity(type: String, name: String) -> EntityHandle {
        let handle = allocateHandle()
        
        let entity: Entity
        switch type {
        case "box":
            let mesh = MeshResource.generateBox(size: 1.0)
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "sphere":
            let mesh = MeshResource.generateSphere(radius: 0.5)
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "plane":
            let mesh = MeshResource.generatePlane(width: 1, depth: 1)
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "text":
            let mesh = MeshResource.generateText(
                name,
                extrusionDepth: 0.01,
                font: .systemFont(ofSize: 0.1)
            )
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "group":
            entity = Entity()
            
        case "light":
            entity = Entity()
            let light = PointLight()
            light.light.intensity = 1000
            entity.components.set(light)
            
        default:
            entity = Entity()
        }
        
        entity.name = name
        rootAnchor.addChild(entity)
        entityRegistry[handle] = entity
        
        return handle
    }
    
    public func destroyEntity(handle: EntityHandle) -> Bool {
        guard let entity = entityRegistry[handle] else { return false }
        entity.removeFromParent()
        entityRegistry.removeValue(forKey: handle)
        return true
    }
    
    // MARK: - Transforms
    public func setTransform(
        handle: EntityHandle,
        position: SIMD3<Float>,
        rotation: simd_quatf,
        scale: SIMD3<Float>
    ) {
        guard let entity = entityRegistry[handle] else { return }
        entity.position = position
        entity.orientation = rotation
        entity.scale = scale
    }
    
    // MARK: - Materials (PBR)
    public func setMaterial(
        handle: EntityHandle,
        baseColor: SIMD4<Float>,
        metallic: Float,
        roughness: Float,
        emissive: SIMD3<Float>?
    ) {
        guard let entity = entityRegistry[handle] as? ModelEntity else { return }
        
        var material = PhysicallyBasedMaterial()
        material.baseColor = .init(tint: UIColor(
            red: CGFloat(baseColor.x),
            green: CGFloat(baseColor.y),
            blue: CGFloat(baseColor.z),
            alpha: CGFloat(baseColor.w)
        ))
        material.metallic = .init(floatLiteral: metallic)
        material.roughness = .init(floatLiteral: roughness)
        
        if let emissive = emissive {
            material.emissiveColor = .init(color: UIColor(
                red: CGFloat(emissive.x),
                green: CGFloat(emissive.y),
                blue: CGFloat(emissive.z),
                alpha: 1.0
            ))
        }
        
        entity.model?.materials = [material]
    }
    
    // MARK: - Asset Loading
    public func loadModel(url: String) async throws -> EntityHandle {
        guard let modelURL = URL(string: url) else {
            throw KimoyoOjuError.invalidURL
        }
        
        let entity = try await Entity(contentsOf: modelURL)
        let handle = allocateHandle()
        
        rootAnchor.addChild(entity)
        entityRegistry[handle] = entity
        
        return handle
    }
    
    // MARK: - AR Configuration
    public func enableARWorldTracking(planeDetection: Bool, sceneReconstruction: Bool) {
        guard let arView = arView else { return }
        
        let config = ARWorldTrackingConfiguration()
        
        if planeDetection {
            config.planeDetection = [.horizontal, .vertical]
        }
        
        if sceneReconstruction, ARWorldTrackingConfiguration.supportsSceneReconstruction(.mesh) {
            config.sceneReconstruction = .mesh
        }
        
        arView.session.run(config)
    }
    
    // MARK: - visionOS Immersive Space
    #if os(visionOS)
    public func enterImmersiveSpace() async throws {
        // visionOS specific: Request immersive space
        // This would be called from SwiftUI app
    }
    
    public func getHandTracking() -> HandTrackingData? {
        guard let arView = arView else { return nil }
        // Access ARKit hand tracking on visionOS
        return nil // Placeholder
    }
    #endif
    
    // MARK: - Private Helpers
    private func allocateHandle() -> EntityHandle {
        let id = nextId
        nextId += 1
        return EntityHandle(id: id, generation: 0)
    }
}

enum KimoyoOjuError: Error {
    case invalidURL
    case entityNotFound
    case loadFailed(String)
}
```

### 1.3 visionOS Specific Features

```swift
// packages/react-native-kimoyo-oju/visionos/KimoyoOjuVisionOSExtensions.swift

#if os(visionOS)
import SwiftUI
import RealityKit

/// visionOS Window/Volume/Immersive Space management
public class KimoyoOjuVisionOSManager {
    
    public enum SpaceType {
        case window      // Standard window
        case volume      // 3D bounded volume
        case immersive   // Full immersive space (passthrough or VR)
    }
    
    public struct ImmersiveConfig {
        var style: ImmersiveSpaceStyle = .mixed
        var upperLimbVisibility: UpperLimbVisibility = .visible
    }
    
    /// Request transition to immersive space
    public func requestImmersiveSpace(config: ImmersiveConfig) async throws {
        // SwiftUI environment action to open immersive space
    }
    
    /// Exit immersive space
    public func dismissImmersiveSpace() async {
        // SwiftUI environment action to dismiss
    }
    
    /// Configure hand tracking
    public func enableHandTracking() async throws -> HandTrackingProvider {
        let session = ARKitSession()
        let handTracking = HandTrackingProvider()
        try await session.run([handTracking])
        return handTracking
    }
    
    /// Get spatial anchors
    public func createWorldAnchor(transform: simd_float4x4) async throws -> WorldAnchor {
        // Create persistent world anchor
        fatalError("Not implemented")
    }
}

/// SwiftUI Immersive Space View
public struct KimoyoOjuImmersiveView: View {
    let engine: HybridKimoyoOjuRealityEngine
    
    public var body: some View {
        RealityView { content in
            // Add root anchor to reality view
        }
        .gesture(SpatialTapGesture().targetedToAnyEntity().onEnded { event in
            // Handle spatial tap
        })
    }
}
#endif
```

---

## 2. Android Platforms (Vulkan + ARCore)

### 2.1 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│              KimoyoOjuVulkanEngine (C++)                    │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │VkInstance   │  │VkDevice     │  │  Scene Graph        │ │
│  │VkSurface    │  │VkSwapchain  │  │  (C++ ECS)          │ │
│  └─────────────┘  └─────────────┘  └─────────────────────┘ │
│                                                             │
│  ┌─────────────────────────────────────────────────────────┐│
│  │                  ARCore Session                          ││
│  │  - Plane Detection                                       ││
│  │  - Light Estimation                                      ││
│  │  - Depth API                                             ││
│  │  - Cloud Anchors                                         ││
│  └─────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

### 2.2 Vulkan Backend Header

```cpp
// packages/kimoyo-oju-engine/cpp/include/kimoyooju/backends/VulkanBackend.hpp

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "kimoyooju/IRenderBackend.hpp"

namespace kimoyooju {

struct VulkanConfig {
    bool enableValidation = true;
    bool enableRayTracing = false;
    uint32_t maxFramesInFlight = 2;
};

class VulkanBackend : public IRenderBackend {
public:
    explicit VulkanBackend(const VulkanConfig& config);
    ~VulkanBackend() override;
    
    // IRenderBackend interface
    bool initialize(void* nativeWindow, int width, int height) override;
    void shutdown() override;
    void resize(int width, int height) override;
    
    void beginFrame() override;
    void endFrame() override;
    
    Handle createMesh(const MeshData& data) override;
    void destroyMesh(Handle handle) override;
    
    Handle createTexture(const TextureData& data) override;
    void destroyTexture(Handle handle) override;
    
    Handle createMaterial(const MaterialData& data) override;
    void destroyMaterial(Handle handle) override;
    
    void renderScene(const SceneGraph& scene, const Camera& camera) override;
    
    // Vulkan-specific
    VkInstance getInstance() const { return instance_; }
    VkDevice getDevice() const { return device_; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice_; }
    
private:
    void createInstance();
    void selectPhysicalDevice();
    void createLogicalDevice();
    void createSwapchain();
    void createRenderPass();
    void createGraphicsPipeline();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    
    void recreateSwapchain();
    void cleanupSwapchain();
    
    VulkanConfig config_;
    
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline_ = VK_NULL_HANDLE;
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
    
    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainImageViews_;
    std::vector<VkFramebuffer> framebuffers_;
    std::vector<VkCommandBuffer> commandBuffers_;
    
    std::vector<VkSemaphore> imageAvailableSemaphores_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;
    std::vector<VkFence> inFlightFences_;
    
    uint32_t currentFrame_ = 0;
    int width_ = 0;
    int height_ = 0;
    bool framebufferResized_ = false;
};

} // namespace kimoyooju
```

### 2.3 ARCore Integration

```cpp
// packages/kimoyo-oju-engine/cpp/include/kimoyooju/backends/ARCoreBackend.hpp

#pragma once

#include <arcore_c_api.h>
#include "kimoyooju/IXRBackend.hpp"

namespace kimoyooju {

struct ARCoreConfig {
    bool enablePlaneDetection = true;
    bool enableLightEstimation = true;
    bool enableDepthAPI = false;
    bool enableCloudAnchors = false;
};

class ARCoreBackend : public IXRBackend {
public:
    explicit ARCoreBackend(const ARCoreConfig& config);
    ~ARCoreBackend() override;
    
    // IXRBackend interface
    bool initialize(void* activity, void* context) override;
    void shutdown() override;
    
    bool resume() override;
    void pause() override;
    
    bool beginFrame(XRFrameData& outFrameData) override;
    void endFrame() override;
    
    XRTrackingState getTrackingState() const override;
    std::vector<XRPlane> getDetectedPlanes() const override;
    XRLightEstimate getLightEstimate() const override;
    
    // Anchors
    Handle createAnchor(const Pose& pose) override;
    bool destroyAnchor(Handle handle) override;
    Pose getAnchorPose(Handle handle) const override;
    
    // Hit testing
    std::vector<XRHitResult> hitTest(float screenX, float screenY) const override;
    
    // Depth
    bool getDepthImage(DepthImage& outDepth) const override;
    
private:
    void updatePlanes();
    void updateLightEstimate();
    
    ARCoreConfig config_;
    
    ArSession* session_ = nullptr;
    ArFrame* frame_ = nullptr;
    ArCamera* camera_ = nullptr;
    
    std::vector<ArPlane*> trackedPlanes_;
    std::map<Handle, ArAnchor*> anchors_;
    
    XRLightEstimate lastLightEstimate_;
    XRTrackingState trackingState_ = XRTrackingState::NotAvailable;
};

} // namespace kimoyooju
```

---

## 3. Android XR (OpenXR + Vulkan)

### 3.1 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│              KimoyoOjuAndroidXREngine (C++)                 │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐ │
│  │                    OpenXR Runtime                      │ │
│  │                                                        │ │
│  │  XrInstance ──► XrSession ──► XrSwapchain             │ │
│  │                                                        │ │
│  │  Extensions:                                           │ │
│  │  - XR_KHR_vulkan_enable2                              │ │
│  │  - XR_ANDROID_session_state_enable (Android XR)       │ │
│  │  - XR_FB_passthrough (if available)                   │ │
│  │  - XR_EXT_hand_tracking                               │ │
│  └───────────────────────────────────────────────────────┘ │
│                           │                                 │
│                           ▼                                 │
│  ┌───────────────────────────────────────────────────────┐ │
│  │                   Vulkan Renderer                      │ │
│  │                                                        │ │
│  │  VkInstance (from OpenXR) ──► VkDevice ──► Render     │ │
│  └───────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Android XR Backend

```cpp
// packages/kimoyo-oju-engine/cpp/include/kimoyooju/backends/AndroidXRBackend.hpp

#pragma once

#include <openxr/openxr.h>
#include <vulkan/vulkan.h>
#include "kimoyooju/IXRBackend.hpp"

namespace kimoyooju {

struct AndroidXRConfig {
    bool enableHandTracking = true;
    bool enablePassthrough = true;
    bool enableSceneUnderstanding = false;
    XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrViewConfigurationType viewConfig = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
};

class AndroidXRBackend : public IXRBackend {
public:
    explicit AndroidXRBackend(const AndroidXRConfig& config);
    ~AndroidXRBackend() override;
    
    // IXRBackend interface
    bool initialize(void* activity, void* context) override;
    void shutdown() override;
    
    bool resume() override;
    void pause() override;
    
    bool beginFrame(XRFrameData& outFrameData) override;
    void endFrame() override;
    
    XRTrackingState getTrackingState() const override;
    
    // OpenXR specific
    XrInstance getInstance() const { return instance_; }
    XrSession getSession() const { return session_; }
    XrSpace getReferenceSpace() const { return localSpace_; }
    
    // Vulkan integration
    VkInstance getVulkanInstance() const { return vkInstance_; }
    VkPhysicalDevice getVulkanPhysicalDevice() const { return vkPhysicalDevice_; }
    VkDevice getVulkanDevice() const { return vkDevice_; }
    
    // Hand tracking
    bool isHandTrackingSupported() const;
    HandJointData getHandJoints(Hand hand) const;
    
    // Passthrough
    bool enablePassthrough();
    void disablePassthrough();
    
    // Controller input
    XRControllerState getControllerState(Hand hand) const;
    
private:
    void createInstance();
    void getSystemId();
    void createSession();
    void createReferenceSpaces();
    void createSwapchains();
    void initHandTracking();
    void initPassthrough();
    
    void handleSessionStateChange(XrSessionState newState);
    void pollEvents();
    
    AndroidXRConfig config_;
    
    // OpenXR handles
    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId systemId_ = XR_NULL_SYSTEM_ID;
    XrSession session_ = XR_NULL_HANDLE;
    XrSpace localSpace_ = XR_NULL_HANDLE;
    XrSpace viewSpace_ = XR_NULL_HANDLE;
    
    std::vector<XrSwapchain> swapchains_;
    std::vector<XrSwapchainImageVulkanKHR> swapchainImages_;
    
    // Vulkan handles (created via OpenXR)
    VkInstance vkInstance_ = VK_NULL_HANDLE;
    VkPhysicalDevice vkPhysicalDevice_ = VK_NULL_HANDLE;
    VkDevice vkDevice_ = VK_NULL_HANDLE;
    
    // Hand tracking
    XrHandTrackerEXT handTrackers_[2] = {XR_NULL_HANDLE, XR_NULL_HANDLE};
    
    // Passthrough
    XrPassthroughFB passthrough_ = XR_NULL_HANDLE;
    XrPassthroughLayerFB passthroughLayer_ = XR_NULL_HANDLE;
    
    // State
    XrSessionState sessionState_ = XR_SESSION_STATE_UNKNOWN;
    bool sessionRunning_ = false;
};

} // namespace kimoyooju
```

---

## 4. Backend Abstraction Interfaces

### 4.1 Render Backend Interface

```cpp
// packages/kimoyo-oju-engine/cpp/include/kimoyooju/IRenderBackend.hpp

#pragma once

#include "kimoyooju/Types.hpp"
#include <memory>

namespace kimoyooju {

/// Abstract render backend interface
class IRenderBackend {
public:
    virtual ~IRenderBackend() = default;
    
    // Lifecycle
    virtual bool initialize(void* nativeWindow, int width, int height) = 0;
    virtual void shutdown() = 0;
    virtual void resize(int width, int height) = 0;
    
    // Frame
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    
    // Resources
    virtual Handle createMesh(const MeshData& data) = 0;
    virtual void destroyMesh(Handle handle) = 0;
    
    virtual Handle createTexture(const TextureData& data) = 0;
    virtual void destroyTexture(Handle handle) = 0;
    
    virtual Handle createMaterial(const MaterialData& data) = 0;
    virtual void destroyMaterial(Handle handle) = 0;
    
    // Rendering
    virtual void renderScene(const SceneGraph& scene, const Camera& camera) = 0;
    
    // Capabilities
    virtual RenderCapabilities getCapabilities() const = 0;
};

struct RenderCapabilities {
    bool supportsRayTracing = false;
    bool supportsCompute = false;
    bool supportsPBR = true;
    uint32_t maxTextureSize = 4096;
    uint32_t maxVertices = 1000000;
};

/// Factory to create appropriate backend for platform
std::unique_ptr<IRenderBackend> createRenderBackend(Platform platform);

} // namespace kimoyooju
```

### 4.2 XR Backend Interface

```cpp
// packages/kimoyo-oju-engine/cpp/include/kimoyooju/IXRBackend.hpp

#pragma once

#include "kimoyooju/Types.hpp"
#include <vector>

namespace kimoyooju {

/// XR frame data passed to renderer
struct XRFrameData {
    bool shouldRender = false;
    int64_t predictedDisplayTime = 0;
    std::vector<XRView> views;
    Pose headPose;
};

struct XRView {
    Pose pose;
    XRFov fov;
    uint32_t swapchainIndex;
};

struct XRFov {
    float angleLeft;
    float angleRight;
    float angleUp;
    float angleDown;
};

enum class XRTrackingState {
    NotAvailable,
    Limited,
    Normal
};

/// Abstract XR backend interface
class IXRBackend {
public:
    virtual ~IXRBackend() = default;
    
    // Lifecycle
    virtual bool initialize(void* platformData1, void* platformData2) = 0;
    virtual void shutdown() = 0;
    virtual bool resume() = 0;
    virtual void pause() = 0;
    
    // Frame
    virtual bool beginFrame(XRFrameData& outFrameData) = 0;
    virtual void endFrame() = 0;
    
    // Tracking
    virtual XRTrackingState getTrackingState() const = 0;
    
    // Planes (AR)
    virtual std::vector<XRPlane> getDetectedPlanes() const { return {}; }
    
    // Light estimation (AR)
    virtual XRLightEstimate getLightEstimate() const { return {}; }
    
    // Anchors
    virtual Handle createAnchor(const Pose& pose) { return {}; }
    virtual bool destroyAnchor(Handle handle) { return false; }
    virtual Pose getAnchorPose(Handle handle) const { return {}; }
    
    // Hit testing
    virtual std::vector<XRHitResult> hitTest(float x, float y) const { return {}; }
    
    // Depth
    virtual bool getDepthImage(DepthImage& outDepth) const { return false; }
    
    // Capabilities
    virtual XRCapabilities getCapabilities() const = 0;
};

struct XRCapabilities {
    bool supportsHandTracking = false;
    bool supportsPlaneDetection = false;
    bool supportsSceneUnderstanding = false;
    bool supportsPassthrough = false;
    bool supportsDepth = false;
    bool supportsCloudAnchors = false;
};

/// Factory to create appropriate XR backend for platform
std::unique_ptr<IXRBackend> createXRBackend(Platform platform, XRMode mode);

} // namespace kimoyooju
```

---

## 5. Platform Detection & Backend Selection

```cpp
// packages/kimoyo-oju-engine/cpp/src/PlatformBackendFactory.cpp

#include "kimoyooju/IRenderBackend.hpp"
#include "kimoyooju/IXRBackend.hpp"

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_VISION
        #define KIMOYOOJU_PLATFORM_VISIONOS 1
    #elif TARGET_OS_IOS
        #define KIMOYOOJU_PLATFORM_IOS 1
    #endif
#elif defined(__ANDROID__)
    #define KIMOYOOJU_PLATFORM_ANDROID 1
#endif

namespace kimoyooju {

std::unique_ptr<IRenderBackend> createRenderBackend(Platform platform) {
    switch (platform) {
#if KIMOYOOJU_PLATFORM_VISIONOS || KIMOYOOJU_PLATFORM_IOS
        case Platform::Apple:
            // RealityKit backend is Swift - bridged via ObjC++
            return createRealityKitBackend();
#endif
            
#if KIMOYOOJU_PLATFORM_ANDROID
        case Platform::Android:
            // Check for Vulkan support
            if (isVulkanSupported()) {
                return std::make_unique<VulkanBackend>(VulkanConfig{});
            }
            // Fallback to OpenGL ES
            return std::make_unique<GLESBackend>(GLESConfig{});
#endif
            
        default:
            return nullptr;
    }
}

std::unique_ptr<IXRBackend> createXRBackend(Platform platform, XRMode mode) {
    switch (platform) {
#if KIMOYOOJU_PLATFORM_VISIONOS
        case Platform::VisionOS:
            return createRealityKitXRBackend();  // Uses ARKit spatial
#endif
            
#if KIMOYOOJU_PLATFORM_IOS
        case Platform::iOS:
            if (mode != XRMode::Flat) {
                return std::make_unique<ARKitBackend>(ARKitConfig{});
            }
            return nullptr;
#endif
            
#if KIMOYOOJU_PLATFORM_ANDROID
        case Platform::Android:
            if (isAndroidXRDevice()) {
                // Android XR headset (glasses, etc.)
                return std::make_unique<AndroidXRBackend>(AndroidXRConfig{});
            }
            if (isQuestDevice()) {
                // Meta Quest via OpenXR
                return std::make_unique<OpenXRBackend>(OpenXRConfig{
                    .graphicsAPI = GraphicsAPI::OpenGLES
                });
            }
            if (mode != XRMode::Flat) {
                // Android phone AR via ARCore
                return std::make_unique<ARCoreBackend>(ARCoreConfig{});
            }
            return nullptr;
#endif
            
        default:
            return nullptr;
    }
}

} // namespace kimoyooju
```

---

## 6. Build Configuration

### 6.1 CMakeLists.txt Updates

```cmake
# packages/kimoyo-oju-engine/cpp/CMakeLists.txt

cmake_minimum_required(VERSION 3.18)
project(KimoyoOjuEngine)

# Platform detection
if(APPLE)
    if(CMAKE_SYSTEM_NAME STREQUAL "visionOS")
        set(KIMOYOOJU_PLATFORM_VISIONOS ON)
        set(KIMOYOOJU_BACKEND_REALITYKIT ON)
    else()
        set(KIMOYOOJU_PLATFORM_IOS ON)
        set(KIMOYOOJU_BACKEND_REALITYKIT ON)
        set(KIMOYOOJU_BACKEND_METAL ON)  # Fallback
    endif()
elseif(ANDROID)
    set(KIMOYOOJU_PLATFORM_ANDROID ON)
    set(KIMOYOOJU_BACKEND_VULKAN ON)
    set(KIMOYOOJU_BACKEND_GLES ON)  # Fallback
    set(KIMOYOOJU_XR_ARCORE ON)
    set(KIMOYOOJU_XR_OPENXR ON)
endif()

# Sources
set(CORE_SOURCES
    src/Engine.cpp
    src/SceneGraph.cpp
    src/CommandBuffer.cpp
    src/Renderer.cpp
)

# Backend sources
if(KIMOYOOJU_BACKEND_VULKAN)
    list(APPEND BACKEND_SOURCES
        src/backends/VulkanBackend.cpp
        src/backends/VulkanPipeline.cpp
        src/backends/VulkanResources.cpp
    )
    find_package(Vulkan REQUIRED)
endif()

if(KIMOYOOJU_BACKEND_GLES)
    list(APPEND BACKEND_SOURCES
        src/backends/GLESBackend.cpp
    )
endif()

if(KIMOYOOJU_BACKEND_METAL)
    list(APPEND BACKEND_SOURCES
        src/backends/MetalBackend.mm
    )
endif()

# XR sources
if(KIMOYOOJU_XR_OPENXR)
    list(APPEND XR_SOURCES
        src/xr/OpenXRBackend.cpp
        src/xr/OpenXRSwapchain.cpp
    )
endif()

if(KIMOYOOJU_XR_ARCORE)
    list(APPEND XR_SOURCES
        src/xr/ARCoreBackend.cpp
    )
endif()

add_library(KimoyoOjuEngine STATIC
    ${CORE_SOURCES}
    ${BACKEND_SOURCES}
    ${XR_SOURCES}
)

# Link libraries
if(KIMOYOOJU_BACKEND_VULKAN)
    target_link_libraries(KimoyoOjuEngine PRIVATE Vulkan::Vulkan)
endif()

if(KIMOYOOJU_XR_ARCORE AND ANDROID)
    target_link_libraries(KimoyoOjuEngine PRIVATE arcore)
endif()

if(KIMOYOOJU_XR_OPENXR)
    target_link_libraries(KimoyoOjuEngine PRIVATE openxr_loader)
endif()
```

---

## 7. Updated README Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Kimoyo Oju Architecture                            │
└─────────────────────────────────────────────────────────────────────────────┘

                              React Components
    ┌──────────────────────────────────────────────────────────────────────┐
    │  KimoyoOjuView  │  KimoyoOjuScene  │  KimoyoOjuNode  │  KimoyoOjuBox │
    └──────────────────────────────────────────────────────────────────────┘
                                      │
                            Nitro HybridObjects
                                      │
    ┌─────────────────────────────────┴─────────────────────────────────────┐
    │                     Platform Abstraction Layer                         │
    │                                                                        │
    │    IRenderBackend                         IXRBackend                   │
    │    ├── RealityKitBackend (Swift)          ├── ARKitBackend            │
    │    ├── VulkanBackend (C++)                ├── ARCoreBackend           │
    │    ├── MetalBackend (ObjC++)              ├── OpenXRBackend           │
    │    └── GLESBackend (C++)                  └── AndroidXRBackend        │
    └────────────────────────────────────────────────────────────────────────┘
                                      │
         ┌────────────────────────────┼────────────────────────────┐
         │                            │                            │
         ▼                            ▼                            ▼
    ┌──────────┐               ┌──────────┐                ┌──────────────┐
    │  Apple   │               │ Android  │                │   XR HMDs    │
    │          │               │  Phones  │                │              │
    │ iOS      │               │          │                │ Meta Quest   │
    │ iPadOS   │               │ Vulkan   │                │ (OpenXR+GLES)│
    │ visionOS │               │ ARCore   │                │              │
    │          │               │          │                │ Android XR   │
    │RealityKit│               │          │                │ (OpenXR+Vk)  │
    │ ARKit    │               │          │                │              │
    └──────────┘               └──────────┘                │ Vision Pro   │
                                                           │ (RealityKit) │
                                                           └──────────────┘
```

#pragma once

#include "kimoyooju/Types.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace kimoyooju {

struct XRView {
    Pose pose;
    float fovLeft;
    float fovRight;
    float fovUp;
    float fovDown;
    uint32_t swapchainIndex;
    uint32_t width;
    uint32_t height;
};

struct XRFrameData {
    bool shouldRender = false;
    int64_t predictedDisplayTime = 0;
    std::vector<XRView> views;
    Pose headPose;
};

struct XRPlane {
    Handle handle;
    Pose pose;
    Vec2 extents;
    PlaneType type;
    TrackingState trackingState;
};

enum class PlaneType {
    HorizontalUp,
    HorizontalDown,
    Vertical,
    Unknown
};

enum class TrackingState {
    NotTracking,
    Limited,
    Tracking
};

struct XRLightEstimate {
    Vec3 direction;
    Vec3 intensity;
    float ambientIntensity;
    float ambientColorTemperature;
};

struct XRHitResult {
    Pose pose;
    float distance;
    Handle planeHandle;
    HitType type;
};

enum class HitType {
    Plane,
    FeaturePoint,
    Mesh
};

struct DepthImage {
    const float* data;
    uint32_t width;
    uint32_t height;
    float nearPlane;
    float farPlane;
};

struct HandJoint {
    Pose pose;
    float radius;
    bool isTracked;
};

struct HandData {
    std::vector<HandJoint> joints;
    bool isTracked;
    float confidence;
};

enum class Hand {
    Left,
    Right
};

struct XRControllerState {
    Pose gripPose;
    Pose aimPose;
    float triggerValue;
    float gripValue;
    Vec2 thumbstick;
    bool primaryButton;
    bool secondaryButton;
    bool isTracked;
};

struct XRCapabilities {
    bool supportsHandTracking = false;
    bool supportsPlaneDetection = false;
    bool supportsSceneUnderstanding = false;
    bool supportsPassthrough = false;
    bool supportsDepth = false;
    bool supportsCloudAnchors = false;
    bool supportsControllers = false;
    bool supportsEyeTracking = false;
};

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
    
    // Session
    virtual bool isSessionRunning() const = 0;
    virtual XRMode getCurrentMode() const = 0;
    
    // Tracking
    virtual TrackingState getTrackingState() const = 0;
    
    // Plane detection (AR)
    virtual std::vector<XRPlane> getDetectedPlanes() const { return {}; }
    virtual void setPlaneDetectionEnabled(bool enabled) {}
    
    // Light estimation (AR)
    virtual std::optional<XRLightEstimate> getLightEstimate() const { return std::nullopt; }
    
    // Anchors
    virtual Handle createAnchor(const Pose& pose) { return {}; }
    virtual bool destroyAnchor(Handle handle) { return false; }
    virtual std::optional<Pose> getAnchorPose(Handle handle) const { return std::nullopt; }
    
    // Hit testing
    virtual std::vector<XRHitResult> hitTest(float screenX, float screenY) const { return {}; }
    virtual std::vector<XRHitResult> hitTestRay(const Vec3& origin, const Vec3& direction) const { return {}; }
    
    // Depth
    virtual bool isDepthAvailable() const { return false; }
    virtual bool getDepthImage(DepthImage& outDepth) const { return false; }
    
    // Hand tracking
    virtual bool isHandTrackingSupported() const { return false; }
    virtual std::optional<HandData> getHandData(Hand hand) const { return std::nullopt; }
    
    // Controllers
    virtual bool areControllersSupported() const { return false; }
    virtual std::optional<XRControllerState> getControllerState(Hand hand) const { return std::nullopt; }
    
    // Passthrough
    virtual bool isPassthroughSupported() const { return false; }
    virtual bool enablePassthrough() { return false; }
    virtual void disablePassthrough() {}
    
    // Capabilities
    virtual XRCapabilities getCapabilities() const = 0;
};

std::unique_ptr<IXRBackend> createXRBackend(Platform platform, XRMode mode);

} // namespace kimoyooju

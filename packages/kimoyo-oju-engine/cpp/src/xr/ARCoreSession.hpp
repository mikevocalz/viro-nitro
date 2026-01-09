#pragma once

#include "../../include/kimoyooju/Types.hpp"
#include <memory>
#include <functional>

// Forward declarations for ARCore types
struct ArSession;
struct ArConfig;
struct ArFrame;
struct ArCamera;
struct ArPose;

namespace kimoyooju {

enum class ARTrackingState {
    NOT_TRACKING,
    TRACKING,
    PAUSED
};

enum class ARPlaneType {
    HORIZONTAL_UPWARD,
    HORIZONTAL_DOWNWARD,
    VERTICAL
};

struct ARPlane {
    uint64_t id;
    ARPlaneType type;
    Vec3 centerPose;
    Quat centerRotation;
    float extentX;
    float extentZ;
    bool isSubsumed;
};

struct ARHitResult {
    Vec3 position;
    Quat rotation;
    float distance;
    uint64_t planeId; // 0 if not hitting a plane
};

struct ARLightEstimate {
    float ambientIntensity;
    Color ambientColor;
    Vec3 mainLightDirection;
    float mainLightIntensity;
};

class ARCoreSession {
public:
    struct Config {
        bool enableDepth = false;
        bool enableLightEstimation = true;
        bool enablePlaneDetection = true;
        bool enableCloudAnchors = false;
    };

    using PlaneCallback = std::function<void(const ARPlane& plane, bool added)>;
    using TrackingCallback = std::function<void(ARTrackingState state)>;

    ARCoreSession();
    ~ARCoreSession();

    // Lifecycle
    bool initialize(void* javaContext, void* activity, const Config& config);
    void shutdown();
    bool resume();
    void pause();

    // Frame processing
    bool update();
    bool beginFrame();
    void endFrame();

    // Camera/Tracking
    ARTrackingState getTrackingState() const;
    void getCameraViewMatrix(float* outMatrix) const;
    void getCameraProjectionMatrix(float* outMatrix, float nearClip, float farClip) const;
    void getCameraTextureSize(uint32_t& width, uint32_t& height) const;
    uint32_t getCameraTextureId() const;

    // Planes
    void setPlaneCallback(PlaneCallback callback);
    void setTrackingCallback(TrackingCallback callback);
    std::vector<ARPlane> getPlanes() const;

    // Hit testing
    bool hitTest(float screenX, float screenY, std::vector<ARHitResult>& results) const;

    // Light estimation
    ARLightEstimate getLightEstimate() const;

    // Anchors
    uint64_t createAnchor(const Vec3& position, const Quat& rotation);
    bool getAnchorPose(uint64_t anchorId, Vec3& position, Quat& rotation) const;
    void removeAnchor(uint64_t anchorId);

    // Depth (if enabled)
    bool hasDepthImage() const;
    float getDepthAtPoint(float screenX, float screenY) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace kimoyooju

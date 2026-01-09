#pragma once

#include "../../include/kimoyooju/Types.hpp"
#include <memory>
#include <functional>
#include <vector>

#ifdef __APPLE__
#import <ARKit/ARKit.h>
#endif

namespace kimoyooju {

enum class ARKitTrackingState {
    NOT_AVAILABLE,
    LIMITED,
    NORMAL
};

enum class ARKitPlaneType {
    HORIZONTAL,
    VERTICAL
};

struct ARKitPlane {
    uint64_t identifier;
    ARKitPlaneType type;
    Vec3 center;
    Quat rotation;
    float width;
    float height;
};

struct ARKitHitResult {
    Vec3 position;
    Quat rotation;
    float distance;
    uint64_t planeIdentifier; // 0 if not a plane hit
};

struct ARKitLightEstimate {
    float ambientIntensity;
    float ambientColorTemperature;
};

class ARKitSession {
public:
    struct Config {
        bool enablePlaneDetection = true;
        bool enableLightEstimation = true;
        bool enableWorldTracking = true;
        bool enablePeopleOcclusion = false;
        bool enableSceneReconstruction = false;
    };

    using PlaneCallback = std::function<void(const ARKitPlane& plane, bool added)>;
    using TrackingCallback = std::function<void(ARKitTrackingState state)>;
    using FrameCallback = std::function<void()>;

    ARKitSession();
    ~ARKitSession();

    // Lifecycle
    bool initialize(const Config& config);
    void shutdown();
    bool run();
    void pause();

    // Frame processing  
    bool processFrame();
    
    // Camera
    ARKitTrackingState getTrackingState() const;
    void getCameraViewMatrix(float* outMatrix) const;
    void getCameraProjectionMatrix(float* outMatrix, float nearClip, float farClip) const;
    void getCameraIntrinsics(float* fx, float* fy, float* cx, float* cy) const;
    
    // Background texture (camera feed)
    uint32_t getCameraTextureY() const;
    uint32_t getCameraTextureCbCr() const;
    void getCameraTextureSize(uint32_t& width, uint32_t& height) const;

    // Planes
    void setPlaneCallback(PlaneCallback callback);
    void setTrackingCallback(TrackingCallback callback);
    void setFrameCallback(FrameCallback callback);
    std::vector<ARKitPlane> getPlanes() const;

    // Hit testing
    bool hitTest(float screenX, float screenY, std::vector<ARKitHitResult>& results) const;
    bool hitTestWithFeatures(float screenX, float screenY, std::vector<ARKitHitResult>& results) const;

    // Light estimation
    ARKitLightEstimate getLightEstimate() const;

    // Anchors
    uint64_t createAnchor(const Vec3& position, const Quat& rotation);
    bool getAnchorPose(uint64_t anchorId, Vec3& position, Quat& rotation) const;
    void removeAnchor(uint64_t anchorId);

    // World map (for persistence)
    bool saveWorldMap(const std::string& path);
    bool loadWorldMap(const std::string& path);

    // People occlusion (if enabled)
    bool hasPeopleOcclusion() const;
    uint32_t getSegmentationTexture() const;
    uint32_t getDepthTexture() const;

#ifdef __APPLE__
    // iOS-specific accessors
    ARSession* getARSession() const { return session_; }
    ARFrame* getCurrentFrame() const { return currentFrame_; }
#endif

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
#ifdef __APPLE__
    ARSession* session_;
    ARFrame* currentFrame_;
    ARWorldTrackingConfiguration* configuration_;
#endif
};

} // namespace kimoyooju

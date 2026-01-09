#include "ARCoreSession.hpp"

#ifdef __ANDROID__
#include <arcore_c_api.h>
#include <android/log.h>
#include <unordered_map>

#define LOGAR(...) __android_log_print(ANDROID_LOG_INFO, "KimoyoOjuAR", __VA_ARGS__)
#define LOGAR_ERR(...) __android_log_print(ANDROID_LOG_ERROR, "KimoyoOjuAR", __VA_ARGS__)
#else
#define LOGAR(...) printf(__VA_ARGS__)
#define LOGAR_ERR(...) fprintf(stderr, __VA_ARGS__)
#endif

namespace kimoyooju {

#ifdef __ANDROID__

class ARCoreSession::Impl {
public:
    ArSession* session = nullptr;
    ArConfig* config = nullptr;
    ArFrame* frame = nullptr;
    ArCamera* camera = nullptr;
    
    ARCoreSession::Config appConfig;
    ARTrackingState trackingState = ARTrackingState::NOT_TRACKING;
    
    PlaneCallback planeCallback;
    TrackingCallback trackingCallback;
    
    uint32_t cameraTextureId = 0;
    int32_t displayRotation = 0;
    int32_t width = 0;
    int32_t height = 0;
    
    std::unordered_map<uint64_t, ArAnchor*> anchors;
    uint64_t nextAnchorId = 1;
    
    bool initialized = false;
};

ARCoreSession::ARCoreSession() : impl_(std::make_unique<Impl>()) {}

ARCoreSession::~ARCoreSession() {
    shutdown();
}

bool ARCoreSession::initialize(void* javaContext, void* activity, const Config& config) {
    if (impl_->initialized) {
        return true;
    }
    
    impl_->appConfig = config;
    
    // Check ARCore availability
    ArAvailability availability;
    ArCoreApk_checkAvailability(static_cast<JNIEnv*>(javaContext), 
                                 static_cast<jobject>(activity), &availability);
    
    if (availability != AR_AVAILABILITY_SUPPORTED_INSTALLED) {
        LOGAR_ERR("ARCore not available or not installed");
        return false;
    }
    
    // Create session
    ArStatus status = ArSession_create(static_cast<JNIEnv*>(javaContext), 
                                        static_cast<jobject>(activity), 
                                        &impl_->session);
    if (status != AR_SUCCESS) {
        LOGAR_ERR("Failed to create ARCore session: %d", status);
        return false;
    }
    
    // Create and configure config
    ArConfig_create(impl_->session, &impl_->config);
    
    // Set update mode
    ArConfig_setUpdateMode(impl_->config, AR_UPDATE_MODE_LATEST_CAMERA_IMAGE);
    
    // Configure plane detection
    if (config.enablePlaneDetection) {
        ArConfig_setPlaneFindingMode(impl_->config, AR_PLANE_FINDING_MODE_HORIZONTAL_AND_VERTICAL);
    } else {
        ArConfig_setPlaneFindingMode(impl_->config, AR_PLANE_FINDING_MODE_DISABLED);
    }
    
    // Configure light estimation
    if (config.enableLightEstimation) {
        ArConfig_setLightEstimationMode(impl_->config, AR_LIGHT_ESTIMATION_MODE_ENVIRONMENTAL_HDR);
    } else {
        ArConfig_setLightEstimationMode(impl_->config, AR_LIGHT_ESTIMATION_MODE_DISABLED);
    }
    
    // Configure depth (if supported and requested)
    if (config.enableDepth) {
        int32_t isDepthSupported = 0;
        ArSession_isDepthModeSupported(impl_->session, AR_DEPTH_MODE_AUTOMATIC, &isDepthSupported);
        if (isDepthSupported) {
            ArConfig_setDepthMode(impl_->config, AR_DEPTH_MODE_AUTOMATIC);
        }
    }
    
    // Apply config
    status = ArSession_configure(impl_->session, impl_->config);
    if (status != AR_SUCCESS) {
        LOGAR_ERR("Failed to configure ARCore session: %d", status);
        ArConfig_destroy(impl_->config);
        ArSession_destroy(impl_->session);
        impl_->config = nullptr;
        impl_->session = nullptr;
        return false;
    }
    
    // Create frame
    ArFrame_create(impl_->session, &impl_->frame);
    
    // Generate camera texture
    glGenTextures(1, &impl_->cameraTextureId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, impl_->cameraTextureId);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    ArSession_setCameraTextureName(impl_->session, impl_->cameraTextureId);
    
    impl_->initialized = true;
    LOGAR("ARCore session initialized");
    
    return true;
}

void ARCoreSession::shutdown() {
    if (!impl_->initialized) {
        return;
    }
    
    // Destroy anchors
    for (auto& pair : impl_->anchors) {
        ArAnchor_release(pair.second);
    }
    impl_->anchors.clear();
    
    // Destroy frame
    if (impl_->frame) {
        ArFrame_destroy(impl_->frame);
        impl_->frame = nullptr;
    }
    
    // Destroy config
    if (impl_->config) {
        ArConfig_destroy(impl_->config);
        impl_->config = nullptr;
    }
    
    // Destroy session
    if (impl_->session) {
        ArSession_destroy(impl_->session);
        impl_->session = nullptr;
    }
    
    // Delete camera texture
    if (impl_->cameraTextureId != 0) {
        glDeleteTextures(1, &impl_->cameraTextureId);
        impl_->cameraTextureId = 0;
    }
    
    impl_->initialized = false;
    LOGAR("ARCore session shutdown");
}

bool ARCoreSession::resume() {
    if (!impl_->session) {
        return false;
    }
    
    ArStatus status = ArSession_resume(impl_->session);
    if (status != AR_SUCCESS) {
        LOGAR_ERR("Failed to resume ARCore session: %d", status);
        return false;
    }
    
    LOGAR("ARCore session resumed");
    return true;
}

void ARCoreSession::pause() {
    if (impl_->session) {
        ArSession_pause(impl_->session);
        LOGAR("ARCore session paused");
    }
}

bool ARCoreSession::update() {
    if (!impl_->session || !impl_->frame) {
        return false;
    }
    
    // Set display geometry for proper camera projection
    ArSession_setDisplayGeometry(impl_->session, impl_->displayRotation, 
                                  impl_->width, impl_->height);
    
    // Update session
    ArStatus status = ArSession_update(impl_->session, impl_->frame);
    if (status != AR_SUCCESS) {
        LOGAR_ERR("ARCore update failed: %d", status);
        return false;
    }
    
    // Get camera
    ArFrame_acquireCamera(impl_->frame, &impl_->camera);
    
    // Check tracking state
    ArTrackingState arTrackingState;
    ArCamera_getTrackingState(impl_->session, impl_->camera, &arTrackingState);
    
    ARTrackingState newState;
    switch (arTrackingState) {
        case AR_TRACKING_STATE_TRACKING:
            newState = ARTrackingState::TRACKING;
            break;
        case AR_TRACKING_STATE_PAUSED:
            newState = ARTrackingState::PAUSED;
            break;
        default:
            newState = ARTrackingState::NOT_TRACKING;
            break;
    }
    
    if (newState != impl_->trackingState) {
        impl_->trackingState = newState;
        if (impl_->trackingCallback) {
            impl_->trackingCallback(newState);
        }
    }
    
    // Process planes if callback registered
    if (impl_->planeCallback && impl_->appConfig.enablePlaneDetection) {
        ArTrackableList* planeList = nullptr;
        ArTrackableList_create(impl_->session, &planeList);
        ArSession_getAllTrackables(impl_->session, AR_TRACKABLE_PLANE, planeList);
        
        int32_t planeCount = 0;
        ArTrackableList_getSize(impl_->session, planeList, &planeCount);
        
        for (int32_t i = 0; i < planeCount; ++i) {
            ArTrackable* trackable = nullptr;
            ArTrackableList_acquireItem(impl_->session, planeList, i, &trackable);
            
            ArPlane* plane = ArAsPlane(trackable);
            
            ArTrackingState planeTrackingState;
            ArTrackable_getTrackingState(impl_->session, trackable, &planeTrackingState);
            
            if (planeTrackingState == AR_TRACKING_STATE_TRACKING) {
                ARPlane arPlane;
                arPlane.id = reinterpret_cast<uint64_t>(plane);
                
                // Get plane type
                ArPlaneType arPlaneType;
                ArPlane_getType(impl_->session, plane, &arPlaneType);
                switch (arPlaneType) {
                    case AR_PLANE_HORIZONTAL_UPWARD_FACING:
                        arPlane.type = ARPlaneType::HORIZONTAL_UPWARD;
                        break;
                    case AR_PLANE_HORIZONTAL_DOWNWARD_FACING:
                        arPlane.type = ARPlaneType::HORIZONTAL_DOWNWARD;
                        break;
                    case AR_PLANE_VERTICAL:
                        arPlane.type = ARPlaneType::VERTICAL;
                        break;
                }
                
                // Get center pose
                ArPose* centerPose = nullptr;
                ArPose_create(impl_->session, nullptr, &centerPose);
                ArPlane_getCenterPose(impl_->session, plane, centerPose);
                
                float poseRaw[7];
                ArPose_getPoseRaw(impl_->session, centerPose, poseRaw);
                arPlane.centerPose = {poseRaw[4], poseRaw[5], poseRaw[6]};
                arPlane.centerRotation = {poseRaw[0], poseRaw[1], poseRaw[2], poseRaw[3]};
                
                ArPose_destroy(centerPose);
                
                // Get extent
                ArPlane_getExtentX(impl_->session, plane, &arPlane.extentX);
                ArPlane_getExtentZ(impl_->session, plane, &arPlane.extentZ);
                
                // Check if subsumed
                ArPlane* subsumingPlane = nullptr;
                ArPlane_acquireSubsumedBy(impl_->session, plane, &subsumingPlane);
                arPlane.isSubsumed = (subsumingPlane != nullptr);
                if (subsumingPlane) {
                    ArTrackable_release(ArAsTrackable(subsumingPlane));
                }
                
                impl_->planeCallback(arPlane, true);
            }
            
            ArTrackable_release(trackable);
        }
        
        ArTrackableList_destroy(planeList);
    }
    
    ArCamera_release(impl_->camera);
    impl_->camera = nullptr;
    
    return true;
}

bool ARCoreSession::beginFrame() {
    return impl_->session != nullptr && impl_->frame != nullptr;
}

void ARCoreSession::endFrame() {
    // Nothing special needed for ARCore
}

ARTrackingState ARCoreSession::getTrackingState() const {
    return impl_->trackingState;
}

void ARCoreSession::getCameraViewMatrix(float* outMatrix) const {
    if (!impl_->session || !impl_->frame) {
        // Return identity
        for (int i = 0; i < 16; i++) outMatrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        return;
    }
    
    ArCamera* camera = nullptr;
    ArFrame_acquireCamera(impl_->frame, &camera);
    
    ArCamera_getViewMatrix(impl_->session, camera, outMatrix);
    
    ArCamera_release(camera);
}

void ARCoreSession::getCameraProjectionMatrix(float* outMatrix, float nearClip, float farClip) const {
    if (!impl_->session || !impl_->frame) {
        // Return identity
        for (int i = 0; i < 16; i++) outMatrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        return;
    }
    
    ArCamera* camera = nullptr;
    ArFrame_acquireCamera(impl_->frame, &camera);
    
    ArCamera_getProjectionMatrix(impl_->session, camera, nearClip, farClip, outMatrix);
    
    ArCamera_release(camera);
}

void ARCoreSession::getCameraTextureSize(uint32_t& width, uint32_t& height) const {
    width = impl_->width;
    height = impl_->height;
}

uint32_t ARCoreSession::getCameraTextureId() const {
    return impl_->cameraTextureId;
}

void ARCoreSession::setPlaneCallback(PlaneCallback callback) {
    impl_->planeCallback = std::move(callback);
}

void ARCoreSession::setTrackingCallback(TrackingCallback callback) {
    impl_->trackingCallback = std::move(callback);
}

std::vector<ARPlane> ARCoreSession::getPlanes() const {
    std::vector<ARPlane> result;
    
    if (!impl_->session) {
        return result;
    }
    
    ArTrackableList* planeList = nullptr;
    ArTrackableList_create(impl_->session, &planeList);
    ArSession_getAllTrackables(impl_->session, AR_TRACKABLE_PLANE, planeList);
    
    int32_t planeCount = 0;
    ArTrackableList_getSize(impl_->session, planeList, &planeCount);
    
    for (int32_t i = 0; i < planeCount; ++i) {
        ArTrackable* trackable = nullptr;
        ArTrackableList_acquireItem(impl_->session, planeList, i, &trackable);
        
        ArTrackingState trackingState;
        ArTrackable_getTrackingState(impl_->session, trackable, &trackingState);
        
        if (trackingState == AR_TRACKING_STATE_TRACKING) {
            ArPlane* plane = ArAsPlane(trackable);
            
            ARPlane arPlane;
            arPlane.id = reinterpret_cast<uint64_t>(plane);
            
            ArPlaneType arPlaneType;
            ArPlane_getType(impl_->session, plane, &arPlaneType);
            switch (arPlaneType) {
                case AR_PLANE_HORIZONTAL_UPWARD_FACING:
                    arPlane.type = ARPlaneType::HORIZONTAL_UPWARD;
                    break;
                case AR_PLANE_HORIZONTAL_DOWNWARD_FACING:
                    arPlane.type = ARPlaneType::HORIZONTAL_DOWNWARD;
                    break;
                case AR_PLANE_VERTICAL:
                    arPlane.type = ARPlaneType::VERTICAL;
                    break;
            }
            
            ArPose* centerPose = nullptr;
            ArPose_create(impl_->session, nullptr, &centerPose);
            ArPlane_getCenterPose(impl_->session, plane, centerPose);
            
            float poseRaw[7];
            ArPose_getPoseRaw(impl_->session, centerPose, poseRaw);
            arPlane.centerPose = {poseRaw[4], poseRaw[5], poseRaw[6]};
            arPlane.centerRotation = {poseRaw[0], poseRaw[1], poseRaw[2], poseRaw[3]};
            
            ArPose_destroy(centerPose);
            
            ArPlane_getExtentX(impl_->session, plane, &arPlane.extentX);
            ArPlane_getExtentZ(impl_->session, plane, &arPlane.extentZ);
            
            ArPlane* subsumingPlane = nullptr;
            ArPlane_acquireSubsumedBy(impl_->session, plane, &subsumingPlane);
            arPlane.isSubsumed = (subsumingPlane != nullptr);
            if (subsumingPlane) {
                ArTrackable_release(ArAsTrackable(subsumingPlane));
            }
            
            result.push_back(arPlane);
        }
        
        ArTrackable_release(trackable);
    }
    
    ArTrackableList_destroy(planeList);
    
    return result;
}

bool ARCoreSession::hitTest(float screenX, float screenY, std::vector<ARHitResult>& results) const {
    results.clear();
    
    if (!impl_->session || !impl_->frame) {
        return false;
    }
    
    ArHitResultList* hitList = nullptr;
    ArHitResultList_create(impl_->session, &hitList);
    
    ArFrame_hitTest(impl_->session, impl_->frame, screenX, screenY, hitList);
    
    int32_t hitCount = 0;
    ArHitResultList_getSize(impl_->session, hitList, &hitCount);
    
    for (int32_t i = 0; i < hitCount; ++i) {
        ArHitResult* hit = nullptr;
        ArHitResult_create(impl_->session, &hit);
        ArHitResultList_getItem(impl_->session, hitList, i, hit);
        
        ArPose* pose = nullptr;
        ArPose_create(impl_->session, nullptr, &pose);
        ArHitResult_getHitPose(impl_->session, hit, pose);
        
        float poseRaw[7];
        ArPose_getPoseRaw(impl_->session, pose, poseRaw);
        
        ARHitResult result;
        result.position = {poseRaw[4], poseRaw[5], poseRaw[6]};
        result.rotation = {poseRaw[0], poseRaw[1], poseRaw[2], poseRaw[3]};
        
        ArHitResult_getDistance(impl_->session, hit, &result.distance);
        
        // Check if hit a plane
        ArTrackable* trackable = nullptr;
        ArHitResult_acquireTrackable(impl_->session, hit, &trackable);
        
        ArTrackableType trackableType;
        ArTrackable_getType(impl_->session, trackable, &trackableType);
        
        if (trackableType == AR_TRACKABLE_PLANE) {
            result.planeId = reinterpret_cast<uint64_t>(trackable);
        } else {
            result.planeId = 0;
        }
        
        ArTrackable_release(trackable);
        ArPose_destroy(pose);
        ArHitResult_destroy(hit);
        
        results.push_back(result);
    }
    
    ArHitResultList_destroy(hitList);
    
    return !results.empty();
}

ARLightEstimate ARCoreSession::getLightEstimate() const {
    ARLightEstimate estimate;
    estimate.ambientIntensity = 1.0f;
    estimate.ambientColor = {1.0f, 1.0f, 1.0f, 1.0f};
    estimate.mainLightDirection = {0.0f, -1.0f, 0.0f};
    estimate.mainLightIntensity = 1.0f;
    
    if (!impl_->session || !impl_->frame || !impl_->appConfig.enableLightEstimation) {
        return estimate;
    }
    
    ArLightEstimate* lightEstimate = nullptr;
    ArLightEstimate_create(impl_->session, &lightEstimate);
    ArFrame_getLightEstimate(impl_->session, impl_->frame, lightEstimate);
    
    ArLightEstimateState state;
    ArLightEstimate_getState(impl_->session, lightEstimate, &state);
    
    if (state == AR_LIGHT_ESTIMATE_STATE_VALID) {
        float intensity;
        ArLightEstimate_getPixelIntensity(impl_->session, lightEstimate, &intensity);
        estimate.ambientIntensity = intensity;
        
        float colorCorrection[4];
        ArLightEstimate_getColorCorrection(impl_->session, lightEstimate, colorCorrection);
        estimate.ambientColor = {colorCorrection[0], colorCorrection[1], colorCorrection[2], colorCorrection[3]};
        
        float direction[3];
        ArLightEstimate_getEnkimoyoojunmentalHdrMainLightDirection(impl_->session, lightEstimate, direction);
        estimate.mainLightDirection = {direction[0], direction[1], direction[2]};
        
        float mainLightIntensity[3];
        ArLightEstimate_getEnkimoyoojunmentalHdrMainLightIntensity(impl_->session, lightEstimate, mainLightIntensity);
        estimate.mainLightIntensity = (mainLightIntensity[0] + mainLightIntensity[1] + mainLightIntensity[2]) / 3.0f;
    }
    
    ArLightEstimate_destroy(lightEstimate);
    
    return estimate;
}

uint64_t ARCoreSession::createAnchor(const Vec3& position, const Quat& rotation) {
    if (!impl_->session) {
        return 0;
    }
    
    float poseRaw[7] = {rotation.x, rotation.y, rotation.z, rotation.w,
                        position.x, position.y, position.z};
    
    ArPose* pose = nullptr;
    ArPose_create(impl_->session, poseRaw, &pose);
    
    ArAnchor* anchor = nullptr;
    ArStatus status = ArSession_acquireNewAnchor(impl_->session, pose, &anchor);
    
    ArPose_destroy(pose);
    
    if (status != AR_SUCCESS || anchor == nullptr) {
        LOGAR_ERR("Failed to create anchor: %d", status);
        return 0;
    }
    
    uint64_t anchorId = impl_->nextAnchorId++;
    impl_->anchors[anchorId] = anchor;
    
    return anchorId;
}

bool ARCoreSession::getAnchorPose(uint64_t anchorId, Vec3& position, Quat& rotation) const {
    auto it = impl_->anchors.find(anchorId);
    if (it == impl_->anchors.end()) {
        return false;
    }
    
    ArAnchor* anchor = it->second;
    
    ArTrackingState trackingState;
    ArAnchor_getTrackingState(impl_->session, anchor, &trackingState);
    
    if (trackingState != AR_TRACKING_STATE_TRACKING) {
        return false;
    }
    
    ArPose* pose = nullptr;
    ArPose_create(impl_->session, nullptr, &pose);
    ArAnchor_getPose(impl_->session, anchor, pose);
    
    float poseRaw[7];
    ArPose_getPoseRaw(impl_->session, pose, poseRaw);
    
    rotation = {poseRaw[0], poseRaw[1], poseRaw[2], poseRaw[3]};
    position = {poseRaw[4], poseRaw[5], poseRaw[6]};
    
    ArPose_destroy(pose);
    
    return true;
}

void ARCoreSession::removeAnchor(uint64_t anchorId) {
    auto it = impl_->anchors.find(anchorId);
    if (it != impl_->anchors.end()) {
        ArAnchor_release(it->second);
        impl_->anchors.erase(it);
    }
}

bool ARCoreSession::hasDepthImage() const {
    if (!impl_->session || !impl_->frame || !impl_->appConfig.enableDepth) {
        return false;
    }
    
    ArImage* depthImage = nullptr;
    ArStatus status = ArFrame_acquireDepthImage(impl_->session, impl_->frame, &depthImage);
    
    if (status == AR_SUCCESS && depthImage != nullptr) {
        ArImage_release(depthImage);
        return true;
    }
    
    return false;
}

float ARCoreSession::getDepthAtPoint(float screenX, float screenY) const {
    // Simplified - actual implementation would sample depth texture
    return 0.0f;
}

#else // !__ANDROID__

// Stub implementation for non-Android platforms
class ARCoreSession::Impl {};

ARCoreSession::ARCoreSession() : impl_(std::make_unique<Impl>()) {}
ARCoreSession::~ARCoreSession() {}
bool ARCoreSession::initialize(void*, void*, const Config&) { return false; }
void ARCoreSession::shutdown() {}
bool ARCoreSession::resume() { return false; }
void ARCoreSession::pause() {}
bool ARCoreSession::update() { return false; }
bool ARCoreSession::beginFrame() { return false; }
void ARCoreSession::endFrame() {}
ARTrackingState ARCoreSession::getTrackingState() const { return ARTrackingState::NOT_TRACKING; }
void ARCoreSession::getCameraViewMatrix(float* m) const { for(int i=0;i<16;i++)m[i]=(i%5==0)?1:0; }
void ARCoreSession::getCameraProjectionMatrix(float* m, float, float) const { for(int i=0;i<16;i++)m[i]=(i%5==0)?1:0; }
void ARCoreSession::getCameraTextureSize(uint32_t& w, uint32_t& h) const { w=h=0; }
uint32_t ARCoreSession::getCameraTextureId() const { return 0; }
void ARCoreSession::setPlaneCallback(PlaneCallback) {}
void ARCoreSession::setTrackingCallback(TrackingCallback) {}
std::vector<ARPlane> ARCoreSession::getPlanes() const { return {}; }
bool ARCoreSession::hitTest(float, float, std::vector<ARHitResult>&) const { return false; }
ARLightEstimate ARCoreSession::getLightEstimate() const { return {}; }
uint64_t ARCoreSession::createAnchor(const Vec3&, const Quat&) { return 0; }
bool ARCoreSession::getAnchorPose(uint64_t, Vec3&, Quat&) const { return false; }
void ARCoreSession::removeAnchor(uint64_t) {}
bool ARCoreSession::hasDepthImage() const { return false; }
float ARCoreSession::getDepthAtPoint(float, float) const { return 0; }

#endif // __ANDROID__

} // namespace kimoyooju

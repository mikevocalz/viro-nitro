#import "ARKitSession.h"

#ifdef __APPLE__

#import <simd/simd.h>
#import <MetalKit/MetalKit.h>
#import <CoreVideo/CoreVideo.h>

namespace kimoyooju {

// ============================================================================
// ARKit Session Delegate
// ============================================================================

@interface KimoyoOjuARSessionDelegate : NSObject <ARSessionDelegate>
@property (nonatomic, assign) ARKitSession* session;
@property (nonatomic, copy) void (^frameCallback)(void);
@property (nonatomic, copy) void (^trackingCallback)(ARKitTrackingState);
@property (nonatomic, copy) void (^planeCallback)(const ARKitPlane&, bool);
@end

@implementation KimoyoOjuARSessionDelegate

- (void)session:(ARSession *)session didUpdateFrame:(ARFrame *)frame {
    if (self.frameCallback) {
        self.frameCallback();
    }
}

- (void)session:(ARSession *)session didAddAnchors:(NSArray<ARAnchor *> *)anchors {
    if (!self.planeCallback) return;
    
    for (ARAnchor* anchor in anchors) {
        if ([anchor isKindOfClass:[ARPlaneAnchor class]]) {
            ARPlaneAnchor* planeAnchor = (ARPlaneAnchor*)anchor;
            ARKitPlane plane = [self planeFromAnchor:planeAnchor];
            self.planeCallback(plane, true);
        }
    }
}

- (void)session:(ARSession *)session didUpdateAnchors:(NSArray<ARAnchor *> *)anchors {
    if (!self.planeCallback) return;
    
    for (ARAnchor* anchor in anchors) {
        if ([anchor isKindOfClass:[ARPlaneAnchor class]]) {
            ARPlaneAnchor* planeAnchor = (ARPlaneAnchor*)anchor;
            ARKitPlane plane = [self planeFromAnchor:planeAnchor];
            self.planeCallback(plane, false);
        }
    }
}

- (void)session:(ARSession *)session cameraDidChangeTrackingState:(ARCamera *)camera {
    if (!self.trackingCallback) return;
    
    ARKitTrackingState state;
    switch (camera.trackingState) {
        case ARTrackingStateNotAvailable:
            state = ARKitTrackingState::NOT_AVAILABLE;
            break;
        case ARTrackingStateLimited:
            state = ARKitTrackingState::LIMITED;
            break;
        case ARTrackingStateNormal:
            state = ARKitTrackingState::NORMAL;
            break;
    }
    self.trackingCallback(state);
}

- (ARKitPlane)planeFromAnchor:(ARPlaneAnchor*)anchor {
    ARKitPlane plane;
    plane.identifier = (uint64_t)anchor.identifier.hash;
    plane.type = anchor.alignment == ARPlaneAnchorAlignmentHorizontal ? 
                 ARKitPlaneType::HORIZONTAL : ARKitPlaneType::VERTICAL;
    
    simd_float4x4 transform = anchor.transform;
    plane.center = {transform.columns[3].x, transform.columns[3].y, transform.columns[3].z};
    
    // Extract rotation quaternion from transform matrix
    simd_quatf quat = simd_quaternion(transform);
    plane.rotation = {quat.vector.x, quat.vector.y, quat.vector.z, quat.vector.w};
    
    plane.width = anchor.extent.x;
    plane.height = anchor.extent.z;
    
    return plane;
}

@end

// ============================================================================
// ARKitSession Implementation
// ============================================================================

class ARKitSession::Impl {
public:
    KimoyoOjuARSessionDelegate* delegate = nil;
    Config config;
    ARKitTrackingState trackingState = ARKitTrackingState::NOT_AVAILABLE;
    NSMutableDictionary<NSUUID*, ARAnchor*>* customAnchors;
    uint64_t nextAnchorId = 1;
    std::unordered_map<uint64_t, NSUUID*> anchorIdMap;
};

ARKitSession::ARKitSession()
    : impl_(std::make_unique<Impl>())
    , session_(nil)
    , currentFrame_(nil)
    , configuration_(nil) {
}

ARKitSession::~ARKitSession() {
    shutdown();
}

bool ARKitSession::initialize(const Config& config) {
    impl_->config = config;
    
    @autoreleasepool {
        // Check ARKit availability
        if (!ARWorldTrackingConfiguration.isSupported) {
            NSLog(@"ARKit World Tracking not supported on this device");
            return false;
        }
        
        // Create session
        session_ = [[ARSession alloc] init];
        
        // Create delegate
        impl_->delegate = [[KimoyoOjuARSessionDelegate alloc] init];
        impl_->delegate.session = this;
        session_.delegate = impl_->delegate;
        
        // Create configuration
        configuration_ = [[ARWorldTrackingConfiguration alloc] init];
        
        // Configure plane detection
        if (config.enablePlaneDetection) {
            configuration_.planeDetection = ARPlaneDetectionHorizontal | ARPlaneDetectionVertical;
        } else {
            configuration_.planeDetection = ARPlaneDetectionNone;
        }
        
        // Configure light estimation
        configuration_.lightEstimationEnabled = config.enableLightEstimation;
        
        // Configure scene reconstruction (iOS 13.4+)
        if (@available(iOS 13.4, *)) {
            if (config.enableSceneReconstruction && 
                ARWorldTrackingConfiguration.supportsSceneReconstruction) {
                configuration_.sceneReconstruction = ARSceneReconstructionMesh;
            }
        }
        
        // Configure people occlusion (iOS 13+)
        if (@available(iOS 13.0, *)) {
            if (config.enablePeopleOcclusion) {
                if (ARWorldTrackingConfiguration.supportsFrameSemantics) {
                    configuration_.frameSemantics = ARFrameSemanticPersonSegmentationWithDepth;
                }
            }
        }
        
        // Initialize anchor storage
        impl_->customAnchors = [[NSMutableDictionary alloc] init];
        
        NSLog(@"ARKit session initialized");
        return true;
    }
}

void ARKitSession::shutdown() {
    @autoreleasepool {
        if (session_) {
            [session_ pause];
            session_.delegate = nil;
            [session_ release];
            session_ = nil;
        }
        
        if (impl_->delegate) {
            [impl_->delegate release];
            impl_->delegate = nil;
        }
        
        if (configuration_) {
            [configuration_ release];
            configuration_ = nil;
        }
        
        if (impl_->customAnchors) {
            [impl_->customAnchors release];
            impl_->customAnchors = nil;
        }
        
        currentFrame_ = nil;
        
        NSLog(@"ARKit session shutdown");
    }
}

bool ARKitSession::run() {
    if (!session_ || !configuration_) {
        return false;
    }
    
    @autoreleasepool {
        [session_ runWithConfiguration:configuration_];
        NSLog(@"ARKit session running");
        return true;
    }
}

void ARKitSession::pause() {
    if (session_) {
        [session_ pause];
        NSLog(@"ARKit session paused");
    }
}

bool ARKitSession::processFrame() {
    if (!session_) {
        return false;
    }
    
    @autoreleasepool {
        currentFrame_ = session_.currentFrame;
        
        if (!currentFrame_) {
            return false;
        }
        
        // Update tracking state
        ARCamera* camera = currentFrame_.camera;
        switch (camera.trackingState) {
            case ARTrackingStateNotAvailable:
                impl_->trackingState = ARKitTrackingState::NOT_AVAILABLE;
                break;
            case ARTrackingStateLimited:
                impl_->trackingState = ARKitTrackingState::LIMITED;
                break;
            case ARTrackingStateNormal:
                impl_->trackingState = ARKitTrackingState::NORMAL;
                break;
        }
        
        return true;
    }
}

ARKitTrackingState ARKitSession::getTrackingState() const {
    return impl_->trackingState;
}

void ARKitSession::getCameraViewMatrix(float* outMatrix) const {
    if (!currentFrame_) {
        // Return identity
        for (int i = 0; i < 16; i++) outMatrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        return;
    }
    
    @autoreleasepool {
        simd_float4x4 viewMatrix = currentFrame_.camera.viewMatrix(forOrientation:UIInterfaceOrientationPortrait);
        memcpy(outMatrix, &viewMatrix, sizeof(simd_float4x4));
    }
}

void ARKitSession::getCameraProjectionMatrix(float* outMatrix, float nearClip, float farClip) const {
    if (!currentFrame_) {
        // Return identity
        for (int i = 0; i < 16; i++) outMatrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        return;
    }
    
    @autoreleasepool {
        CGSize viewportSize = CGSizeMake(UIScreen.mainScreen.bounds.size.width,
                                          UIScreen.mainScreen.bounds.size.height);
        
        simd_float4x4 projMatrix = [currentFrame_.camera 
            projectionMatrixForOrientation:UIInterfaceOrientationPortrait
                              viewportSize:viewportSize
                                     zNear:nearClip
                                      zFar:farClip];
        
        memcpy(outMatrix, &projMatrix, sizeof(simd_float4x4));
    }
}

void ARKitSession::getCameraIntrinsics(float* fx, float* fy, float* cx, float* cy) const {
    if (!currentFrame_) {
        *fx = *fy = *cx = *cy = 0;
        return;
    }
    
    @autoreleasepool {
        simd_float3x3 intrinsics = currentFrame_.camera.intrinsics;
        *fx = intrinsics.columns[0].x;
        *fy = intrinsics.columns[1].y;
        *cx = intrinsics.columns[2].x;
        *cy = intrinsics.columns[2].y;
    }
}

uint32_t ARKitSession::getCameraTextureY() const {
    if (!currentFrame_) return 0;
    
    CVPixelBufferRef pixelBuffer = currentFrame_.capturedImage;
    if (!pixelBuffer) return 0;
    
    // In actual implementation, this would return a Metal texture ID
    // created from the CVPixelBuffer's Y plane
    return 0; // Placeholder
}

uint32_t ARKitSession::getCameraTextureCbCr() const {
    if (!currentFrame_) return 0;
    
    // Similar to above, for CbCr plane
    return 0; // Placeholder
}

void ARKitSession::getCameraTextureSize(uint32_t& width, uint32_t& height) const {
    if (!currentFrame_) {
        width = height = 0;
        return;
    }
    
    @autoreleasepool {
        CVPixelBufferRef pixelBuffer = currentFrame_.capturedImage;
        if (pixelBuffer) {
            width = (uint32_t)CVPixelBufferGetWidth(pixelBuffer);
            height = (uint32_t)CVPixelBufferGetHeight(pixelBuffer);
        }
    }
}

void ARKitSession::setPlaneCallback(PlaneCallback callback) {
    if (impl_->delegate) {
        impl_->delegate.planeCallback = ^(const ARKitPlane& plane, bool added) {
            callback(plane, added);
        };
    }
}

void ARKitSession::setTrackingCallback(TrackingCallback callback) {
    if (impl_->delegate) {
        impl_->delegate.trackingCallback = ^(ARKitTrackingState state) {
            callback(state);
        };
    }
}

void ARKitSession::setFrameCallback(FrameCallback callback) {
    if (impl_->delegate) {
        impl_->delegate.frameCallback = ^{
            callback();
        };
    }
}

std::vector<ARKitPlane> ARKitSession::getPlanes() const {
    std::vector<ARKitPlane> result;
    
    if (!currentFrame_) return result;
    
    @autoreleasepool {
        for (ARAnchor* anchor in currentFrame_.anchors) {
            if ([anchor isKindOfClass:[ARPlaneAnchor class]]) {
                ARPlaneAnchor* planeAnchor = (ARPlaneAnchor*)anchor;
                ARKitPlane plane = [impl_->delegate planeFromAnchor:planeAnchor];
                result.push_back(plane);
            }
        }
    }
    
    return result;
}

bool ARKitSession::hitTest(float screenX, float screenY, std::vector<ARKitHitResult>& results) const {
    results.clear();
    
    if (!currentFrame_) return false;
    
    @autoreleasepool {
        CGPoint point = CGPointMake(screenX, screenY);
        
        NSArray<ARRaycastResult*>* raycastResults = [session_ 
            raycast:[ARRaycastQuery alloc] 
            initWithOrigin:simd_make_float3(0, 0, 0) 
            direction:simd_make_float3(0, 0, -1) 
            allowingTarget:ARRaycastTargetExistingPlaneGeometry 
            alignment:ARRaycastTargetAlignmentAny];
        
        // Use deprecated hit test for simplicity (raycast requires more setup)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
        NSArray<ARHitTestResult*>* hits = [currentFrame_ hitTest:point 
            types:ARHitTestResultTypeExistingPlaneUsingExtent | 
                  ARHitTestResultTypeExistingPlane];
        #pragma clang diagnostic pop
        
        for (ARHitTestResult* hit in hits) {
            ARKitHitResult result;
            
            simd_float4x4 transform = hit.worldTransform;
            result.position = {transform.columns[3].x, transform.columns[3].y, transform.columns[3].z};
            
            simd_quatf quat = simd_quaternion(transform);
            result.rotation = {quat.vector.x, quat.vector.y, quat.vector.z, quat.vector.w};
            
            result.distance = hit.distance;
            
            if (hit.anchor && [hit.anchor isKindOfClass:[ARPlaneAnchor class]]) {
                result.planeIdentifier = (uint64_t)hit.anchor.identifier.hash;
            } else {
                result.planeIdentifier = 0;
            }
            
            results.push_back(result);
        }
        
        return !results.empty();
    }
}

bool ARKitSession::hitTestWithFeatures(float screenX, float screenY, std::vector<ARKitHitResult>& results) const {
    results.clear();
    
    if (!currentFrame_) return false;
    
    @autoreleasepool {
        CGPoint point = CGPointMake(screenX, screenY);
        
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
        NSArray<ARHitTestResult*>* hits = [currentFrame_ hitTest:point 
            types:ARHitTestResultTypeFeaturePoint];
        #pragma clang diagnostic pop
        
        for (ARHitTestResult* hit in hits) {
            ARKitHitResult result;
            
            simd_float4x4 transform = hit.worldTransform;
            result.position = {transform.columns[3].x, transform.columns[3].y, transform.columns[3].z};
            
            simd_quatf quat = simd_quaternion(transform);
            result.rotation = {quat.vector.x, quat.vector.y, quat.vector.z, quat.vector.w};
            
            result.distance = hit.distance;
            result.planeIdentifier = 0;
            
            results.push_back(result);
        }
        
        return !results.empty();
    }
}

ARKitLightEstimate ARKitSession::getLightEstimate() const {
    ARKitLightEstimate estimate = {1000.0f, 6500.0f}; // Default values
    
    if (!currentFrame_ || !currentFrame_.lightEstimate) {
        return estimate;
    }
    
    @autoreleasepool {
        ARLightEstimate* lightEstimate = currentFrame_.lightEstimate;
        estimate.ambientIntensity = lightEstimate.ambientIntensity;
        estimate.ambientColorTemperature = lightEstimate.ambientColorTemperature;
    }
    
    return estimate;
}

uint64_t ARKitSession::createAnchor(const Vec3& position, const Quat& rotation) {
    if (!session_) return 0;
    
    @autoreleasepool {
        simd_float4x4 transform = matrix_identity_float4x4;
        
        // Set position
        transform.columns[3].x = position.x;
        transform.columns[3].y = position.y;
        transform.columns[3].z = position.z;
        
        // Set rotation from quaternion
        simd_quatf quat = simd_quaternion(rotation.x, rotation.y, rotation.z, rotation.w);
        simd_float4x4 rotMatrix = simd_matrix4x4(quat);
        transform.columns[0] = rotMatrix.columns[0];
        transform.columns[1] = rotMatrix.columns[1];
        transform.columns[2] = rotMatrix.columns[2];
        
        ARAnchor* anchor = [[ARAnchor alloc] initWithTransform:transform];
        [session_ addAnchor:anchor];
        
        uint64_t anchorId = impl_->nextAnchorId++;
        impl_->customAnchors[anchor.identifier] = anchor;
        impl_->anchorIdMap[anchorId] = anchor.identifier;
        
        [anchor release];
        
        return anchorId;
    }
}

bool ARKitSession::getAnchorPose(uint64_t anchorId, Vec3& position, Quat& rotation) const {
    auto it = impl_->anchorIdMap.find(anchorId);
    if (it == impl_->anchorIdMap.end()) {
        return false;
    }
    
    @autoreleasepool {
        ARAnchor* anchor = impl_->customAnchors[it->second];
        if (!anchor) return false;
        
        simd_float4x4 transform = anchor.transform;
        position = {transform.columns[3].x, transform.columns[3].y, transform.columns[3].z};
        
        simd_quatf quat = simd_quaternion(transform);
        rotation = {quat.vector.x, quat.vector.y, quat.vector.z, quat.vector.w};
        
        return true;
    }
}

void ARKitSession::removeAnchor(uint64_t anchorId) {
    auto it = impl_->anchorIdMap.find(anchorId);
    if (it == impl_->anchorIdMap.end()) {
        return;
    }
    
    @autoreleasepool {
        ARAnchor* anchor = impl_->customAnchors[it->second];
        if (anchor && session_) {
            [session_ removeAnchor:anchor];
        }
        [impl_->customAnchors removeObjectForKey:it->second];
        impl_->anchorIdMap.erase(it);
    }
}

bool ARKitSession::saveWorldMap(const std::string& path) {
    if (!session_) return false;
    
    __block bool success = false;
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    
    @autoreleasepool {
        [session_ getCurrentWorldMapWithCompletionHandler:^(ARWorldMap* worldMap, NSError* error) {
            if (worldMap && !error) {
                NSData* data = [NSKeyedArchiver archivedDataWithRootObject:worldMap
                                                     requiringSecureCoding:YES
                                                                     error:nil];
                if (data) {
                    NSString* pathStr = [NSString stringWithUTF8String:path.c_str()];
                    success = [data writeToFile:pathStr atomically:YES];
                }
            }
            dispatch_semaphore_signal(semaphore);
        }];
    }
    
    dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, 5 * NSEC_PER_SEC));
    return success;
}

bool ARKitSession::loadWorldMap(const std::string& path) {
    if (!session_ || !configuration_) return false;
    
    @autoreleasepool {
        NSString* pathStr = [NSString stringWithUTF8String:path.c_str()];
        NSData* data = [NSData dataWithContentsOfFile:pathStr];
        if (!data) return false;
        
        NSError* error = nil;
        ARWorldMap* worldMap = [NSKeyedUnarchiver unarchivedObjectOfClass:[ARWorldMap class]
                                                                 fromData:data
                                                                    error:&error];
        if (!worldMap || error) return false;
        
        configuration_.initialWorldMap = worldMap;
        [session_ runWithConfiguration:configuration_ options:ARSessionRunOptionResetTracking];
        
        return true;
    }
}

bool ARKitSession::hasPeopleOcclusion() const {
    if (@available(iOS 13.0, *)) {
        return impl_->config.enablePeopleOcclusion && 
               ARWorldTrackingConfiguration.supportsFrameSemantics;
    }
    return false;
}

uint32_t ARKitSession::getSegmentationTexture() const {
    // Would return Metal texture ID for segmentation buffer
    return 0; // Placeholder
}

uint32_t ARKitSession::getDepthTexture() const {
    // Would return Metal texture ID for depth buffer
    return 0; // Placeholder
}

} // namespace kimoyooju

#endif // __APPLE__

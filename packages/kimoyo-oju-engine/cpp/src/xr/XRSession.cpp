#include "../../include/kimoyooju/XRSession.hpp"
#include <chrono>

#ifdef KIMOYOOJU_ENABLE_OPENXR
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#define LOGXR(...) __android_log_print(ANDROID_LOG_INFO, "KimoyoOjuXR", __VA_ARGS__)
#define LOGXR_ERR(...) __android_log_print(ANDROID_LOG_ERROR, "KimoyoOjuXR", __VA_ARGS__)
#else
#define LOGXR(...) printf(__VA_ARGS__)
#define LOGXR_ERR(...) fprintf(stderr, __VA_ARGS__)
#endif

namespace kimoyooju {

#ifdef KIMOYOOJU_ENABLE_OPENXR

// ============================================================================
// OpenXR Helper Functions
// ============================================================================

static const char* xrResultToString(XrResult result) {
    switch (result) {
        case XR_SUCCESS: return "XR_SUCCESS";
        case XR_ERROR_VALIDATION_FAILURE: return "XR_ERROR_VALIDATION_FAILURE";
        case XR_ERROR_RUNTIME_FAILURE: return "XR_ERROR_RUNTIME_FAILURE";
        case XR_ERROR_OUT_OF_MEMORY: return "XR_ERROR_OUT_OF_MEMORY";
        case XR_ERROR_API_VERSION_UNSUPPORTED: return "XR_ERROR_API_VERSION_UNSUPPORTED";
        case XR_ERROR_INITIALIZATION_FAILED: return "XR_ERROR_INITIALIZATION_FAILED";
        case XR_ERROR_FUNCTION_UNSUPPORTED: return "XR_ERROR_FUNCTION_UNSUPPORTED";
        case XR_ERROR_FEATURE_UNSUPPORTED: return "XR_ERROR_FEATURE_UNSUPPORTED";
        case XR_ERROR_EXTENSION_NOT_PRESENT: return "XR_ERROR_EXTENSION_NOT_PRESENT";
        case XR_ERROR_LIMIT_REACHED: return "XR_ERROR_LIMIT_REACHED";
        case XR_ERROR_SIZE_INSUFFICIENT: return "XR_ERROR_SIZE_INSUFFICIENT";
        case XR_ERROR_HANDLE_INVALID: return "XR_ERROR_HANDLE_INVALID";
        case XR_ERROR_INSTANCE_LOST: return "XR_ERROR_INSTANCE_LOST";
        case XR_ERROR_SESSION_RUNNING: return "XR_ERROR_SESSION_RUNNING";
        case XR_ERROR_SESSION_NOT_RUNNING: return "XR_ERROR_SESSION_NOT_RUNNING";
        case XR_ERROR_SESSION_LOST: return "XR_ERROR_SESSION_LOST";
        case XR_ERROR_SYSTEM_INVALID: return "XR_ERROR_SYSTEM_INVALID";
        case XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED: return "XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED";
        default: return "UNKNOWN_XR_ERROR";
    }
}

#define XR_CHECK(cmd) do { \
    XrResult result = (cmd); \
    if (XR_FAILED(result)) { \
        LOGXR_ERR("OpenXR error: %s at %s:%d", xrResultToString(result), __FILE__, __LINE__); \
        return false; \
    } \
} while(0)

#define XR_CHECK_VOID(cmd) do { \
    XrResult result = (cmd); \
    if (XR_FAILED(result)) { \
        LOGXR_ERR("OpenXR error: %s at %s:%d", xrResultToString(result), __FILE__, __LINE__); \
        return; \
    } \
} while(0)

// ============================================================================
// XRSession Implementation
// ============================================================================

class XRSessionImpl {
public:
    XrInstance instance = XR_NULL_HANDLE;
    XrSystemId systemId = XR_NULL_SYSTEM_ID;
    XrSession session = XR_NULL_HANDLE;
    XrSpace appSpace = XR_NULL_HANDLE;
    XrSpace viewSpace = XR_NULL_HANDLE;
    
    XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
    bool sessionRunning = false;
    bool exitRequested = false;
    
    std::vector<XrSwapchain> swapchains;
    std::vector<std::vector<XrSwapchainImageOpenGLESKHR>> swapchainImages;
    std::vector<XrView> views;
    std::vector<XrViewConfigurationView> configViews;
    
    XrFrameState frameState = {};
    XrCompositionLayerProjection projectionLayer = {};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    
    int64_t swapchainFormat = 0;
    uint32_t viewCount = 0;
    
    XRSession::Config config;
};

XRSession::XRSession() : impl_(std::make_unique<XRSessionImpl>()) {}

XRSession::~XRSession() {
    shutdown();
}

bool XRSession::initialize(const Config& config, void* androidApp) {
    impl_->config = config;
    
    // Create OpenXR instance
    std::vector<const char*> extensions = {
        XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME,
    };
    
#ifdef __ANDROID__
    extensions.push_back(XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME);
    
    XrInstanceCreateInfoAndroidKHR androidInfo = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    androidInfo.applicationVM = nullptr; // Set from Java
    androidInfo.applicationActivity = androidApp;
#endif
    
    XrInstanceCreateInfo createInfo = {XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.enabledExtensionNames = extensions.data();
    
    strncpy(createInfo.applicationInfo.applicationName, "KimoyoOjuNitro", XR_MAX_APPLICATION_NAME_SIZE);
    createInfo.applicationInfo.applicationVersion = 1;
    strncpy(createInfo.applicationInfo.engineName, "KimoyoOjuEngine", XR_MAX_ENGINE_NAME_SIZE);
    createInfo.applicationInfo.engineVersion = 1;
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
    
#ifdef __ANDROID__
    createInfo.next = &androidInfo;
#endif
    
    XR_CHECK(xrCreateInstance(&createInfo, &impl_->instance));
    LOGXR("OpenXR instance created");
    
    // Get system
    XrSystemGetInfo systemInfo = {XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    
    XR_CHECK(xrGetSystem(impl_->instance, &systemInfo, &impl_->systemId));
    LOGXR("OpenXR system acquired");
    
    // Get view configuration
    uint32_t viewConfigCount = 0;
    XR_CHECK(xrEnumerateViewConfigurations(impl_->instance, impl_->systemId, 0, &viewConfigCount, nullptr));
    
    std::vector<XrViewConfigurationType> viewConfigs(viewConfigCount);
    XR_CHECK(xrEnumerateViewConfigurations(impl_->instance, impl_->systemId, viewConfigCount, &viewConfigCount, viewConfigs.data()));
    
    // Use stereo view for VR
    XrViewConfigurationType viewConfigType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    
    // Get view configuration views (one per eye)
    XR_CHECK(xrEnumerateViewConfigurationViews(impl_->instance, impl_->systemId, viewConfigType, 0, &impl_->viewCount, nullptr));
    impl_->configViews.resize(impl_->viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    XR_CHECK(xrEnumerateViewConfigurationViews(impl_->instance, impl_->systemId, viewConfigType, impl_->viewCount, &impl_->viewCount, impl_->configViews.data()));
    
    impl_->views.resize(impl_->viewCount, {XR_TYPE_VIEW});
    
    LOGXR("OpenXR views: %d, recommended size: %dx%d", 
          impl_->viewCount,
          impl_->configViews[0].recommendedImageRectWidth,
          impl_->configViews[0].recommendedImageRectHeight);
    
    return true;
}

bool XRSession::createSession(void* graphicsBinding) {
    if (!impl_->instance || impl_->systemId == XR_NULL_SYSTEM_ID) {
        LOGXR_ERR("Instance or system not initialized");
        return false;
    }
    
    // Create session with graphics binding
    XrSessionCreateInfo sessionInfo = {XR_TYPE_SESSION_CREATE_INFO};
    sessionInfo.systemId = impl_->systemId;
    sessionInfo.next = graphicsBinding;
    
    XR_CHECK(xrCreateSession(impl_->instance, &sessionInfo, &impl_->session));
    LOGXR("OpenXR session created");
    
    // Create reference spaces
    XrReferenceSpaceCreateInfo spaceInfo = {XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    spaceInfo.poseInReferenceSpace.orientation.w = 1.0f;
    
    XR_CHECK(xrCreateReferenceSpace(impl_->session, &spaceInfo, &impl_->appSpace));
    
    spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    XR_CHECK(xrCreateReferenceSpace(impl_->session, &spaceInfo, &impl_->viewSpace));
    
    LOGXR("OpenXR reference spaces created");
    
    return true;
}

bool XRSession::createSwapchains() {
    if (!impl_->session) {
        LOGXR_ERR("Session not created");
        return false;
    }
    
    // Get swapchain formats
    uint32_t formatCount = 0;
    XR_CHECK(xrEnumerateSwapchainFormats(impl_->session, 0, &formatCount, nullptr));
    
    std::vector<int64_t> formats(formatCount);
    XR_CHECK(xrEnumerateSwapchainFormats(impl_->session, formatCount, &formatCount, formats.data()));
    
    // Prefer SRGB8_ALPHA8
    impl_->swapchainFormat = formats[0]; // Use first available
    for (int64_t format : formats) {
        if (format == 0x8C43) { // GL_SRGB8_ALPHA8
            impl_->swapchainFormat = format;
            break;
        }
    }
    
    LOGXR("Using swapchain format: 0x%llx", (unsigned long long)impl_->swapchainFormat);
    
    // Create swapchains for each view
    impl_->swapchains.resize(impl_->viewCount);
    impl_->swapchainImages.resize(impl_->viewCount);
    impl_->projectionLayerViews.resize(impl_->viewCount);
    
    for (uint32_t i = 0; i < impl_->viewCount; ++i) {
        XrSwapchainCreateInfo swapchainInfo = {XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
        swapchainInfo.format = impl_->swapchainFormat;
        swapchainInfo.sampleCount = 1;
        swapchainInfo.width = impl_->configViews[i].recommendedImageRectWidth;
        swapchainInfo.height = impl_->configViews[i].recommendedImageRectHeight;
        swapchainInfo.faceCount = 1;
        swapchainInfo.arraySize = 1;
        swapchainInfo.mipCount = 1;
        
        XR_CHECK(xrCreateSwapchain(impl_->session, &swapchainInfo, &impl_->swapchains[i]));
        
        // Get swapchain images
        uint32_t imageCount = 0;
        XR_CHECK(xrEnumerateSwapchainImages(impl_->swapchains[i], 0, &imageCount, nullptr));
        
        impl_->swapchainImages[i].resize(imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});
        XR_CHECK(xrEnumerateSwapchainImages(impl_->swapchains[i], imageCount, &imageCount,
            reinterpret_cast<XrSwapchainImageBaseHeader*>(impl_->swapchainImages[i].data())));
        
        LOGXR("Swapchain %d: %dx%d, %d images", i, swapchainInfo.width, swapchainInfo.height, imageCount);
        
        // Initialize projection layer view
        impl_->projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        impl_->projectionLayerViews[i].subImage.swapchain = impl_->swapchains[i];
        impl_->projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        impl_->projectionLayerViews[i].subImage.imageRect.extent = {
            static_cast<int32_t>(swapchainInfo.width),
            static_cast<int32_t>(swapchainInfo.height)
        };
    }
    
    // Initialize projection layer
    impl_->projectionLayer = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    impl_->projectionLayer.space = impl_->appSpace;
    impl_->projectionLayer.viewCount = impl_->viewCount;
    impl_->projectionLayer.views = impl_->projectionLayerViews.data();
    
    return true;
}

void XRSession::shutdown() {
    // Destroy swapchains
    for (auto& swapchain : impl_->swapchains) {
        if (swapchain != XR_NULL_HANDLE) {
            xrDestroySwapchain(swapchain);
        }
    }
    impl_->swapchains.clear();
    impl_->swapchainImages.clear();
    
    // Destroy spaces
    if (impl_->appSpace != XR_NULL_HANDLE) {
        xrDestroySpace(impl_->appSpace);
        impl_->appSpace = XR_NULL_HANDLE;
    }
    
    if (impl_->viewSpace != XR_NULL_HANDLE) {
        xrDestroySpace(impl_->viewSpace);
        impl_->viewSpace = XR_NULL_HANDLE;
    }
    
    // Destroy session
    if (impl_->session != XR_NULL_HANDLE) {
        xrDestroySession(impl_->session);
        impl_->session = XR_NULL_HANDLE;
    }
    
    // Destroy instance
    if (impl_->instance != XR_NULL_HANDLE) {
        xrDestroyInstance(impl_->instance);
        impl_->instance = XR_NULL_HANDLE;
    }
    
    impl_->sessionRunning = false;
    impl_->sessionState = XR_SESSION_STATE_UNKNOWN;
    
    LOGXR("OpenXR session shutdown complete");
}

bool XRSession::pollEvents() {
    XrEventDataBuffer event = {XR_TYPE_EVENT_DATA_BUFFER};
    
    while (xrPollEvent(impl_->instance, &event) == XR_SUCCESS) {
        switch (event.type) {
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                auto* stateEvent = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
                impl_->sessionState = stateEvent->state;
                
                LOGXR("Session state changed: %d", impl_->sessionState);
                
                if (impl_->sessionState == XR_SESSION_STATE_READY) {
                    XrSessionBeginInfo beginInfo = {XR_TYPE_SESSION_BEGIN_INFO};
                    beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
                    
                    if (XR_SUCCEEDED(xrBeginSession(impl_->session, &beginInfo))) {
                        impl_->sessionRunning = true;
                        LOGXR("Session started");
                    }
                } else if (impl_->sessionState == XR_SESSION_STATE_STOPPING) {
                    impl_->sessionRunning = false;
                    xrEndSession(impl_->session);
                    LOGXR("Session stopped");
                } else if (impl_->sessionState == XR_SESSION_STATE_EXITING ||
                           impl_->sessionState == XR_SESSION_STATE_LOSS_PENDING) {
                    impl_->exitRequested = true;
                }
                break;
            }
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                impl_->exitRequested = true;
                LOGXR("Instance loss pending");
                break;
            }
            default:
                break;
        }
        
        event = {XR_TYPE_EVENT_DATA_BUFFER};
    }
    
    return !impl_->exitRequested;
}

bool XRSession::beginFrame() {
    if (!impl_->sessionRunning) {
        return false;
    }
    
    impl_->frameState = {XR_TYPE_FRAME_STATE};
    XrFrameWaitInfo waitInfo = {XR_TYPE_FRAME_WAIT_INFO};
    
    XrResult result = xrWaitFrame(impl_->session, &waitInfo, &impl_->frameState);
    if (XR_FAILED(result)) {
        LOGXR_ERR("xrWaitFrame failed: %s", xrResultToString(result));
        return false;
    }
    
    XrFrameBeginInfo beginInfo = {XR_TYPE_FRAME_BEGIN_INFO};
    result = xrBeginFrame(impl_->session, &beginInfo);
    if (XR_FAILED(result)) {
        LOGXR_ERR("xrBeginFrame failed: %s", xrResultToString(result));
        return false;
    }
    
    return true;
}

bool XRSession::acquireSwapchainImage(uint32_t viewIndex, uint32_t& imageIndex) {
    if (viewIndex >= impl_->swapchains.size()) {
        return false;
    }
    
    XrSwapchainImageAcquireInfo acquireInfo = {XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
    XrResult result = xrAcquireSwapchainImage(impl_->swapchains[viewIndex], &acquireInfo, &imageIndex);
    if (XR_FAILED(result)) {
        LOGXR_ERR("xrAcquireSwapchainImage failed");
        return false;
    }
    
    XrSwapchainImageWaitInfo waitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
    waitInfo.timeout = XR_INFINITE_DURATION;
    
    result = xrWaitSwapchainImage(impl_->swapchains[viewIndex], &waitInfo);
    if (XR_FAILED(result)) {
        LOGXR_ERR("xrWaitSwapchainImage failed");
        return false;
    }
    
    return true;
}

void XRSession::releaseSwapchainImage(uint32_t viewIndex) {
    if (viewIndex >= impl_->swapchains.size()) {
        return;
    }
    
    XrSwapchainImageReleaseInfo releaseInfo = {XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
    xrReleaseSwapchainImage(impl_->swapchains[viewIndex], &releaseInfo);
}

bool XRSession::endFrame() {
    if (!impl_->sessionRunning) {
        return false;
    }
    
    // Locate views
    XrViewLocateInfo locateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
    locateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    locateInfo.displayTime = impl_->frameState.predictedDisplayTime;
    locateInfo.space = impl_->appSpace;
    
    XrViewState viewState = {XR_TYPE_VIEW_STATE};
    uint32_t viewCountOutput = 0;
    
    XrResult result = xrLocateViews(impl_->session, &locateInfo, &viewState, 
                                    impl_->viewCount, &viewCountOutput, impl_->views.data());
    
    std::vector<const XrCompositionLayerBaseHeader*> layers;
    
    if (impl_->frameState.shouldRender && XR_SUCCEEDED(result)) {
        // Update projection layer views with current poses
        for (uint32_t i = 0; i < impl_->viewCount; ++i) {
            impl_->projectionLayerViews[i].pose = impl_->views[i].pose;
            impl_->projectionLayerViews[i].fov = impl_->views[i].fov;
        }
        
        layers.push_back(reinterpret_cast<const XrCompositionLayerBaseHeader*>(&impl_->projectionLayer));
    }
    
    XrFrameEndInfo endInfo = {XR_TYPE_FRAME_END_INFO};
    endInfo.displayTime = impl_->frameState.predictedDisplayTime;
    endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    endInfo.layerCount = static_cast<uint32_t>(layers.size());
    endInfo.layers = layers.data();
    
    result = xrEndFrame(impl_->session, &endInfo);
    if (XR_FAILED(result)) {
        LOGXR_ERR("xrEndFrame failed: %s", xrResultToString(result));
        return false;
    }
    
    return true;
}

XRSessionState XRSession::getState() const {
    switch (impl_->sessionState) {
        case XR_SESSION_STATE_IDLE: return XRSessionState::IDLE;
        case XR_SESSION_STATE_READY: return XRSessionState::READY;
        case XR_SESSION_STATE_SYNCHRONIZED: return XRSessionState::SYNCHRONIZED;
        case XR_SESSION_STATE_VISIBLE: return XRSessionState::VISIBLE;
        case XR_SESSION_STATE_FOCUSED: return XRSessionState::FOCUSED;
        case XR_SESSION_STATE_STOPPING: return XRSessionState::STOPPING;
        case XR_SESSION_STATE_LOSS_PENDING: return XRSessionState::LOSS_PENDING;
        case XR_SESSION_STATE_EXITING: return XRSessionState::EXITING;
        default: return XRSessionState::IDLE;
    }
}

bool XRSession::isRunning() const {
    return impl_->sessionRunning;
}

bool XRSession::shouldRender() const {
    return impl_->frameState.shouldRender;
}

uint32_t XRSession::getViewCount() const {
    return impl_->viewCount;
}

uint32_t XRSession::getSwapchainImage(uint32_t viewIndex, uint32_t imageIndex) const {
    if (viewIndex < impl_->swapchainImages.size() && 
        imageIndex < impl_->swapchainImages[viewIndex].size()) {
        return impl_->swapchainImages[viewIndex][imageIndex].image;
    }
    return 0;
}

void XRSession::getViewProjection(uint32_t viewIndex, float* viewMatrix, float* projMatrix) const {
    if (viewIndex >= impl_->viewCount) return;
    
    const XrView& view = impl_->views[viewIndex];
    
    // Convert XrPosef to view matrix
    // This is a simplified version - proper implementation would use quaternion math
    XrPosef pose = view.pose;
    
    // View matrix (inverse of pose)
    // For now, just set identity with translation
    float* v = viewMatrix;
    v[0] = 1; v[1] = 0; v[2] = 0; v[3] = 0;
    v[4] = 0; v[5] = 1; v[6] = 0; v[7] = 0;
    v[8] = 0; v[9] = 0; v[10] = 1; v[11] = 0;
    v[12] = -pose.position.x;
    v[13] = -pose.position.y;
    v[14] = -pose.position.z;
    v[15] = 1;
    
    // Projection matrix from FOV
    const XrFovf& fov = view.fov;
    float tanLeft = tanf(fov.angleLeft);
    float tanRight = tanf(fov.angleRight);
    float tanUp = tanf(fov.angleUp);
    float tanDown = tanf(fov.angleDown);
    
    float tanWidth = tanRight - tanLeft;
    float tanHeight = tanUp - tanDown;
    
    float nearZ = 0.1f;
    float farZ = 1000.0f;
    
    float* p = projMatrix;
    p[0] = 2.0f / tanWidth;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    
    p[4] = 0;
    p[5] = 2.0f / tanHeight;
    p[6] = 0;
    p[7] = 0;
    
    p[8] = (tanRight + tanLeft) / tanWidth;
    p[9] = (tanUp + tanDown) / tanHeight;
    p[10] = -(farZ + nearZ) / (farZ - nearZ);
    p[11] = -1;
    
    p[12] = 0;
    p[13] = 0;
    p[14] = -2.0f * farZ * nearZ / (farZ - nearZ);
    p[15] = 0;
}

void XRSession::getSwapchainSize(uint32_t viewIndex, uint32_t& width, uint32_t& height) const {
    if (viewIndex < impl_->configViews.size()) {
        width = impl_->configViews[viewIndex].recommendedImageRectWidth;
        height = impl_->configViews[viewIndex].recommendedImageRectHeight;
    }
}

#else // !KIMOYOOJU_ENABLE_OPENXR

// Stub implementation when OpenXR is not available
class XRSessionImpl {};

XRSession::XRSession() : impl_(std::make_unique<XRSessionImpl>()) {}
XRSession::~XRSession() {}
bool XRSession::initialize(const Config&, void*) { return false; }
bool XRSession::createSession(void*) { return false; }
bool XRSession::createSwapchains() { return false; }
void XRSession::shutdown() {}
bool XRSession::pollEvents() { return false; }
bool XRSession::beginFrame() { return false; }
bool XRSession::acquireSwapchainImage(uint32_t, uint32_t&) { return false; }
void XRSession::releaseSwapchainImage(uint32_t) {}
bool XRSession::endFrame() { return false; }
XRSessionState XRSession::getState() const { return XRSessionState::IDLE; }
bool XRSession::isRunning() const { return false; }
bool XRSession::shouldRender() const { return false; }
uint32_t XRSession::getViewCount() const { return 0; }
uint32_t XRSession::getSwapchainImage(uint32_t, uint32_t) const { return 0; }
void XRSession::getViewProjection(uint32_t, float*, float*) const {}
void XRSession::getSwapchainSize(uint32_t, uint32_t&, uint32_t&) const {}

#endif // KIMOYOOJU_ENABLE_OPENXR

} // namespace kimoyooju

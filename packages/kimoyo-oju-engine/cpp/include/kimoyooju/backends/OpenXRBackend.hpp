#pragma once

/**
 * OpenXR Backend - Runtime-Agnostic XR Implementation
 * 
 * IMPORTANT: This backend uses OpenXR EXCLUSIVELY.
 * 
 * Supported Runtimes (via OpenXR abstraction):
 * - Windows Mixed Reality (WMR OpenXR)
 * - SteamVR (SteamVR OpenXR)
 * - PICO (PICO OpenXR, PC streaming)
 * - Meta Quest (Quest OpenXR)
 * - Android XR devices
 * 
 * FORBIDDEN:
 * - OpenVR API
 * - SteamVR SDK APIs
 * - Vendor-specific rendering paths
 * - Per-device renderer forks
 * - Hard-coded controller models
 * 
 * Graphics API Selection:
 * - Windows: D3D11 via XR_KHR_D3D11_enable
 * - Android: Vulkan via XR_KHR_vulkan_enable2 or OpenGL ES via XR_KHR_opengl_es_enable
 * - Linux: Vulkan via XR_KHR_vulkan_enable2
 */

#include <openxr/openxr.h>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <functional>
#include <unordered_map>
#include "kimoyooju/IXRBackend.hpp"

#ifdef _WIN32
#include <d3d11.h>
#endif

namespace kimoyooju {

// ============================================================================
// Configuration
// ============================================================================

enum class OpenXRGraphicsAPI {
    D3D11,      // Windows (XR_KHR_D3D11_enable)
    Vulkan,     // Cross-platform (XR_KHR_vulkan_enable2)
    OpenGLES    // Android/Quest (XR_KHR_opengl_es_enable)
};

struct OpenXRConfig {
    // Form factor
    XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    
    // View configuration (stereo for HMDs)
    XrViewConfigurationType viewConfigType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    
    // Environment blend mode
    XrEnvironmentBlendMode preferredBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    
    // Graphics API (auto-detected if not specified)
    std::optional<OpenXRGraphicsAPI> graphicsAPI;
    
    // Optional extensions to request
    bool requestHandTracking = true;
    bool requestPassthrough = false;
    bool requestEyeTracking = false;
    
    // Application info
    std::string applicationName = "KimoyoOju";
    uint32_t applicationVersion = 1;
};

// ============================================================================
// OpenXR Actions Input System
// ============================================================================

/**
 * Action-based input following OpenXR spec.
 * NO hard-coded controller models or layouts.
 * All input via interaction profiles.
 */
struct OpenXRAction {
    XrAction action = XR_NULL_HANDLE;
    XrActionType type;
    std::string name;
    std::string localizedName;
};

struct OpenXRActionSet {
    XrActionSet actionSet = XR_NULL_HANDLE;
    std::string name;
    std::string localizedName;
    uint32_t priority = 0;
    std::vector<OpenXRAction> actions;
};

// Standard action paths (runtime-agnostic)
namespace ActionPaths {
    constexpr const char* LEFT_HAND = "/user/hand/left";
    constexpr const char* RIGHT_HAND = "/user/hand/right";
    constexpr const char* HEAD = "/user/head";
    constexpr const char* GAMEPAD = "/user/gamepad";
}

// Interaction profiles (bound at runtime, not hard-coded)
namespace InteractionProfiles {
    constexpr const char* KHR_SIMPLE = "/interaction_profiles/khr/simple_controller";
    constexpr const char* OCULUS_TOUCH = "/interaction_profiles/oculus/touch_controller";
    constexpr const char* VALVE_INDEX = "/interaction_profiles/valve/index_controller";
    constexpr const char* HTC_VIVE = "/interaction_profiles/htc/vive_controller";
    constexpr const char* MS_MOTION = "/interaction_profiles/microsoft/motion_controller";
    constexpr const char* PICO_NEO3 = "/interaction_profiles/bytedance/pico_neo3_controller";
    constexpr const char* PICO_4 = "/interaction_profiles/bytedance/pico4_controller";
}

// ============================================================================
// OpenXR Backend Class
// ============================================================================

class OpenXRBackend : public IXRBackend {
public:
    explicit OpenXRBackend(const OpenXRConfig& config = {});
    ~OpenXRBackend() override;
    
    // ========================================================================
    // IXRBackend Interface
    // ========================================================================
    
    bool initialize(void* platformData1, void* platformData2) override;
    void shutdown() override;
    bool resume() override;
    void pause() override;
    
    bool beginFrame(XRFrameData& outFrameData) override;
    void endFrame() override;
    
    bool isSessionRunning() const override { return sessionRunning_; }
    XRMode getCurrentMode() const override;
    TrackingState getTrackingState() const override;
    
    // Hand tracking (via XR_EXT_hand_tracking if available)
    bool isHandTrackingSupported() const override;
    std::optional<HandData> getHandData(Hand hand) const override;
    
    // Controllers (via OpenXR Actions)
    bool areControllersSupported() const override { return true; }
    std::optional<XRControllerState> getControllerState(Hand hand) const override;
    
    // Passthrough (via runtime extension if available)
    bool isPassthroughSupported() const override;
    bool enablePassthrough() override;
    void disablePassthrough() override;
    
    XRCapabilities getCapabilities() const override;
    
    // ========================================================================
    // OpenXR-Specific API
    // ========================================================================
    
    // Instance/Session access for graphics integration
    XrInstance getInstance() const { return instance_; }
    XrSession getSession() const { return session_; }
    XrSystemId getSystemId() const { return systemId_; }
    
    // Reference space
    XrSpace getLocalSpace() const { return localSpace_; }
    XrSpace getViewSpace() const { return viewSpace_; }
    XrSpace getStageSpace() const { return stageSpace_; }
    
    // Runtime info (for debugging only, NOT for branching logic)
    std::string getRuntimeName() const { return runtimeName_; }
    XrVersion getRuntimeVersion() const { return runtimeVersion_; }
    
    // Swapchain info for renderer integration
    struct SwapchainInfo {
        XrSwapchain swapchain = XR_NULL_HANDLE;
        int64_t format = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t sampleCount = 1;
        uint32_t arraySize = 1;
        
#ifdef _WIN32
        std::vector<ID3D11Texture2D*> d3d11Images;
#endif
        std::vector<uint32_t> glImages;  // For OpenGL ES
        // Vulkan images handled separately
    };
    
    const std::vector<SwapchainInfo>& getSwapchains() const { return swapchains_; }
    uint32_t getAcquiredImageIndex(uint32_t viewIndex) const;
    
#ifdef _WIN32
    // D3D11 integration
    ID3D11Device* getD3D11Device() const { return d3d11Device_; }
    ID3D11DeviceContext* getD3D11Context() const { return d3d11Context_; }
#endif
    
    // ========================================================================
    // Input System (OpenXR Actions)
    // ========================================================================
    
    // Create action set
    bool createActionSet(const std::string& name, const std::string& localizedName, 
                         uint32_t priority = 0);
    
    // Create actions
    bool createBoolAction(const std::string& actionSetName, const std::string& name,
                          const std::string& localizedName,
                          const std::vector<std::string>& subactionPaths = {});
    
    bool createFloatAction(const std::string& actionSetName, const std::string& name,
                           const std::string& localizedName,
                           const std::vector<std::string>& subactionPaths = {});
    
    bool createVector2Action(const std::string& actionSetName, const std::string& name,
                             const std::string& localizedName,
                             const std::vector<std::string>& subactionPaths = {});
    
    bool createPoseAction(const std::string& actionSetName, const std::string& name,
                          const std::string& localizedName,
                          const std::vector<std::string>& subactionPaths = {});
    
    bool createVibrationAction(const std::string& actionSetName, const std::string& name,
                               const std::string& localizedName,
                               const std::vector<std::string>& subactionPaths = {});
    
    // Suggest bindings for interaction profiles
    bool suggestBindings(const std::string& interactionProfile,
                         const std::vector<std::pair<std::string, std::string>>& bindings);
    
    // Attach action sets to session
    bool attachActionSets();
    
    // Sync actions (call each frame)
    bool syncActions();
    
    // Query action states
    bool getBoolActionState(const std::string& actionName, bool& outValue,
                            const std::string& subactionPath = "") const;
    
    bool getFloatActionState(const std::string& actionName, float& outValue,
                             const std::string& subactionPath = "") const;
    
    bool getVector2ActionState(const std::string& actionName, float& outX, float& outY,
                               const std::string& subactionPath = "") const;
    
    bool getPoseActionState(const std::string& actionName, Pose& outPose,
                            const std::string& subactionPath = "") const;
    
    // Haptics
    bool applyHapticFeedback(const std::string& actionName, float amplitude,
                             float duration, float frequency,
                             const std::string& subactionPath = "");
    
    bool stopHapticFeedback(const std::string& actionName,
                            const std::string& subactionPath = "");
    
private:
    // ========================================================================
    // Initialization
    // ========================================================================
    
    void queryExtensions();
    void createInstance();
    void getInstanceProperties();
    void getSystemId();
    void enumerateViewConfigurations();
    void createGraphicsBinding(void* platformData);
    void createSession();
    void createReferenceSpaces();
    void createSwapchains();
    void createDefaultActions();
    void initHandTracking();
    
    // ========================================================================
    // Frame Loop (follows OpenXR spec exactly)
    // ========================================================================
    
    /**
     * OpenXR Frame Loop - MUST follow this order:
     * 1. xrWaitFrame
     * 2. xrBeginFrame
     * 3. xrLocateViews
     * 4. Acquire swapchain images
     * 5. Render stereo views (left/right)
     * 6. Release swapchain images
     * 7. xrEndFrame
     */
    bool waitFrame();
    bool beginFrameInternal();
    void locateViews(XrTime predictedTime);
    bool acquireSwapchainImages();
    void releaseSwapchainImages();
    bool endFrameInternal(const std::vector<XrCompositionLayerBaseHeader*>& layers);
    
    // ========================================================================
    // Event Handling
    // ========================================================================
    
    void pollEvents();
    void handleSessionStateChange(XrSessionState newState);
    void handleInstanceLossPending();
    void handleInteractionProfileChanged();
    
    // ========================================================================
    // Cleanup
    // ========================================================================
    
    void destroySwapchains();
    void destroySession();
    void destroyInstance();
    
    // ========================================================================
    // Extension Helpers
    // ========================================================================
    
    bool isExtensionSupported(const char* extensionName) const;
    bool loadExtensionFunctions();
    
    // ========================================================================
    // Member Variables
    // ========================================================================
    
    OpenXRConfig config_;
    
    // OpenXR core handles
    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId systemId_ = XR_NULL_SYSTEM_ID;
    XrSession session_ = XR_NULL_HANDLE;
    
    // Reference spaces
    XrSpace localSpace_ = XR_NULL_HANDLE;
    XrSpace viewSpace_ = XR_NULL_HANDLE;
    XrSpace stageSpace_ = XR_NULL_HANDLE;
    
    // Runtime info (for debugging, NOT for branching)
    std::string runtimeName_;
    XrVersion runtimeVersion_ = 0;
    
    // Supported extensions
    std::vector<XrExtensionProperties> availableExtensions_;
    std::vector<const char*> enabledExtensions_;
    
    // View configuration
    std::vector<XrViewConfigurationView> viewConfigViews_;
    uint32_t viewCount_ = 0;
    XrEnvironmentBlendMode blendMode_ = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    
    // Swapchains (one per view for stereo)
    std::vector<SwapchainInfo> swapchains_;
    std::vector<uint32_t> acquiredImageIndices_;
    std::vector<bool> imagesAcquired_;
    
    // Frame state
    XrFrameState frameState_ = {XR_TYPE_FRAME_STATE};
    bool frameWaited_ = false;
    bool frameBegun_ = false;
    std::vector<XrView> views_;
    std::vector<XrCompositionLayerProjectionView> projectionViews_;
    XrCompositionLayerProjection projectionLayer_ = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    
    // Session state
    XrSessionState sessionState_ = XR_SESSION_STATE_UNKNOWN;
    bool sessionRunning_ = false;
    bool exitRequested_ = false;
    
    // Actions system
    std::unordered_map<std::string, OpenXRActionSet> actionSets_;
    std::unordered_map<std::string, XrAction> actionsByName_;
    std::unordered_map<std::string, XrSpace> actionSpaces_;
    XrPath handPaths_[2] = {};
    bool actionsAttached_ = false;
    
    // Hand tracking (XR_EXT_hand_tracking)
    bool handTrackingSupported_ = false;
    XrHandTrackerEXT handTrackers_[2] = {XR_NULL_HANDLE, XR_NULL_HANDLE};
    
    // Passthrough
    bool passthroughSupported_ = false;
    bool passthroughEnabled_ = false;
    
#ifdef _WIN32
    // D3D11 graphics binding
    ID3D11Device* d3d11Device_ = nullptr;
    ID3D11DeviceContext* d3d11Context_ = nullptr;
    XrGraphicsBindingD3D11KHR d3d11Binding_ = {XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
#endif
    
    // Extension function pointers
    PFN_xrGetD3D11GraphicsRequirementsKHR xrGetD3D11GraphicsRequirementsKHR_ = nullptr;
    PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT_ = nullptr;
    PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT_ = nullptr;
    PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT_ = nullptr;
};

// ============================================================================
// Helper: Convert XrResult to string
// ============================================================================

const char* xrResultToString(XrResult result);

// ============================================================================
// Macros for error checking
// ============================================================================

#define XR_CHECK(result, msg) \
    do { \
        XrResult _res = (result); \
        if (XR_FAILED(_res)) { \
            KIMOYOOJU_LOG_ERROR("OpenXR error: {} - {} ({})", msg, xrResultToString(_res), static_cast<int>(_res)); \
            return false; \
        } \
    } while(0)

#define XR_CHECK_VOID(result, msg) \
    do { \
        XrResult _res = (result); \
        if (XR_FAILED(_res)) { \
            KIMOYOOJU_LOG_ERROR("OpenXR error: {} - {} ({})", msg, xrResultToString(_res), static_cast<int>(_res)); \
            return; \
        } \
    } while(0)

} // namespace kimoyooju

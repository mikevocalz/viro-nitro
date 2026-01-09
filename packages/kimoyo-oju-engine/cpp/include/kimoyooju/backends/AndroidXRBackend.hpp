#pragma once

#include <openxr/openxr.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <map>
#include "kimoyooju/IXRBackend.hpp"

namespace kimoyooju {

struct AndroidXRConfig {
    bool enableHandTracking = true;
    bool enablePassthrough = true;
    bool enableSceneUnderstanding = false;
    bool enableEyeTracking = false;
    XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrViewConfigurationType viewConfig = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    XrEnvironmentBlendMode blendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
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
    
    bool isSessionRunning() const override { return sessionRunning_; }
    XRMode getCurrentMode() const override;
    TrackingState getTrackingState() const override;
    
    // Hand tracking
    bool isHandTrackingSupported() const override;
    std::optional<HandData> getHandData(Hand hand) const override;
    
    // Controllers
    bool areControllersSupported() const override { return true; }
    std::optional<XRControllerState> getControllerState(Hand hand) const override;
    
    // Passthrough
    bool isPassthroughSupported() const override;
    bool enablePassthrough() override;
    void disablePassthrough() override;
    
    // Capabilities
    XRCapabilities getCapabilities() const override;
    
    // Vulkan integration - for VulkanBackend to use
    VkInstance getVulkanInstance() const { return vkInstance_; }
    VkPhysicalDevice getVulkanPhysicalDevice() const { return vkPhysicalDevice_; }
    VkDevice getVulkanDevice() const { return vkDevice_; }
    uint32_t getVulkanQueueFamilyIndex() const { return vkQueueFamilyIndex_; }
    uint32_t getVulkanQueueIndex() const { return vkQueueIndex_; }
    
    // Swapchain access for rendering
    struct SwapchainInfo {
        XrSwapchain swapchain;
        VkFormat format;
        uint32_t width;
        uint32_t height;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
    };
    const std::vector<SwapchainInfo>& getSwapchains() const { return swapchains_; }
    uint32_t getCurrentSwapchainImageIndex(uint32_t viewIndex) const;
    
private:
    // Initialization
    void createInstance(void* activity);
    void getSystemId();
    void enumerateViewConfigurations();
    void createVulkanInstance();
    void selectVulkanPhysicalDevice();
    void createVulkanDevice();
    void createSession();
    void createReferenceSpaces();
    void createSwapchains();
    void initHandTracking();
    void initPassthrough();
    void initControllers();
    
    // Event handling
    void pollEvents();
    void handleSessionStateChange(XrSessionState newState);
    
    // Frame helpers
    void locateViews(XrTime predictedTime, std::vector<XRView>& outViews);
    void acquireSwapchainImages();
    void releaseSwapchainImages();
    
    // Hand tracking helpers
    void updateHandTracking();
    HandData convertHandJoints(XrHandTrackerEXT tracker) const;
    
    // Controller helpers
    void updateControllers();
    XRControllerState getControllerStateInternal(bool isLeft) const;
    
    // Cleanup
    void destroySwapchains();
    void destroySession();
    
    // Configuration
    AndroidXRConfig config_;
    
    // OpenXR handles
    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId systemId_ = XR_NULL_SYSTEM_ID;
    XrSession session_ = XR_NULL_HANDLE;
    
    // Reference spaces
    XrSpace localSpace_ = XR_NULL_HANDLE;
    XrSpace viewSpace_ = XR_NULL_HANDLE;
    XrSpace stageSpace_ = XR_NULL_HANDLE;
    
    // View configuration
    std::vector<XrViewConfigurationView> viewConfigViews_;
    uint32_t viewCount_ = 0;
    
    // Swapchains
    std::vector<SwapchainInfo> swapchains_;
    std::vector<uint32_t> swapchainImageIndices_;
    
    // Vulkan handles (created via OpenXR requirements)
    VkInstance vkInstance_ = VK_NULL_HANDLE;
    VkPhysicalDevice vkPhysicalDevice_ = VK_NULL_HANDLE;
    VkDevice vkDevice_ = VK_NULL_HANDLE;
    uint32_t vkQueueFamilyIndex_ = 0;
    uint32_t vkQueueIndex_ = 0;
    
    // Hand tracking (XR_EXT_hand_tracking)
    bool handTrackingSupported_ = false;
    XrHandTrackerEXT handTrackers_[2] = {XR_NULL_HANDLE, XR_NULL_HANDLE};
    mutable HandData cachedHandData_[2];
    
    // Passthrough (XR_FB_passthrough or platform equivalent)
    bool passthroughSupported_ = false;
    bool passthroughEnabled_ = false;
    XrPassthroughFB passthrough_ = XR_NULL_HANDLE;
    XrPassthroughLayerFB passthroughLayer_ = XR_NULL_HANDLE;
    
    // Controllers
    XrActionSet actionSet_ = XR_NULL_HANDLE;
    XrAction poseAction_ = XR_NULL_HANDLE;
    XrAction triggerAction_ = XR_NULL_HANDLE;
    XrAction gripAction_ = XR_NULL_HANDLE;
    XrAction thumbstickAction_ = XR_NULL_HANDLE;
    XrAction primaryButtonAction_ = XR_NULL_HANDLE;
    XrAction secondaryButtonAction_ = XR_NULL_HANDLE;
    XrPath handPaths_[2] = {};
    XrSpace handSpaces_[2] = {XR_NULL_HANDLE, XR_NULL_HANDLE};
    mutable XRControllerState cachedControllerState_[2];
    
    // Frame state
    XrFrameState frameState_ = {XR_TYPE_FRAME_STATE};
    bool frameActive_ = false;
    std::vector<XrView> views_;
    std::vector<XrCompositionLayerProjectionView> projectionViews_;
    XrCompositionLayerProjection projectionLayer_ = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    
    // Session state
    XrSessionState sessionState_ = XR_SESSION_STATE_UNKNOWN;
    bool sessionRunning_ = false;
    bool exitRequested_ = false;
    
    // Extension function pointers
    PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT_ = nullptr;
    PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT_ = nullptr;
    PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT_ = nullptr;
    PFN_xrCreatePassthroughFB xrCreatePassthroughFB_ = nullptr;
    PFN_xrDestroyPassthroughFB xrDestroyPassthroughFB_ = nullptr;
    PFN_xrPassthroughStartFB xrPassthroughStartFB_ = nullptr;
    PFN_xrPassthroughPauseFB xrPassthroughPauseFB_ = nullptr;
    PFN_xrCreatePassthroughLayerFB xrCreatePassthroughLayerFB_ = nullptr;
    PFN_xrDestroyPassthroughLayerFB xrDestroyPassthroughLayerFB_ = nullptr;
};

} // namespace kimoyooju

#pragma once

#include "kimoyooju/Types.hpp"
#include "kimoyooju/MemoryCounters.hpp"

#include <memory>
#include <vector>
#include <functional>
#include <atomic>

#if defined(__ANDROID__)
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#else
// Stub types for non-OpenXR platforms
using XrInstance = void*;
using XrSession = void*;
using XrSpace = void*;
using XrSwapchain = void*;
using XrTime = int64_t;
using XrResult = int32_t;
using XrSessionState = int32_t;
constexpr XrResult XR_SUCCESS = 0;
#define XR_NULL_HANDLE nullptr
#define XR_FAILED(x) ((x) < 0)
#endif

namespace kimoyooju {

template<typename T, auto DestroyFn>
class XrHandle {
    T handle_ = XR_NULL_HANDLE;
public:
    XrHandle() = default;
    explicit XrHandle(T h) : handle_(h) {}
    ~XrHandle() { reset(); }
    
    XrHandle(XrHandle&& o) noexcept 
        : handle_(std::exchange(o.handle_, static_cast<T>(XR_NULL_HANDLE))) {}
    
    XrHandle& operator=(XrHandle&& o) noexcept {
        if (this != &o) { 
            reset(); 
            handle_ = std::exchange(o.handle_, static_cast<T>(XR_NULL_HANDLE)); 
        }
        return *this;
    }
    
    XrHandle(const XrHandle&) = delete;
    XrHandle& operator=(const XrHandle&) = delete;
    
    void reset() {
        if (handle_ != XR_NULL_HANDLE) {
            DestroyFn(handle_);
            handle_ = static_cast<T>(XR_NULL_HANDLE);
        }
    }
    
    T get() const { return handle_; }
    T release() { return std::exchange(handle_, static_cast<T>(XR_NULL_HANDLE)); }
    explicit operator bool() const { return handle_ != XR_NULL_HANDLE; }
    T* put() { reset(); return &handle_; }
};

struct SwapchainImage {
    uint32_t imageId;
    bool acquired = false;
};

class XRSession {
public:
    struct Config {
        XRMode mode = XRMode::IMMERSIVE_VR;
        bool enablePassthrough = false;
        bool enableFoveation = true;
    };
    
    using SessionStateCallback = std::function<void(XRSessionState)>;
    
    static std::unique_ptr<XRSession> create(const Config& config);
    ~XRSession();
    
    XRSession(const XRSession&) = delete;
    XRSession& operator=(const XRSession&) = delete;
    
    Result<void> initialize(void* graphicsBinding);
    Result<void> begin();
    Result<void> end();
    void destroy();
    
    bool isSessionRunning() const;
    XRSessionState getSessionState() const;
    
    void setSessionStateCallback(SessionStateCallback callback);
    
    Result<void> waitFrame();
    Result<void> beginFrame();
    Result<void> endFrame();
    
    bool shouldRender() const;
    XrTime getPredictedDisplayTime() const;
    
    struct ViewInfo {
        Mat4 viewMatrix;
        Mat4 projectionMatrix;
        uint32_t swapchainIndex;
    };
    
    std::vector<ViewInfo> getViews() const;
    
    Result<uint32_t> acquireSwapchainImage(uint32_t viewIndex);
    Result<void> waitSwapchainImage(uint32_t viewIndex);
    Result<void> releaseSwapchainImage(uint32_t viewIndex);
    
    void handleSessionStateChange(XrSessionState newState);

private:
    explicit XRSession(const Config& config);
    
    Result<void> createInstance();
    Result<void> createSession(void* graphicsBinding);
    Result<void> createReferenceSpaces();
    Result<void> createSwapchains();
    
    void releaseAllSwapchainImages();
    void handleSessionLoss();
    
    XrResult checkXrResult(XrResult result, const char* operation);

private:
    Config config_;
    
    XrInstance instance_ = XR_NULL_HANDLE;
    XrSession session_ = XR_NULL_HANDLE;
    XrSpace localSpace_ = XR_NULL_HANDLE;
    XrSpace viewSpace_ = XR_NULL_HANDLE;
    
    std::vector<XrSwapchain> swapchains_;
    std::vector<std::vector<SwapchainImage>> swapchainImages_;
    std::vector<bool> swapchainImageAcquired_;
    
    std::atomic<XRSessionState> sessionState_{XRSessionState::IDLE};
    std::atomic<bool> sessionRunning_{false};
    std::atomic<bool> shouldRender_{false};
    
    XrTime predictedDisplayTime_ = 0;
    
    SessionStateCallback sessionStateCallback_;
    
    ScopedSwapchainCounter swapchainCounter_;
};

} // namespace kimoyooju

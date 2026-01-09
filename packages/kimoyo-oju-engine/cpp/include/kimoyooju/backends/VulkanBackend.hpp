#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "kimoyooju/IRenderBackend.hpp"

namespace kimoyooju {

struct VulkanConfig {
    bool enableValidation = true;
    bool enableRayTracing = false;
    uint32_t maxFramesInFlight = 2;
    VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
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
    void updateMaterial(Handle handle, const MaterialData& data) override;
    
    void setCamera(const RenderCamera& camera) override;
    void renderMesh(Handle meshHandle, Handle materialHandle, const Mat4& transform) override;
    void renderLight(const Light& light) override;
    
    RenderCapabilities getCapabilities() const override;
    uint64_t getGPUMemoryUsage() const override;
    
    // Vulkan-specific getters for XR integration
    VkInstance getInstance() const { return instance_; }
    VkDevice getDevice() const { return device_; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice_; }
    VkQueue getGraphicsQueue() const { return graphicsQueue_; }
    uint32_t getGraphicsQueueFamily() const { return graphicsQueueFamily_; }
    
private:
    // Initialization
    void createInstance();
    void setupDebugMessenger();
    void selectPhysicalDevice();
    void createLogicalDevice();
    void createSurface(void* nativeWindow);
    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createDepthResources();
    
    // Cleanup
    void cleanupSwapchain();
    void recreateSwapchain();
    
    // Helpers
    VkShaderModule createShaderModule(const std::vector<uint32_t>& code);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                                  VkImageTiling tiling,
                                  VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer& buffer,
                      VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createImage(uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage& image,
                     VkDeviceMemory& imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    
    // Configuration
    VulkanConfig config_;
    
    // Core Vulkan objects
    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    
    // Queues
    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkQueue presentQueue_ = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily_ = 0;
    uint32_t presentQueueFamily_ = 0;
    
    // Swapchain
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainImageViews_;
    VkFormat swapchainImageFormat_;
    VkExtent2D swapchainExtent_;
    
    // Render pass and pipeline
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline_ = VK_NULL_HANDLE;
    
    // Framebuffers
    std::vector<VkFramebuffer> framebuffers_;
    
    // Command buffers
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers_;
    
    // Synchronization
    std::vector<VkSemaphore> imageAvailableSemaphores_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;
    std::vector<VkFence> inFlightFences_;
    
    // Depth buffer
    VkImage depthImage_ = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory_ = VK_NULL_HANDLE;
    VkImageView depthImageView_ = VK_NULL_HANDLE;
    
    // Uniform buffers
    std::vector<VkBuffer> uniformBuffers_;
    std::vector<VkDeviceMemory> uniformBuffersMemory_;
    std::vector<void*> uniformBuffersMapped_;
    
    // Descriptors
    VkDescriptorPool descriptorPool_ = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets_;
    
    // Resource registries
    struct MeshResource {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };
    std::unordered_map<uint64_t, MeshResource> meshes_;
    
    struct TextureResource {
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler sampler;
    };
    std::unordered_map<uint64_t, TextureResource> textures_;
    
    struct MaterialResource {
        MaterialData data;
        VkDescriptorSet descriptorSet;
    };
    std::unordered_map<uint64_t, MaterialResource> materials_;
    
    // State
    uint32_t currentFrame_ = 0;
    uint32_t currentImageIndex_ = 0;
    int width_ = 0;
    int height_ = 0;
    bool framebufferResized_ = false;
    
    // Camera
    RenderCamera currentCamera_;
    
    // Render queue
    struct RenderCommand {
        Handle meshHandle;
        Handle materialHandle;
        Mat4 transform;
    };
    std::vector<RenderCommand> renderQueue_;
    std::vector<Light> lights_;
    
    // Handle allocation
    uint64_t nextHandle_ = 1;
    
    // Memory tracking
    uint64_t gpuMemoryUsage_ = 0;
};

} // namespace kimoyooju

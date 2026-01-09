#pragma once

#include "kimoyooju/Types.hpp"
#include <memory>
#include <vector>

namespace kimoyooju {

struct MeshData {
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<uint32_t> indices;
};

struct TextureData {
    const uint8_t* pixels;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    bool generateMipmaps;
};

struct MaterialData {
    Vec4 baseColor;
    float metallic;
    float roughness;
    Handle baseColorTexture;
    Handle normalTexture;
    Handle metallicRoughnessTexture;
};

struct RenderCapabilities {
    bool supportsRayTracing = false;
    bool supportsCompute = false;
    bool supportsPBR = true;
    bool supportsHDR = false;
    uint32_t maxTextureSize = 4096;
    uint32_t maxVertices = 1000000;
};

class IRenderBackend {
public:
    virtual ~IRenderBackend() = default;
    
    // Lifecycle
    virtual bool initialize(void* nativeWindow, int width, int height) = 0;
    virtual void shutdown() = 0;
    virtual void resize(int width, int height) = 0;
    
    // Frame
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    
    // Resources
    virtual Handle createMesh(const MeshData& data) = 0;
    virtual void destroyMesh(Handle handle) = 0;
    
    virtual Handle createTexture(const TextureData& data) = 0;
    virtual void destroyTexture(Handle handle) = 0;
    
    virtual Handle createMaterial(const MaterialData& data) = 0;
    virtual void destroyMaterial(Handle handle) = 0;
    virtual void updateMaterial(Handle handle, const MaterialData& data) = 0;
    
    // Scene rendering
    virtual void setCamera(const RenderCamera& camera) = 0;
    virtual void renderMesh(Handle meshHandle, Handle materialHandle, const Mat4& transform) = 0;
    virtual void renderLight(const Light& light) = 0;
    
    // Capabilities
    virtual RenderCapabilities getCapabilities() const = 0;
    
    // Memory stats
    virtual uint64_t getGPUMemoryUsage() const = 0;
};

enum class Platform {
    iOS,
    iPadOS,
    visionOS,
    Android,
    Quest,
    AndroidXR
};

std::unique_ptr<IRenderBackend> createRenderBackend(Platform platform);

} // namespace kimoyooju

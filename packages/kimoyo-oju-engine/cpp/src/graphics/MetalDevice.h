#pragma once

#include "../include/kimoyooju/GraphicsDevice.hpp"
#include "../include/kimoyooju/Types.hpp"

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>
#endif

#include <string>
#include <unordered_map>
#include <memory>

namespace kimoyooju {

#ifdef __APPLE__

class MetalTexture : public Texture {
public:
    MetalTexture(id<MTLTexture> texture, uint32_t width, uint32_t height, TextureFormat format);
    ~MetalTexture() override;

    void bind(uint32_t unit) override;
    void upload(const void* data, uint32_t width, uint32_t height, TextureFormat format) override;
    void generateMipmaps() override;

    id<MTLTexture> getTexture() const { return texture_; }

private:
    id<MTLTexture> texture_;
    uint32_t width_;
    uint32_t height_;
    TextureFormat format_;
};

class MetalBuffer : public Buffer {
public:
    MetalBuffer(id<MTLBuffer> buffer, size_t size, BufferType type);
    ~MetalBuffer() override;

    void upload(const void* data, size_t size, size_t offset) override;
    void* map() override;
    void unmap() override;

    id<MTLBuffer> getBuffer() const { return buffer_; }

private:
    id<MTLBuffer> buffer_;
    size_t size_;
    BufferType type_;
};

class MetalShader : public Shader {
public:
    MetalShader(id<MTLRenderPipelineState> pipeline, const std::string& name);
    ~MetalShader() override;

    void bind() override;
    void setUniform(const std::string& name, float value) override;
    void setUniform(const std::string& name, const float* matrix4x4) override;
    void setUniform(const std::string& name, const float* vec, uint32_t count) override;

    id<MTLRenderPipelineState> getPipeline() const { return pipeline_; }

private:
    id<MTLRenderPipelineState> pipeline_;
    std::string name_;
    std::unordered_map<std::string, uint32_t> uniformOffsets_;
};

class MetalFramebuffer : public Framebuffer {
public:
    MetalFramebuffer(uint32_t width, uint32_t height);
    ~MetalFramebuffer() override;

    void bind() override;
    void unbind() override;
    void resize(uint32_t width, uint32_t height) override;

    void attachColor(std::shared_ptr<Texture> texture, uint32_t index) override;
    void attachDepth(std::shared_ptr<Texture> texture) override;

    MTLRenderPassDescriptor* getRenderPassDescriptor() const { return renderPassDescriptor_; }

private:
    MTLRenderPassDescriptor* renderPassDescriptor_;
    uint32_t width_;
    uint32_t height_;
    std::vector<std::shared_ptr<Texture>> colorAttachments_;
    std::shared_ptr<Texture> depthAttachment_;
};

class MetalDevice : public GraphicsDevice {
public:
    MetalDevice();
    ~MetalDevice() override;

    bool initialize(void* metalLayer) override;
    void shutdown() override;

    std::shared_ptr<Texture> createTexture(uint32_t width, uint32_t height, TextureFormat format) override;
    std::shared_ptr<Buffer> createBuffer(size_t size, BufferType type) override;
    std::shared_ptr<Shader> createShader(const std::string& vertexSrc, const std::string& fragmentSrc) override;
    std::shared_ptr<Framebuffer> createFramebuffer(uint32_t width, uint32_t height) override;

    void beginFrame() override;
    void endFrame() override;

    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void setClearColor(float r, float g, float b, float a) override;
    void clear(bool color, bool depth, bool stencil) override;

    void drawArrays(uint32_t vertexCount, uint32_t firstVertex) override;
    void drawIndexed(uint32_t indexCount, uint32_t firstIndex) override;

    void setVertexBuffer(std::shared_ptr<Buffer> buffer, uint32_t stride) override;
    void setIndexBuffer(std::shared_ptr<Buffer> buffer) override;

    bool swapBuffers() override;

    // Metal-specific
    id<MTLDevice> getDevice() const { return device_; }
    id<MTLCommandQueue> getCommandQueue() const { return commandQueue_; }
    id<MTLRenderCommandEncoder> getCurrentEncoder() const { return currentEncoder_; }
    
    // Shader library management
    id<MTLLibrary> compileShaderLibrary(const std::string& source);
    std::shared_ptr<Shader> createShaderFromFunctions(
        id<MTLFunction> vertexFunc, 
        id<MTLFunction> fragmentFunc,
        const std::string& name);

private:
    id<MTLDevice> device_;
    id<MTLCommandQueue> commandQueue_;
    CAMetalLayer* metalLayer_;
    
    id<MTLCommandBuffer> currentCommandBuffer_;
    id<MTLRenderCommandEncoder> currentEncoder_;
    id<CAMetalDrawable> currentDrawable_;
    
    id<MTLDepthStencilState> depthStencilState_;
    id<MTLTexture> depthTexture_;
    
    MTLViewport viewport_;
    MTLClearColor clearColor_;
    
    std::shared_ptr<MetalBuffer> currentVertexBuffer_;
    std::shared_ptr<MetalBuffer> currentIndexBuffer_;
    uint32_t currentVertexStride_;
    
    bool initialized_;
    uint32_t viewportWidth_;
    uint32_t viewportHeight_;
};

#else

// Stub for non-Apple platforms
class MetalDevice : public GraphicsDevice {
public:
    bool initialize(void*) override { return false; }
    void shutdown() override {}
    std::shared_ptr<Texture> createTexture(uint32_t, uint32_t, TextureFormat) override { return nullptr; }
    std::shared_ptr<Buffer> createBuffer(size_t, BufferType) override { return nullptr; }
    std::shared_ptr<Shader> createShader(const std::string&, const std::string&) override { return nullptr; }
    std::shared_ptr<Framebuffer> createFramebuffer(uint32_t, uint32_t) override { return nullptr; }
    void beginFrame() override {}
    void endFrame() override {}
    void setViewport(uint32_t, uint32_t, uint32_t, uint32_t) override {}
    void setClearColor(float, float, float, float) override {}
    void clear(bool, bool, bool) override {}
    void drawArrays(uint32_t, uint32_t) override {}
    void drawIndexed(uint32_t, uint32_t) override {}
    void setVertexBuffer(std::shared_ptr<Buffer>, uint32_t) override {}
    void setIndexBuffer(std::shared_ptr<Buffer>) override {}
    bool swapBuffers() override { return false; }
};

#endif // __APPLE__

} // namespace kimoyooju

#pragma once

#include "kimoyooju/Types.hpp"
#include "kimoyooju/MemoryCounters.hpp"

#include <memory>
#include <cstdint>

namespace kimoyooju {

enum class GraphicsAPI : uint8_t {
    OPENGL_ES,
    VULKAN,
    METAL
};

class Texture {
public:
    struct Desc {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t mipLevels = 1;
        bool sRGB = true;
    };
    
    virtual ~Texture() = default;
    virtual uint32_t getHandle() const = 0;
    virtual const Desc& getDesc() const = 0;
    
protected:
    ScopedTextureCounter counter_;
};

class Buffer {
public:
    enum class Type { VERTEX, INDEX, UNIFORM };
    
    struct Desc {
        Type type;
        size_t size = 0;
        bool dynamic = false;
    };
    
    virtual ~Buffer() = default;
    virtual void update(const void* data, size_t size, size_t offset = 0) = 0;
    
protected:
    ScopedBufferCounter counter_;
};

class Shader {
public:
    virtual ~Shader() = default;
    virtual void bind() = 0;
    virtual void setUniform(const char* name, const Mat4& value) = 0;
    virtual void setUniform(const char* name, const Vec4& value) = 0;
    virtual void setUniform(const char* name, float value) = 0;
};

class Framebuffer {
public:
    struct Desc {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t samples = 1;
        bool hasDepth = true;
    };
    
    virtual ~Framebuffer() = default;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual uint32_t getColorTexture() const = 0;
};

class GraphicsDevice {
public:
    struct Config {
        GraphicsAPI api = GraphicsAPI::OPENGL_ES;
        bool enableValidation = false;
        bool enableMSAA = true;
        uint32_t msaaSamples = 4;
    };
    
    static std::unique_ptr<GraphicsDevice> create(const Config& config);
    virtual ~GraphicsDevice() = default;
    
    GraphicsDevice(const GraphicsDevice&) = delete;
    GraphicsDevice& operator=(const GraphicsDevice&) = delete;
    
    virtual Result<void> initialize(void* nativeSurface, uint32_t width, uint32_t height) = 0;
    virtual void shutdown() = 0;
    
    virtual Result<void> resize(uint32_t width, uint32_t height) = 0;
    
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void present() = 0;
    
    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void setClearColor(float r, float g, float b, float a) = 0;
    virtual void clear(bool color, bool depth, bool stencil) = 0;
    
    virtual std::unique_ptr<Texture> createTexture(const Texture::Desc& desc, const void* data) = 0;
    virtual std::unique_ptr<Buffer> createBuffer(const Buffer::Desc& desc, const void* data = nullptr) = 0;
    virtual std::unique_ptr<Shader> createShader(const char* vertexSrc, const char* fragmentSrc) = 0;
    virtual std::unique_ptr<Framebuffer> createFramebuffer(const Framebuffer::Desc& desc) = 0;
    
    virtual void* getGraphicsBinding() = 0; // For OpenXR
    
    virtual bool isValid() const = 0;
    virtual GraphicsAPI getAPI() const = 0;

protected:
    GraphicsDevice() = default;
};

} // namespace kimoyooju

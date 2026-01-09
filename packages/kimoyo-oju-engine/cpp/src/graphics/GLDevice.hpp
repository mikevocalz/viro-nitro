#pragma once

#include "../include/kimoyooju/GraphicsDevice.hpp"
#include "../include/kimoyooju/Types.hpp"

#ifdef __ANDROID__
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
// Stub for non-Android builds
typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned int GLenum;
#endif

#include <string>
#include <unordered_map>
#include <memory>

namespace kimoyooju {

class GLTexture : public Texture {
public:
    GLTexture(GLuint id, uint32_t width, uint32_t height, TextureFormat format);
    ~GLTexture() override;

    void bind(uint32_t unit) override;
    void upload(const void* data, uint32_t width, uint32_t height, TextureFormat format) override;
    void generateMipmaps() override;

    GLuint getId() const { return textureId_; }

private:
    GLuint textureId_ = 0;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    TextureFormat format_ = TextureFormat::RGBA8;
};

class GLBuffer : public Buffer {
public:
    GLBuffer(GLuint id, size_t size, BufferType type);
    ~GLBuffer() override;

    void upload(const void* data, size_t size, size_t offset) override;
    void* map() override;
    void unmap() override;

    GLuint getId() const { return bufferId_; }
    GLenum getTarget() const;

private:
    GLuint bufferId_ = 0;
    size_t size_ = 0;
    BufferType type_;
    void* mappedPtr_ = nullptr;
};

class GLShader : public Shader {
public:
    GLShader(GLuint program, const std::string& name);
    ~GLShader() override;

    void bind() override;
    void setUniform(const std::string& name, float value) override;
    void setUniform(const std::string& name, const float* matrix4x4) override;
    void setUniform(const std::string& name, const float* vec, uint32_t count) override;

    GLuint getProgram() const { return program_; }
    GLint getUniformLocation(const std::string& name);

private:
    GLuint program_ = 0;
    std::string name_;
    std::unordered_map<std::string, GLint> uniformCache_;
};

class GLFramebuffer : public Framebuffer {
public:
    GLFramebuffer(GLuint fbo, uint32_t width, uint32_t height);
    ~GLFramebuffer() override;

    void bind() override;
    void unbind() override;
    void resize(uint32_t width, uint32_t height) override;

    void attachColor(std::shared_ptr<Texture> texture, uint32_t index) override;
    void attachDepth(std::shared_ptr<Texture> texture) override;

    GLuint getId() const { return fbo_; }

private:
    GLuint fbo_ = 0;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    std::vector<std::shared_ptr<Texture>> colorAttachments_;
    std::shared_ptr<Texture> depthAttachment_;
};

class GLDevice : public GraphicsDevice {
public:
    GLDevice();
    ~GLDevice() override;

    bool initialize(void* nativeWindow) override;
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

    // OpenGL-specific
    bool initEGL(void* nativeWindow);
    void destroyEGL();
    GLuint compileShader(GLenum type, const std::string& source);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);

private:
#ifdef __ANDROID__
    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLSurface surface_ = EGL_NO_SURFACE;
    EGLContext context_ = EGL_NO_CONTEXT;
    EGLConfig config_ = nullptr;
#endif
    
    bool initialized_ = false;
    uint32_t viewportWidth_ = 0;
    uint32_t viewportHeight_ = 0;
    
    std::shared_ptr<GLBuffer> currentVertexBuffer_;
    std::shared_ptr<GLBuffer> currentIndexBuffer_;
    uint32_t currentVertexStride_ = 0;
    
    GLuint defaultVAO_ = 0;
};

} // namespace kimoyooju

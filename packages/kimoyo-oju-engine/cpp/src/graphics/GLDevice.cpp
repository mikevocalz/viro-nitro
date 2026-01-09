#include "GLDevice.hpp"
#include <stdexcept>
#include <vector>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "KimoyoOjuGL", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "KimoyoOjuGL", __VA_ARGS__)
#else
#define LOGE(...) fprintf(stderr, __VA_ARGS__)
#define LOGI(...) printf(__VA_ARGS__)
#endif

namespace kimoyooju {

// ============================================================================
// GLTexture Implementation
// ============================================================================

GLTexture::GLTexture(GLuint id, uint32_t width, uint32_t height, TextureFormat format)
    : textureId_(id), width_(width), height_(height), format_(format) {}

GLTexture::~GLTexture() {
#ifdef __ANDROID__
    if (textureId_ != 0) {
        glDeleteTextures(1, &textureId_);
        textureId_ = 0;
    }
#endif
}

void GLTexture::bind(uint32_t unit) {
#ifdef __ANDROID__
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureId_);
#endif
}

void GLTexture::upload(const void* data, uint32_t width, uint32_t height, TextureFormat format) {
#ifdef __ANDROID__
    glBindTexture(GL_TEXTURE_2D, textureId_);
    
    GLenum glFormat = GL_RGBA;
    GLenum glType = GL_UNSIGNED_BYTE;
    
    switch (format) {
        case TextureFormat::R8:
            glFormat = GL_RED;
            break;
        case TextureFormat::RG8:
            glFormat = GL_RG;
            break;
        case TextureFormat::RGB8:
            glFormat = GL_RGB;
            break;
        case TextureFormat::RGBA8:
            glFormat = GL_RGBA;
            break;
        case TextureFormat::DEPTH24:
            glFormat = GL_DEPTH_COMPONENT;
            glType = GL_UNSIGNED_INT;
            break;
        case TextureFormat::DEPTH32F:
            glFormat = GL_DEPTH_COMPONENT;
            glType = GL_FLOAT;
            break;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, glType, data);
    
    width_ = width;
    height_ = height;
    format_ = format;
#endif
}

void GLTexture::generateMipmaps() {
#ifdef __ANDROID__
    glBindTexture(GL_TEXTURE_2D, textureId_);
    glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

// ============================================================================
// GLBuffer Implementation
// ============================================================================

GLBuffer::GLBuffer(GLuint id, size_t size, BufferType type)
    : bufferId_(id), size_(size), type_(type) {}

GLBuffer::~GLBuffer() {
#ifdef __ANDROID__
    if (bufferId_ != 0) {
        glDeleteBuffers(1, &bufferId_);
        bufferId_ = 0;
    }
#endif
}

GLenum GLBuffer::getTarget() const {
#ifdef __ANDROID__
    switch (type_) {
        case BufferType::VERTEX: return GL_ARRAY_BUFFER;
        case BufferType::INDEX: return GL_ELEMENT_ARRAY_BUFFER;
        case BufferType::UNIFORM: return GL_UNIFORM_BUFFER;
        default: return GL_ARRAY_BUFFER;
    }
#else
    return 0;
#endif
}

void GLBuffer::upload(const void* data, size_t size, size_t offset) {
#ifdef __ANDROID__
    GLenum target = getTarget();
    glBindBuffer(target, bufferId_);
    
    if (offset == 0 && size == size_) {
        glBufferData(target, size, data, GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(target, offset, size, data);
    }
#endif
}

void* GLBuffer::map() {
#ifdef __ANDROID__
    GLenum target = getTarget();
    glBindBuffer(target, bufferId_);
    mappedPtr_ = glMapBufferRange(target, 0, size_, 
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    return mappedPtr_;
#else
    return nullptr;
#endif
}

void GLBuffer::unmap() {
#ifdef __ANDROID__
    if (mappedPtr_) {
        GLenum target = getTarget();
        glBindBuffer(target, bufferId_);
        glUnmapBuffer(target);
        mappedPtr_ = nullptr;
    }
#endif
}

// ============================================================================
// GLShader Implementation
// ============================================================================

GLShader::GLShader(GLuint program, const std::string& name)
    : program_(program), name_(name) {}

GLShader::~GLShader() {
#ifdef __ANDROID__
    if (program_ != 0) {
        glDeleteProgram(program_);
        program_ = 0;
    }
#endif
}

void GLShader::bind() {
#ifdef __ANDROID__
    glUseProgram(program_);
#endif
}

GLint GLShader::getUniformLocation(const std::string& name) {
#ifdef __ANDROID__
    auto it = uniformCache_.find(name);
    if (it != uniformCache_.end()) {
        return it->second;
    }
    
    GLint location = glGetUniformLocation(program_, name.c_str());
    uniformCache_[name] = location;
    return location;
#else
    return -1;
#endif
}

void GLShader::setUniform(const std::string& name, float value) {
#ifdef __ANDROID__
    GLint location = getUniformLocation(name);
    if (location >= 0) {
        glUniform1f(location, value);
    }
#endif
}

void GLShader::setUniform(const std::string& name, const float* matrix4x4) {
#ifdef __ANDROID__
    GLint location = getUniformLocation(name);
    if (location >= 0) {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix4x4);
    }
#endif
}

void GLShader::setUniform(const std::string& name, const float* vec, uint32_t count) {
#ifdef __ANDROID__
    GLint location = getUniformLocation(name);
    if (location >= 0) {
        switch (count) {
            case 1: glUniform1fv(location, 1, vec); break;
            case 2: glUniform2fv(location, 1, vec); break;
            case 3: glUniform3fv(location, 1, vec); break;
            case 4: glUniform4fv(location, 1, vec); break;
        }
    }
#endif
}

// ============================================================================
// GLFramebuffer Implementation
// ============================================================================

GLFramebuffer::GLFramebuffer(GLuint fbo, uint32_t width, uint32_t height)
    : fbo_(fbo), width_(width), height_(height) {}

GLFramebuffer::~GLFramebuffer() {
#ifdef __ANDROID__
    if (fbo_ != 0) {
        glDeleteFramebuffers(1, &fbo_);
        fbo_ = 0;
    }
#endif
}

void GLFramebuffer::bind() {
#ifdef __ANDROID__
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, width_, height_);
#endif
}

void GLFramebuffer::unbind() {
#ifdef __ANDROID__
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

void GLFramebuffer::resize(uint32_t width, uint32_t height) {
    width_ = width;
    height_ = height;
}

void GLFramebuffer::attachColor(std::shared_ptr<Texture> texture, uint32_t index) {
#ifdef __ANDROID__
    auto glTex = std::dynamic_pointer_cast<GLTexture>(texture);
    if (glTex) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, 
            GL_TEXTURE_2D, glTex->getId(), 0);
        
        if (index >= colorAttachments_.size()) {
            colorAttachments_.resize(index + 1);
        }
        colorAttachments_[index] = texture;
    }
#endif
}

void GLFramebuffer::attachDepth(std::shared_ptr<Texture> texture) {
#ifdef __ANDROID__
    auto glTex = std::dynamic_pointer_cast<GLTexture>(texture);
    if (glTex) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
            GL_TEXTURE_2D, glTex->getId(), 0);
        depthAttachment_ = texture;
    }
#endif
}

// ============================================================================
// GLDevice Implementation
// ============================================================================

GLDevice::GLDevice() {}

GLDevice::~GLDevice() {
    shutdown();
}

bool GLDevice::initialize(void* nativeWindow) {
    if (initialized_) {
        return true;
    }
    
#ifdef __ANDROID__
    if (!initEGL(nativeWindow)) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    
    // Create default VAO
    glGenVertexArrays(1, &defaultVAO_);
    glBindVertexArray(defaultVAO_);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    LOGI("OpenGL ES initialized: %s", glGetString(GL_VERSION));
    LOGI("Renderer: %s", glGetString(GL_RENDERER));
#endif
    
    initialized_ = true;
    return true;
}

void GLDevice::shutdown() {
    if (!initialized_) {
        return;
    }
    
#ifdef __ANDROID__
    if (defaultVAO_ != 0) {
        glDeleteVertexArrays(1, &defaultVAO_);
        defaultVAO_ = 0;
    }
    
    destroyEGL();
#endif
    
    initialized_ = false;
}

#ifdef __ANDROID__
bool GLDevice::initEGL(void* nativeWindow) {
    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }
    
    EGLint major, minor;
    if (!eglInitialize(display_, &major, &minor)) {
        LOGE("eglInitialize failed");
        return false;
    }
    
    LOGI("EGL version: %d.%d", major, minor);
    
    // Choose config
    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    
    EGLint numConfigs;
    if (!eglChooseConfig(display_, configAttribs, &config_, 1, &numConfigs) || numConfigs == 0) {
        LOGE("eglChooseConfig failed");
        return false;
    }
    
    // Create surface
    surface_ = eglCreateWindowSurface(display_, config_, 
        static_cast<EGLNativeWindowType>(nativeWindow), nullptr);
    if (surface_ == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return false;
    }
    
    // Create context
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    
    context_ = eglCreateContext(display_, config_, EGL_NO_CONTEXT, contextAttribs);
    if (context_ == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return false;
    }
    
    // Make current
    if (!eglMakeCurrent(display_, surface_, surface_, context_)) {
        LOGE("eglMakeCurrent failed");
        return false;
    }
    
    // Get surface size
    EGLint width, height;
    eglQuerySurface(display_, surface_, EGL_WIDTH, &width);
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &height);
    viewportWidth_ = width;
    viewportHeight_ = height;
    
    return true;
}

void GLDevice::destroyEGL() {
    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(display_, context_);
            context_ = EGL_NO_CONTEXT;
        }
        
        if (surface_ != EGL_NO_SURFACE) {
            eglDestroySurface(display_, surface_);
            surface_ = EGL_NO_SURFACE;
        }
        
        eglTerminate(display_);
        display_ = EGL_NO_DISPLAY;
    }
}
#endif

std::shared_ptr<Texture> GLDevice::createTexture(uint32_t width, uint32_t height, TextureFormat format) {
#ifdef __ANDROID__
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Allocate storage
    GLenum glFormat = GL_RGBA;
    GLenum internalFormat = GL_RGBA8;
    GLenum glType = GL_UNSIGNED_BYTE;
    
    switch (format) {
        case TextureFormat::R8:
            glFormat = GL_RED;
            internalFormat = GL_R8;
            break;
        case TextureFormat::RG8:
            glFormat = GL_RG;
            internalFormat = GL_RG8;
            break;
        case TextureFormat::RGB8:
            glFormat = GL_RGB;
            internalFormat = GL_RGB8;
            break;
        case TextureFormat::RGBA8:
            glFormat = GL_RGBA;
            internalFormat = GL_RGBA8;
            break;
        case TextureFormat::DEPTH24:
            glFormat = GL_DEPTH_COMPONENT;
            internalFormat = GL_DEPTH_COMPONENT24;
            glType = GL_UNSIGNED_INT;
            break;
        case TextureFormat::DEPTH32F:
            glFormat = GL_DEPTH_COMPONENT;
            internalFormat = GL_DEPTH_COMPONENT32F;
            glType = GL_FLOAT;
            break;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, glFormat, glType, nullptr);
    
    return std::make_shared<GLTexture>(textureId, width, height, format);
#else
    return nullptr;
#endif
}

std::shared_ptr<Buffer> GLDevice::createBuffer(size_t size, BufferType type) {
#ifdef __ANDROID__
    GLuint bufferId;
    glGenBuffers(1, &bufferId);
    
    auto buffer = std::make_shared<GLBuffer>(bufferId, size, type);
    
    GLenum target = buffer->getTarget();
    glBindBuffer(target, bufferId);
    glBufferData(target, size, nullptr, GL_DYNAMIC_DRAW);
    
    return buffer;
#else
    return nullptr;
#endif
}

GLuint GLDevice::compileShader(GLenum type, const std::string& source) {
#ifdef __ANDROID__
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOGE("Shader compilation failed: %s", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
#else
    return 0;
#endif
}

GLuint GLDevice::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
#ifdef __ANDROID__
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOGE("Program linking failed: %s", infoLog);
        glDeleteProgram(program);
        return 0;
    }
    
    return program;
#else
    return 0;
#endif
}

std::shared_ptr<Shader> GLDevice::createShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
#ifdef __ANDROID__
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    if (vertexShader == 0) {
        return nullptr;
    }
    
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return nullptr;
    }
    
    GLuint program = linkProgram(vertexShader, fragmentShader);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    if (program == 0) {
        return nullptr;
    }
    
    return std::make_shared<GLShader>(program, "shader");
#else
    return nullptr;
#endif
}

std::shared_ptr<Framebuffer> GLDevice::createFramebuffer(uint32_t width, uint32_t height) {
#ifdef __ANDROID__
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    return std::make_shared<GLFramebuffer>(fbo, width, height);
#else
    return nullptr;
#endif
}

void GLDevice::beginFrame() {
#ifdef __ANDROID__
    glBindVertexArray(defaultVAO_);
#endif
}

void GLDevice::endFrame() {
    // Nothing to do
}

void GLDevice::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
#ifdef __ANDROID__
    glViewport(x, y, width, height);
    viewportWidth_ = width;
    viewportHeight_ = height;
#endif
}

void GLDevice::setClearColor(float r, float g, float b, float a) {
#ifdef __ANDROID__
    glClearColor(r, g, b, a);
#endif
}

void GLDevice::clear(bool color, bool depth, bool stencil) {
#ifdef __ANDROID__
    GLbitfield mask = 0;
    if (color) mask |= GL_COLOR_BUFFER_BIT;
    if (depth) mask |= GL_DEPTH_BUFFER_BIT;
    if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
    glClear(mask);
#endif
}

void GLDevice::setVertexBuffer(std::shared_ptr<Buffer> buffer, uint32_t stride) {
    currentVertexBuffer_ = std::dynamic_pointer_cast<GLBuffer>(buffer);
    currentVertexStride_ = stride;
}

void GLDevice::setIndexBuffer(std::shared_ptr<Buffer> buffer) {
    currentIndexBuffer_ = std::dynamic_pointer_cast<GLBuffer>(buffer);
#ifdef __ANDROID__
    if (currentIndexBuffer_) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentIndexBuffer_->getId());
    }
#endif
}

void GLDevice::drawArrays(uint32_t vertexCount, uint32_t firstVertex) {
#ifdef __ANDROID__
    if (currentVertexBuffer_) {
        glBindBuffer(GL_ARRAY_BUFFER, currentVertexBuffer_->getId());
        
        // Position attribute (location 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, currentVertexStride_, (void*)0);
        
        // Normal attribute (location 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, currentVertexStride_, (void*)(3 * sizeof(float)));
        
        // UV attribute (location 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, currentVertexStride_, (void*)(6 * sizeof(float)));
    }
    
    glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
#endif
}

void GLDevice::drawIndexed(uint32_t indexCount, uint32_t firstIndex) {
#ifdef __ANDROID__
    if (currentVertexBuffer_) {
        glBindBuffer(GL_ARRAY_BUFFER, currentVertexBuffer_->getId());
        
        // Position attribute (location 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, currentVertexStride_, (void*)0);
        
        // Normal attribute (location 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, currentVertexStride_, (void*)(3 * sizeof(float)));
        
        // UV attribute (location 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, currentVertexStride_, (void*)(6 * sizeof(float)));
    }
    
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 
        (void*)(firstIndex * sizeof(uint32_t)));
#endif
}

bool GLDevice::swapBuffers() {
#ifdef __ANDROID__
    return eglSwapBuffers(display_, surface_) == EGL_TRUE;
#else
    return true;
#endif
}

} // namespace kimoyooju

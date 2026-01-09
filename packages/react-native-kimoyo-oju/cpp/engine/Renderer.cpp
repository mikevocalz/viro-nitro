#include "Renderer.hpp"
#include "SceneNode.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Geometry.hpp"
#include <android/log.h>

#define LOG_TAG "KimoyoOjuRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace kimoyooju {

static const char* VERTEX_SHADER = R"(#version 300 es
precision highp float;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 vWorldPos;
out vec3 vNormal;

void main() {
    vec4 worldPos = uModelMatrix * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(uModelMatrix))) * aNormal;
    gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
}
)";

static const char* FRAGMENT_SHADER = R"(#version 300 es
precision highp float;

in vec3 vWorldPos;
in vec3 vNormal;

uniform vec4 uColor;
uniform vec3 uAmbientLight;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(-uLightDirection);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    vec3 ambient = uAmbientLight;
    vec3 result = (ambient + diffuse) * uColor.rgb;
    
    fragColor = vec4(result, uColor.a);
}
)";

Renderer::Renderer() = default;

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize() {
    if (initialized_) return true;

    LOGI("Initializing renderer...");

    if (!compileShaders()) {
        LOGE("Failed to compile shaders");
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    initialized_ = true;
    LOGI("Renderer initialized successfully");
    return true;
}

void Renderer::shutdown() {
    if (defaultShader_ != 0) {
        glDeleteProgram(defaultShader_);
        defaultShader_ = 0;
    }
    initialized_ = false;
}

void Renderer::setViewport(int width, int height) {
    viewportWidth_ = width;
    viewportHeight_ = height;
    glViewport(0, 0, width, height);
}

void Renderer::setViewport(int x, int y, int width, int height) {
    viewportWidth_ = width;
    viewportHeight_ = height;
    glViewport(x, y, width, height);
}

void Renderer::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    glFinish();
}

void Renderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(const std::vector<std::shared_ptr<SceneNode>>& nodes,
                      const std::shared_ptr<Camera>& camera,
                      const std::vector<std::shared_ptr<Light>>& lights) {
    if (!initialized_ || !camera) return;

    glUseProgram(defaultShader_);
    
    static int frameCount = 0;
    int nodesRendered = 0;

    glm::mat4 viewMatrix = camera->getViewMatrix();
    glm::mat4 projectionMatrix = camera->getProjectionMatrix(
        static_cast<float>(viewportWidth_) / static_cast<float>(viewportHeight_)
    );

    glUniformMatrix4fv(uViewMatrix_, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(uProjectionMatrix_, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Set lighting
    glm::vec3 ambientLight(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDirection(0.5f, -1.0f, 0.5f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    for (const auto& light : lights) {
        if (light->getType() == LightType::Ambient) {
            ambientLight = light->getColor();
        } else if (light->getType() == LightType::Directional) {
            lightDirection = light->getDirection();
            lightColor = light->getColor() * light->getIntensity();
        }
    }

    glUniform3fv(uAmbientLight_, 1, glm::value_ptr(ambientLight));
    glUniform3fv(uLightDirection_, 1, glm::value_ptr(lightDirection));
    glUniform3fv(uLightColor_, 1, glm::value_ptr(lightColor));

    for (const auto& node : nodes) {
        renderNode(node, viewMatrix, projectionMatrix, lights);
    }
    
    frameCount++;
    if (frameCount == 1 || frameCount % 60 == 0) {
        LOGI("Frame %d: rendered %zu root nodes", frameCount, nodes.size());
    }
}

void Renderer::renderNode(const std::shared_ptr<SceneNode>& node,
                          const glm::mat4& viewMatrix,
                          const glm::mat4& projectionMatrix,
                          const std::vector<std::shared_ptr<Light>>& lights) {
    if (!node || !node->isVisible()) return;

    glm::mat4 modelMatrix = node->getWorldTransform();
    glUniformMatrix4fv(uModelMatrix_, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    auto geometry = node->getGeometry();
    if (geometry) {
        glm::vec4 color = node->getColor();
        glUniform4fv(uColor_, 1, glm::value_ptr(color));
        
        static int debugCount = 0;
        if (debugCount < 4) {
            glm::vec3 pos = node->getPosition();
            LOGI("Drawing node %u at pos(%.2f, %.2f, %.2f) color(%.2f, %.2f, %.2f)", 
                 node->getId(), pos.x, pos.y, pos.z, color.r, color.g, color.b);
            debugCount++;
        }
        
        geometry->draw();
    }

    for (const auto& child : node->getChildren()) {
        renderNode(child, viewMatrix, projectionMatrix, lights);
    }
}

bool Renderer::compileShaders() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
    if (vertexShader == 0) return false;

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    defaultShader_ = linkProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (defaultShader_ == 0) return false;

    uModelMatrix_ = glGetUniformLocation(defaultShader_, "uModelMatrix");
    uViewMatrix_ = glGetUniformLocation(defaultShader_, "uViewMatrix");
    uProjectionMatrix_ = glGetUniformLocation(defaultShader_, "uProjectionMatrix");
    uColor_ = glGetUniformLocation(defaultShader_, "uColor");
    uAmbientLight_ = glGetUniformLocation(defaultShader_, "uAmbientLight");
    uLightDirection_ = glGetUniformLocation(defaultShader_, "uLightDirection");
    uLightColor_ = glGetUniformLocation(defaultShader_, "uLightColor");

    LOGI("Shader uniforms: model=%d view=%d proj=%d color=%d ambient=%d lightDir=%d lightColor=%d",
         uModelMatrix_, uViewMatrix_, uProjectionMatrix_, uColor_, 
         uAmbientLight_, uLightDirection_, uLightColor_);

    return true;
}

GLuint Renderer::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
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
}

GLuint Renderer::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
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
}

} // namespace kimoyooju

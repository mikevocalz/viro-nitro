#pragma once

#include <GLES3/gl3.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace kimoyooju {

class SceneNode;
class Camera;
class Light;

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize();
    void shutdown();

    void setViewport(int width, int height);
    void setViewport(int x, int y, int width, int height);
    void beginFrame();
    void endFrame();

    void render(const std::vector<std::shared_ptr<SceneNode>>& nodes,
                const std::shared_ptr<Camera>& camera,
                const std::vector<std::shared_ptr<Light>>& lights);

    void clear(float r, float g, float b, float a);

    GLuint getDefaultShader() const { return defaultShader_; }

private:
    bool compileShaders();
    GLuint compileShader(GLenum type, const char* source);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);

    void renderNode(const std::shared_ptr<SceneNode>& node,
                    const glm::mat4& viewMatrix,
                    const glm::mat4& projectionMatrix,
                    const std::vector<std::shared_ptr<Light>>& lights);

    int viewportWidth_ = 0;
    int viewportHeight_ = 0;
    bool initialized_ = false;

    GLuint defaultShader_ = 0;

    // Uniform locations
    GLint uModelMatrix_ = -1;
    GLint uViewMatrix_ = -1;
    GLint uProjectionMatrix_ = -1;
    GLint uColor_ = -1;
    GLint uAmbientLight_ = -1;
    GLint uLightDirection_ = -1;
    GLint uLightColor_ = -1;
};

} // namespace kimoyooju

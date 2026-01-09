#include "SceneRenderer.hpp"
#include <cmath>

namespace kimoyooju {

// Hash helper for mesh cache keys
inline uint64_t hashDimensions(float a, float b, float c) {
    uint32_t ia = *reinterpret_cast<uint32_t*>(&a);
    uint32_t ib = *reinterpret_cast<uint32_t*>(&b);
    uint32_t ic = *reinterpret_cast<uint32_t*>(&c);
    return (static_cast<uint64_t>(ia) << 32) ^ (static_cast<uint64_t>(ib) << 16) ^ ic;
}

SceneRenderer::SceneRenderer() {}

SceneRenderer::~SceneRenderer() {
    shutdown();
}

bool SceneRenderer::initialize(std::shared_ptr<GraphicsDevice> device) {
    if (initialized_) {
        return true;
    }
    
    device_ = device;
    
    // Create default lit shader
    defaultShader_ = device_->createShader(DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMENT_SHADER);
    if (!defaultShader_) {
        return false;
    }
    
    // Create unlit shader
    unlitShader_ = device_->createShader(UNLIT_VERTEX_SHADER, UNLIT_FRAGMENT_SHADER);
    if (!unlitShader_) {
        return false;
    }
    
    // Set default camera
    camera_.position = {0, 0, 5};
    camera_.viewMatrix = mat4LookAt(camera_.position, {0, 0, 0}, {0, 1, 0});
    camera_.projectionMatrix = mat4Perspective(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    
    // Set default lighting
    ambientLight_.color = {0.2f, 0.2f, 0.2f, 1.0f};
    directionalLight_.direction = {0.0f, -1.0f, -1.0f};
    directionalLight_.color = {1.0f, 1.0f, 1.0f, 1.0f};
    directionalLight_.intensity = 1.0f;
    
    initialized_ = true;
    return true;
}

void SceneRenderer::shutdown() {
    if (!initialized_) {
        return;
    }
    
    // Clear mesh caches
    boxMeshCache_.clear();
    sphereMeshCache_.clear();
    planeMeshCache_.clear();
    cylinderMeshCache_.clear();
    
    defaultShader_.reset();
    unlitShader_.reset();
    device_.reset();
    
    initialized_ = false;
}

void SceneRenderer::setCamera(const RenderCamera& camera) {
    camera_ = camera;
}

void SceneRenderer::setAmbientLight(const AmbientLight& light) {
    ambientLight_ = light;
}

void SceneRenderer::setDirectionalLight(const RenderLight& light) {
    directionalLight_ = light;
}

void SceneRenderer::beginFrame() {
    if (!device_) return;
    
    device_->beginFrame();
    device_->setClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    device_->clear(true, true, true);
}

void SceneRenderer::renderScene(SceneGraph& scene) {
    if (!device_ || !initialized_) return;
    
    // Bind default shader
    defaultShader_->bind();
    
    // Set camera uniforms
    defaultShader_->setUniform("u_viewMatrix", camera_.viewMatrix.m);
    defaultShader_->setUniform("u_projectionMatrix", camera_.projectionMatrix.m);
    defaultShader_->setUniform("u_cameraPosition", &camera_.position.x, 3);
    
    // Set lighting uniforms
    defaultShader_->setUniform("u_ambientLight", &ambientLight_.color.r, 3);
    defaultShader_->setUniform("u_lightDirection", &directionalLight_.direction.x, 3);
    defaultShader_->setUniform("u_lightColor", &directionalLight_.color.r, 3);
    defaultShader_->setUniform("u_lightIntensity", directionalLight_.intensity);
    
    // Traverse and render scene
    if (scene.getRoot()) {
        Mat4 identity = mat4Identity();
        renderNode(scene.getRoot().get(), identity);
    }
}

void SceneRenderer::endFrame() {
    if (!device_) return;
    
    device_->endFrame();
    device_->swapBuffers();
}

void SceneRenderer::renderNode(SceneNode* node, const Mat4& parentTransform) {
    if (!node || !node->isVisible()) return;
    
    // Calculate world transform
    Mat4 localTransform = mat4FromTransform(node->getLocalTransform());
    Mat4 worldTransform = mat4Multiply(parentTransform, localTransform);
    
    // Render based on node type
    switch (node->getType()) {
        case NodeType::MESH: {
            auto* meshNode = static_cast<MeshNode*>(node);
            renderMeshNode(meshNode, worldTransform);
            break;
        }
        case NodeType::LIGHT: {
            // Update light from light node
            auto* lightNode = static_cast<LightNode*>(node);
            if (lightNode->getLightType() == LightType::DIRECTIONAL) {
                directionalLight_.color = lightNode->getColor();
                directionalLight_.intensity = lightNode->getIntensity();
                // Direction from node's forward vector (negative Z in local space)
                // For simplicity, use a fixed direction modified by rotation
            }
            break;
        }
        case NodeType::CAMERA: {
            // Could update camera from camera node
            break;
        }
        default:
            break;
    }
    
    // Render children
    for (auto& child : node->getChildren()) {
        renderNode(child.get(), worldTransform);
    }
}

void SceneRenderer::renderMeshNode(MeshNode* node, const Mat4& worldTransform) {
    if (!node) return;
    
    // Get material properties
    Color diffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
    Color specularColor = {1.0f, 1.0f, 1.0f, 1.0f};
    float shininess = 32.0f;
    
    // TODO: Get actual material from node
    // For now, use node name to determine color (hack for demo)
    
    setupShaderUniforms(worldTransform, diffuseColor);
    
    // Get or create mesh based on geometry type
    // For now, render a default box
    Mesh& mesh = getBoxMesh(1.0f, 1.0f, 1.0f);
    
    // Set vertex/index buffers and draw
    device_->setVertexBuffer(mesh.vertexBuffer, VERTEX_STRIDE);
    device_->setIndexBuffer(mesh.indexBuffer);
    device_->drawIndexed(mesh.indexCount, 0);
}

void SceneRenderer::setupShaderUniforms(const Mat4& modelMatrix, const Color& diffuseColor) {
    defaultShader_->setUniform("u_modelMatrix", modelMatrix.m);
    
    // Normal matrix = transpose(inverse(modelMatrix))
    Mat4 normalMatrix = mat4Transpose(mat4Inverse(modelMatrix));
    defaultShader_->setUniform("u_normalMatrix", normalMatrix.m);
    
    // Material properties
    defaultShader_->setUniform("u_diffuseColor", &diffuseColor.r, 4);
    
    Color specular = {1.0f, 1.0f, 1.0f, 1.0f};
    defaultShader_->setUniform("u_specularColor", &specular.r, 4);
    defaultShader_->setUniform("u_shininess", 32.0f);
    
    // No texture for now
    defaultShader_->setUniform("u_hasDiffuseTexture", 0.0f);
}

Mesh& SceneRenderer::getBoxMesh(float width, float height, float depth) {
    uint64_t key = hashDimensions(width, height, depth);
    
    auto it = boxMeshCache_.find(key);
    if (it != boxMeshCache_.end()) {
        return it->second;
    }
    
    boxMeshCache_[key] = GeometryGenerator::createBox(*device_, width, height, depth);
    return boxMeshCache_[key];
}

Mesh& SceneRenderer::getSphereMesh(float radius) {
    uint64_t key = hashDimensions(radius, 0, 0);
    
    auto it = sphereMeshCache_.find(key);
    if (it != sphereMeshCache_.end()) {
        return it->second;
    }
    
    sphereMeshCache_[key] = GeometryGenerator::createSphere(*device_, radius, 32, 16);
    return sphereMeshCache_[key];
}

Mesh& SceneRenderer::getPlaneMesh(float width, float height) {
    uint64_t key = hashDimensions(width, height, 0);
    
    auto it = planeMeshCache_.find(key);
    if (it != planeMeshCache_.end()) {
        return it->second;
    }
    
    planeMeshCache_[key] = GeometryGenerator::createPlane(*device_, width, height);
    return planeMeshCache_[key];
}

Mesh& SceneRenderer::getCylinderMesh(float radius, float height) {
    uint64_t key = hashDimensions(radius, height, 0);
    
    auto it = cylinderMeshCache_.find(key);
    if (it != cylinderMeshCache_.end()) {
        return it->second;
    }
    
    cylinderMeshCache_[key] = GeometryGenerator::createCylinder(*device_, radius, height, 32);
    return cylinderMeshCache_[key];
}

} // namespace kimoyooju

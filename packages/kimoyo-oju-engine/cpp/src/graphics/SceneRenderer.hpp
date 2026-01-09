#pragma once

#include "../include/kimoyooju/SceneGraph.hpp"
#include "../include/kimoyooju/GraphicsDevice.hpp"
#include "../include/kimoyooju/Types.hpp"
#include "Geometry.hpp"
#include "Shaders.hpp"
#include <memory>
#include <unordered_map>

namespace kimoyooju {

struct RenderCamera {
    Mat4 viewMatrix;
    Mat4 projectionMatrix;
    Vec3 position;
    float fov = 60.0f;
    float nearClip = 0.1f;
    float farClip = 1000.0f;
};

struct RenderLight {
    Vec3 direction = {0.0f, -1.0f, 0.0f};
    Color color = {1.0f, 1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
};

struct AmbientLight {
    Color color = {0.2f, 0.2f, 0.2f, 1.0f};
};

class SceneRenderer {
public:
    SceneRenderer();
    ~SceneRenderer();

    bool initialize(std::shared_ptr<GraphicsDevice> device);
    void shutdown();

    void setCamera(const RenderCamera& camera);
    void setAmbientLight(const AmbientLight& light);
    void setDirectionalLight(const RenderLight& light);

    void beginFrame();
    void renderScene(SceneGraph& scene);
    void endFrame();

    // Primitive mesh cache
    Mesh& getBoxMesh(float width, float height, float depth);
    Mesh& getSphereMesh(float radius);
    Mesh& getPlaneMesh(float width, float height);
    Mesh& getCylinderMesh(float radius, float height);

private:
    void renderNode(SceneNode* node, const Mat4& parentTransform);
    void renderMeshNode(MeshNode* node, const Mat4& worldTransform);
    void setupShaderUniforms(const Mat4& modelMatrix, const Color& diffuseColor);

    std::shared_ptr<GraphicsDevice> device_;
    std::shared_ptr<Shader> defaultShader_;
    std::shared_ptr<Shader> unlitShader_;

    RenderCamera camera_;
    AmbientLight ambientLight_;
    RenderLight directionalLight_;

    // Mesh cache (keyed by dimensions)
    std::unordered_map<uint64_t, Mesh> boxMeshCache_;
    std::unordered_map<uint64_t, Mesh> sphereMeshCache_;
    std::unordered_map<uint64_t, Mesh> planeMeshCache_;
    std::unordered_map<uint64_t, Mesh> cylinderMeshCache_;

    bool initialized_ = false;
};

// ============================================================================
// Math Helpers
// ============================================================================

inline Mat4 mat4Identity() {
    return {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

inline Mat4 mat4Multiply(const Mat4& a, const Mat4& b) {
    Mat4 result = {};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float sum = 0;
            for (int k = 0; k < 4; ++k) {
                sum += a.m[i * 4 + k] * b.m[k * 4 + j];
            }
            result.m[i * 4 + j] = sum;
        }
    }
    return result;
}

inline Mat4 mat4Translation(const Vec3& t) {
    return {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        t.x, t.y, t.z, 1
    };
}

inline Mat4 mat4Scale(const Vec3& s) {
    return {
        s.x, 0, 0, 0,
        0, s.y, 0, 0,
        0, 0, s.z, 0,
        0, 0, 0, 1
    };
}

inline Mat4 mat4FromQuat(const Quat& q) {
    float x2 = q.x + q.x;
    float y2 = q.y + q.y;
    float z2 = q.z + q.z;
    float xx = q.x * x2;
    float xy = q.x * y2;
    float xz = q.x * z2;
    float yy = q.y * y2;
    float yz = q.y * z2;
    float zz = q.z * z2;
    float wx = q.w * x2;
    float wy = q.w * y2;
    float wz = q.w * z2;

    return {
        1 - (yy + zz), xy + wz, xz - wy, 0,
        xy - wz, 1 - (xx + zz), yz + wx, 0,
        xz + wy, yz - wx, 1 - (xx + yy), 0,
        0, 0, 0, 1
    };
}

inline Mat4 mat4FromTransform(const Transform& t) {
    Mat4 translation = mat4Translation(t.position);
    Mat4 rotation = mat4FromQuat(t.rotation);
    Mat4 scale = mat4Scale(t.scale);
    return mat4Multiply(mat4Multiply(translation, rotation), scale);
}

inline Mat4 mat4Perspective(float fovY, float aspect, float nearZ, float farZ) {
    float tanHalfFov = std::tan(fovY * 0.5f * 3.14159265359f / 180.0f);
    float range = farZ - nearZ;
    
    return {
        1.0f / (aspect * tanHalfFov), 0, 0, 0,
        0, 1.0f / tanHalfFov, 0, 0,
        0, 0, -(farZ + nearZ) / range, -1,
        0, 0, -2.0f * farZ * nearZ / range, 0
    };
}

inline Mat4 mat4LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = {center.x - eye.x, center.y - eye.y, center.z - eye.z};
    float fLen = std::sqrt(f.x*f.x + f.y*f.y + f.z*f.z);
    f = {f.x/fLen, f.y/fLen, f.z/fLen};
    
    Vec3 s = {
        f.y * up.z - f.z * up.y,
        f.z * up.x - f.x * up.z,
        f.x * up.y - f.y * up.x
    };
    float sLen = std::sqrt(s.x*s.x + s.y*s.y + s.z*s.z);
    s = {s.x/sLen, s.y/sLen, s.z/sLen};
    
    Vec3 u = {
        s.y * f.z - s.z * f.y,
        s.z * f.x - s.x * f.z,
        s.x * f.y - s.y * f.x
    };
    
    return {
        s.x, u.x, -f.x, 0,
        s.y, u.y, -f.y, 0,
        s.z, u.z, -f.z, 0,
        -(s.x*eye.x + s.y*eye.y + s.z*eye.z),
        -(u.x*eye.x + u.y*eye.y + u.z*eye.z),
        (f.x*eye.x + f.y*eye.y + f.z*eye.z),
        1
    };
}

inline Mat4 mat4Transpose(const Mat4& m) {
    return {
        m.m[0], m.m[4], m.m[8], m.m[12],
        m.m[1], m.m[5], m.m[9], m.m[13],
        m.m[2], m.m[6], m.m[10], m.m[14],
        m.m[3], m.m[7], m.m[11], m.m[15]
    };
}

inline Mat4 mat4Inverse(const Mat4& m) {
    // Simplified inverse for transform matrices
    Mat4 inv;
    float det;

    inv.m[0] = m.m[5] * m.m[10] * m.m[15] - m.m[5] * m.m[11] * m.m[14] - 
               m.m[9] * m.m[6] * m.m[15] + m.m[9] * m.m[7] * m.m[14] + 
               m.m[13] * m.m[6] * m.m[11] - m.m[13] * m.m[7] * m.m[10];

    inv.m[4] = -m.m[4] * m.m[10] * m.m[15] + m.m[4] * m.m[11] * m.m[14] + 
                m.m[8] * m.m[6] * m.m[15] - m.m[8] * m.m[7] * m.m[14] - 
                m.m[12] * m.m[6] * m.m[11] + m.m[12] * m.m[7] * m.m[10];

    inv.m[8] = m.m[4] * m.m[9] * m.m[15] - m.m[4] * m.m[11] * m.m[13] - 
               m.m[8] * m.m[5] * m.m[15] + m.m[8] * m.m[7] * m.m[13] + 
               m.m[12] * m.m[5] * m.m[11] - m.m[12] * m.m[7] * m.m[9];

    inv.m[12] = -m.m[4] * m.m[9] * m.m[14] + m.m[4] * m.m[10] * m.m[13] + 
                 m.m[8] * m.m[5] * m.m[14] - m.m[8] * m.m[6] * m.m[13] - 
                 m.m[12] * m.m[5] * m.m[10] + m.m[12] * m.m[6] * m.m[9];

    inv.m[1] = -m.m[1] * m.m[10] * m.m[15] + m.m[1] * m.m[11] * m.m[14] + 
                m.m[9] * m.m[2] * m.m[15] - m.m[9] * m.m[3] * m.m[14] - 
                m.m[13] * m.m[2] * m.m[11] + m.m[13] * m.m[3] * m.m[10];

    inv.m[5] = m.m[0] * m.m[10] * m.m[15] - m.m[0] * m.m[11] * m.m[14] - 
               m.m[8] * m.m[2] * m.m[15] + m.m[8] * m.m[3] * m.m[14] + 
               m.m[12] * m.m[2] * m.m[11] - m.m[12] * m.m[3] * m.m[10];

    inv.m[9] = -m.m[0] * m.m[9] * m.m[15] + m.m[0] * m.m[11] * m.m[13] + 
                m.m[8] * m.m[1] * m.m[15] - m.m[8] * m.m[3] * m.m[13] - 
                m.m[12] * m.m[1] * m.m[11] + m.m[12] * m.m[3] * m.m[9];

    inv.m[13] = m.m[0] * m.m[9] * m.m[14] - m.m[0] * m.m[10] * m.m[13] - 
                m.m[8] * m.m[1] * m.m[14] + m.m[8] * m.m[2] * m.m[13] + 
                m.m[12] * m.m[1] * m.m[10] - m.m[12] * m.m[2] * m.m[9];

    inv.m[2] = m.m[1] * m.m[6] * m.m[15] - m.m[1] * m.m[7] * m.m[14] - 
               m.m[5] * m.m[2] * m.m[15] + m.m[5] * m.m[3] * m.m[14] + 
               m.m[13] * m.m[2] * m.m[7] - m.m[13] * m.m[3] * m.m[6];

    inv.m[6] = -m.m[0] * m.m[6] * m.m[15] + m.m[0] * m.m[7] * m.m[14] + 
                m.m[4] * m.m[2] * m.m[15] - m.m[4] * m.m[3] * m.m[14] - 
                m.m[12] * m.m[2] * m.m[7] + m.m[12] * m.m[3] * m.m[6];

    inv.m[10] = m.m[0] * m.m[5] * m.m[15] - m.m[0] * m.m[7] * m.m[13] - 
                m.m[4] * m.m[1] * m.m[15] + m.m[4] * m.m[3] * m.m[13] + 
                m.m[12] * m.m[1] * m.m[7] - m.m[12] * m.m[3] * m.m[5];

    inv.m[14] = -m.m[0] * m.m[5] * m.m[14] + m.m[0] * m.m[6] * m.m[13] + 
                 m.m[4] * m.m[1] * m.m[14] - m.m[4] * m.m[2] * m.m[13] - 
                 m.m[12] * m.m[1] * m.m[6] + m.m[12] * m.m[2] * m.m[5];

    inv.m[3] = -m.m[1] * m.m[6] * m.m[11] + m.m[1] * m.m[7] * m.m[10] + 
                m.m[5] * m.m[2] * m.m[11] - m.m[5] * m.m[3] * m.m[10] - 
                m.m[9] * m.m[2] * m.m[7] + m.m[9] * m.m[3] * m.m[6];

    inv.m[7] = m.m[0] * m.m[6] * m.m[11] - m.m[0] * m.m[7] * m.m[10] - 
               m.m[4] * m.m[2] * m.m[11] + m.m[4] * m.m[3] * m.m[10] + 
               m.m[8] * m.m[2] * m.m[7] - m.m[8] * m.m[3] * m.m[6];

    inv.m[11] = -m.m[0] * m.m[5] * m.m[11] + m.m[0] * m.m[7] * m.m[9] + 
                 m.m[4] * m.m[1] * m.m[11] - m.m[4] * m.m[3] * m.m[9] - 
                 m.m[8] * m.m[1] * m.m[7] + m.m[8] * m.m[3] * m.m[5];

    inv.m[15] = m.m[0] * m.m[5] * m.m[10] - m.m[0] * m.m[6] * m.m[9] - 
                m.m[4] * m.m[1] * m.m[10] + m.m[4] * m.m[2] * m.m[9] + 
                m.m[8] * m.m[1] * m.m[6] - m.m[8] * m.m[2] * m.m[5];

    det = m.m[0] * inv.m[0] + m.m[1] * inv.m[4] + m.m[2] * inv.m[8] + m.m[3] * inv.m[12];

    if (det == 0) return mat4Identity();

    det = 1.0f / det;
    for (int i = 0; i < 16; i++) {
        inv.m[i] *= det;
    }

    return inv;
}

} // namespace kimoyooju

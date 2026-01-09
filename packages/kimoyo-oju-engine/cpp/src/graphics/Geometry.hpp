#pragma once

#include "../include/kimoyooju/GraphicsDevice.hpp"
#include <vector>
#include <memory>
#include <cmath>

namespace kimoyooju {

// Vertex layout: position(3) + normal(3) + uv(2) = 8 floats
struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];
};

constexpr size_t VERTEX_STRIDE = sizeof(Vertex);

struct Mesh {
    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
};

class GeometryGenerator {
public:
    static Mesh createBox(GraphicsDevice& device, float width, float height, float depth);
    static Mesh createSphere(GraphicsDevice& device, float radius, int segments, int rings);
    static Mesh createPlane(GraphicsDevice& device, float width, float height);
    static Mesh createCylinder(GraphicsDevice& device, float radius, float height, int segments);
    static Mesh createQuad(GraphicsDevice& device);
    
private:
    static void uploadMesh(GraphicsDevice& device, Mesh& mesh, 
                          const std::vector<Vertex>& vertices,
                          const std::vector<uint32_t>& indices);
};

// ============================================================================
// Implementation
// ============================================================================

inline void GeometryGenerator::uploadMesh(GraphicsDevice& device, Mesh& mesh,
                                         const std::vector<Vertex>& vertices,
                                         const std::vector<uint32_t>& indices) {
    mesh.vertexCount = static_cast<uint32_t>(vertices.size());
    mesh.indexCount = static_cast<uint32_t>(indices.size());
    
    mesh.vertexBuffer = device.createBuffer(vertices.size() * sizeof(Vertex), BufferType::VERTEX);
    mesh.vertexBuffer->upload(vertices.data(), vertices.size() * sizeof(Vertex), 0);
    
    mesh.indexBuffer = device.createBuffer(indices.size() * sizeof(uint32_t), BufferType::INDEX);
    mesh.indexBuffer->upload(indices.data(), indices.size() * sizeof(uint32_t), 0);
}

inline Mesh GeometryGenerator::createBox(GraphicsDevice& device, float width, float height, float depth) {
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float hd = depth * 0.5f;
    
    std::vector<Vertex> vertices = {
        // Front face
        {{-hw, -hh,  hd}, { 0,  0,  1}, {0, 0}},
        {{ hw, -hh,  hd}, { 0,  0,  1}, {1, 0}},
        {{ hw,  hh,  hd}, { 0,  0,  1}, {1, 1}},
        {{-hw,  hh,  hd}, { 0,  0,  1}, {0, 1}},
        
        // Back face
        {{ hw, -hh, -hd}, { 0,  0, -1}, {0, 0}},
        {{-hw, -hh, -hd}, { 0,  0, -1}, {1, 0}},
        {{-hw,  hh, -hd}, { 0,  0, -1}, {1, 1}},
        {{ hw,  hh, -hd}, { 0,  0, -1}, {0, 1}},
        
        // Top face
        {{-hw,  hh,  hd}, { 0,  1,  0}, {0, 0}},
        {{ hw,  hh,  hd}, { 0,  1,  0}, {1, 0}},
        {{ hw,  hh, -hd}, { 0,  1,  0}, {1, 1}},
        {{-hw,  hh, -hd}, { 0,  1,  0}, {0, 1}},
        
        // Bottom face
        {{-hw, -hh, -hd}, { 0, -1,  0}, {0, 0}},
        {{ hw, -hh, -hd}, { 0, -1,  0}, {1, 0}},
        {{ hw, -hh,  hd}, { 0, -1,  0}, {1, 1}},
        {{-hw, -hh,  hd}, { 0, -1,  0}, {0, 1}},
        
        // Right face
        {{ hw, -hh,  hd}, { 1,  0,  0}, {0, 0}},
        {{ hw, -hh, -hd}, { 1,  0,  0}, {1, 0}},
        {{ hw,  hh, -hd}, { 1,  0,  0}, {1, 1}},
        {{ hw,  hh,  hd}, { 1,  0,  0}, {0, 1}},
        
        // Left face
        {{-hw, -hh, -hd}, {-1,  0,  0}, {0, 0}},
        {{-hw, -hh,  hd}, {-1,  0,  0}, {1, 0}},
        {{-hw,  hh,  hd}, {-1,  0,  0}, {1, 1}},
        {{-hw,  hh, -hd}, {-1,  0,  0}, {0, 1}},
    };
    
    std::vector<uint32_t> indices = {
        0,  1,  2,  0,  2,  3,   // Front
        4,  5,  6,  4,  6,  7,   // Back
        8,  9,  10, 8,  10, 11,  // Top
        12, 13, 14, 12, 14, 15,  // Bottom
        16, 17, 18, 16, 18, 19,  // Right
        20, 21, 22, 20, 22, 23   // Left
    };
    
    Mesh mesh;
    uploadMesh(device, mesh, vertices, indices);
    return mesh;
}

inline Mesh GeometryGenerator::createSphere(GraphicsDevice& device, float radius, int segments, int rings) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    const float PI = 3.14159265359f;
    
    for (int ring = 0; ring <= rings; ++ring) {
        float phi = PI * static_cast<float>(ring) / static_cast<float>(rings);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);
        
        for (int seg = 0; seg <= segments; ++seg) {
            float theta = 2.0f * PI * static_cast<float>(seg) / static_cast<float>(segments);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);
            
            float x = sinPhi * cosTheta;
            float y = cosPhi;
            float z = sinPhi * sinTheta;
            
            float u = static_cast<float>(seg) / static_cast<float>(segments);
            float v = static_cast<float>(ring) / static_cast<float>(rings);
            
            Vertex vertex;
            vertex.position[0] = radius * x;
            vertex.position[1] = radius * y;
            vertex.position[2] = radius * z;
            vertex.normal[0] = x;
            vertex.normal[1] = y;
            vertex.normal[2] = z;
            vertex.uv[0] = u;
            vertex.uv[1] = v;
            
            vertices.push_back(vertex);
        }
    }
    
    for (int ring = 0; ring < rings; ++ring) {
        for (int seg = 0; seg < segments; ++seg) {
            uint32_t current = ring * (segments + 1) + seg;
            uint32_t next = current + segments + 1;
            
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);
            
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
    
    Mesh mesh;
    uploadMesh(device, mesh, vertices, indices);
    return mesh;
}

inline Mesh GeometryGenerator::createPlane(GraphicsDevice& device, float width, float height) {
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    
    std::vector<Vertex> vertices = {
        {{-hw, 0, -hh}, {0, 1, 0}, {0, 0}},
        {{ hw, 0, -hh}, {0, 1, 0}, {1, 0}},
        {{ hw, 0,  hh}, {0, 1, 0}, {1, 1}},
        {{-hw, 0,  hh}, {0, 1, 0}, {0, 1}},
    };
    
    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
    
    Mesh mesh;
    uploadMesh(device, mesh, vertices, indices);
    return mesh;
}

inline Mesh GeometryGenerator::createCylinder(GraphicsDevice& device, float radius, float height, int segments) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    const float PI = 3.14159265359f;
    float halfHeight = height * 0.5f;
    
    // Side vertices
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = std::cos(theta);
        float z = std::sin(theta);
        float u = static_cast<float>(i) / static_cast<float>(segments);
        
        // Bottom
        Vertex bottom;
        bottom.position[0] = radius * x;
        bottom.position[1] = -halfHeight;
        bottom.position[2] = radius * z;
        bottom.normal[0] = x;
        bottom.normal[1] = 0;
        bottom.normal[2] = z;
        bottom.uv[0] = u;
        bottom.uv[1] = 0;
        vertices.push_back(bottom);
        
        // Top
        Vertex top;
        top.position[0] = radius * x;
        top.position[1] = halfHeight;
        top.position[2] = radius * z;
        top.normal[0] = x;
        top.normal[1] = 0;
        top.normal[2] = z;
        top.uv[0] = u;
        top.uv[1] = 1;
        vertices.push_back(top);
    }
    
    // Side indices
    for (int i = 0; i < segments; ++i) {
        uint32_t bottom0 = i * 2;
        uint32_t top0 = i * 2 + 1;
        uint32_t bottom1 = (i + 1) * 2;
        uint32_t top1 = (i + 1) * 2 + 1;
        
        indices.push_back(bottom0);
        indices.push_back(bottom1);
        indices.push_back(top0);
        
        indices.push_back(top0);
        indices.push_back(bottom1);
        indices.push_back(top1);
    }
    
    // Top cap center
    uint32_t topCenter = static_cast<uint32_t>(vertices.size());
    Vertex topCenterVert;
    topCenterVert.position[0] = 0;
    topCenterVert.position[1] = halfHeight;
    topCenterVert.position[2] = 0;
    topCenterVert.normal[0] = 0;
    topCenterVert.normal[1] = 1;
    topCenterVert.normal[2] = 0;
    topCenterVert.uv[0] = 0.5f;
    topCenterVert.uv[1] = 0.5f;
    vertices.push_back(topCenterVert);
    
    // Bottom cap center
    uint32_t bottomCenter = static_cast<uint32_t>(vertices.size());
    Vertex bottomCenterVert;
    bottomCenterVert.position[0] = 0;
    bottomCenterVert.position[1] = -halfHeight;
    bottomCenterVert.position[2] = 0;
    bottomCenterVert.normal[0] = 0;
    bottomCenterVert.normal[1] = -1;
    bottomCenterVert.normal[2] = 0;
    bottomCenterVert.uv[0] = 0.5f;
    bottomCenterVert.uv[1] = 0.5f;
    vertices.push_back(bottomCenterVert);
    
    // Cap vertices and indices
    uint32_t capStart = static_cast<uint32_t>(vertices.size());
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = std::cos(theta);
        float z = std::sin(theta);
        
        // Top cap
        Vertex topCapVert;
        topCapVert.position[0] = radius * x;
        topCapVert.position[1] = halfHeight;
        topCapVert.position[2] = radius * z;
        topCapVert.normal[0] = 0;
        topCapVert.normal[1] = 1;
        topCapVert.normal[2] = 0;
        topCapVert.uv[0] = x * 0.5f + 0.5f;
        topCapVert.uv[1] = z * 0.5f + 0.5f;
        vertices.push_back(topCapVert);
        
        // Bottom cap
        Vertex bottomCapVert;
        bottomCapVert.position[0] = radius * x;
        bottomCapVert.position[1] = -halfHeight;
        bottomCapVert.position[2] = radius * z;
        bottomCapVert.normal[0] = 0;
        bottomCapVert.normal[1] = -1;
        bottomCapVert.normal[2] = 0;
        bottomCapVert.uv[0] = x * 0.5f + 0.5f;
        bottomCapVert.uv[1] = z * 0.5f + 0.5f;
        vertices.push_back(bottomCapVert);
    }
    
    // Cap indices
    for (int i = 0; i < segments; ++i) {
        // Top cap
        indices.push_back(topCenter);
        indices.push_back(capStart + i * 2);
        indices.push_back(capStart + (i + 1) * 2);
        
        // Bottom cap
        indices.push_back(bottomCenter);
        indices.push_back(capStart + (i + 1) * 2 + 1);
        indices.push_back(capStart + i * 2 + 1);
    }
    
    Mesh mesh;
    uploadMesh(device, mesh, vertices, indices);
    return mesh;
}

inline Mesh GeometryGenerator::createQuad(GraphicsDevice& device) {
    std::vector<Vertex> vertices = {
        {{-1, -1, 0}, {0, 0, 1}, {0, 0}},
        {{ 1, -1, 0}, {0, 0, 1}, {1, 0}},
        {{ 1,  1, 0}, {0, 0, 1}, {1, 1}},
        {{-1,  1, 0}, {0, 0, 1}, {0, 1}},
    };
    
    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
    
    Mesh mesh;
    uploadMesh(device, mesh, vertices, indices);
    return mesh;
}

} // namespace kimoyooju

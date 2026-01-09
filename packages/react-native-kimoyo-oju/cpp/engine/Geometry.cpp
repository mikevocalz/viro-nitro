#include "Geometry.hpp"
#include <cmath>
#include <memory>
#include <android/log.h>

#define LOG_TAG "KimoyoOjuGeometry"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace kimoyooju {

Geometry::Geometry() = default;

Geometry::~Geometry() {
    if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
    if (nbo_ != 0) glDeleteBuffers(1, &nbo_);
    if (ebo_ != 0) glDeleteBuffers(1, &ebo_);
}

void Geometry::draw() {
    // Create GL buffers on first draw (must be on GL thread)
    if (hasPendingData_ && !initialized_) {
        createGLBuffers();
    }
    
    if (!initialized_) return;
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount_), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Geometry::setData(const std::vector<float>& vertices,
                       const std::vector<float>& normals,
                       const std::vector<unsigned int>& indices) {
    // Store data for deferred GL buffer creation
    pendingVertices_ = vertices;
    pendingNormals_ = normals;
    pendingIndices_ = indices;
    hasPendingData_ = true;
}

void Geometry::createGLBuffers() {
    if (!hasPendingData_ || initialized_) return;
    
    LOGI("Creating GL buffers: %zu vertices, %zu indices", pendingVertices_.size() / 3, pendingIndices_.size());
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &nbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, pendingVertices_.size() * sizeof(float), pendingVertices_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, nbo_);
    glBufferData(GL_ARRAY_BUFFER, pendingNormals_.size() * sizeof(float), pendingNormals_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pendingIndices_.size() * sizeof(unsigned int), pendingIndices_.data(), GL_STATIC_DRAW);

    indexCount_ = pendingIndices_.size();
    initialized_ = true;

    glBindVertexArray(0);
    
    // Clear pending data to free memory
    pendingVertices_.clear();
    pendingNormals_.clear();
    pendingIndices_.clear();
    hasPendingData_ = false;
}

std::shared_ptr<Geometry> Geometry::createBox(float width, float height, float length) {
    auto geometry = std::make_shared<Geometry>();
    
    float hw = width / 2.0f;
    float hh = height / 2.0f;
    float hl = length / 2.0f;

    std::vector<float> vertices = {
        // Front
        -hw, -hh,  hl,   hw, -hh,  hl,   hw,  hh,  hl,  -hw,  hh,  hl,
        // Back
         hw, -hh, -hl,  -hw, -hh, -hl,  -hw,  hh, -hl,   hw,  hh, -hl,
        // Top
        -hw,  hh,  hl,   hw,  hh,  hl,   hw,  hh, -hl,  -hw,  hh, -hl,
        // Bottom
        -hw, -hh, -hl,   hw, -hh, -hl,   hw, -hh,  hl,  -hw, -hh,  hl,
        // Right
         hw, -hh,  hl,   hw, -hh, -hl,   hw,  hh, -hl,   hw,  hh,  hl,
        // Left
        -hw, -hh, -hl,  -hw, -hh,  hl,  -hw,  hh,  hl,  -hw,  hh, -hl
    };

    std::vector<float> normals = {
        // Front
        0, 0, 1,  0, 0, 1,  0, 0, 1,  0, 0, 1,
        // Back
        0, 0, -1,  0, 0, -1,  0, 0, -1,  0, 0, -1,
        // Top
        0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,
        // Bottom
        0, -1, 0,  0, -1, 0,  0, -1, 0,  0, -1, 0,
        // Right
        1, 0, 0,  1, 0, 0,  1, 0, 0,  1, 0, 0,
        // Left
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };

    geometry->setData(vertices, normals, indices);
    return geometry;
}

std::shared_ptr<Geometry> Geometry::createSphere(float radius, int segments) {
    auto geometry = std::make_shared<Geometry>();
    
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned int> indices;

    for (int y = 0; y <= segments; y++) {
        for (int x = 0; x <= segments; x++) {
            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)segments;
            float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            float yPos = std::cos(ySegment * M_PI);
            float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

            vertices.push_back(xPos * radius);
            vertices.push_back(yPos * radius);
            vertices.push_back(zPos * radius);

            normals.push_back(xPos);
            normals.push_back(yPos);
            normals.push_back(zPos);
        }
    }

    for (int y = 0; y < segments; y++) {
        for (int x = 0; x < segments; x++) {
            unsigned int first = y * (segments + 1) + x;
            unsigned int second = first + segments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    geometry->setData(vertices, normals, indices);
    return geometry;
}

std::shared_ptr<Geometry> Geometry::createPlane(float width, float height) {
    auto geometry = std::make_shared<Geometry>();
    
    float hw = width / 2.0f;
    float hh = height / 2.0f;

    std::vector<float> vertices = {
        -hw, 0, -hh,
         hw, 0, -hh,
         hw, 0,  hh,
        -hw, 0,  hh
    };

    std::vector<float> normals = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0
    };

    geometry->setData(vertices, normals, indices);
    return geometry;
}

} // namespace kimoyooju

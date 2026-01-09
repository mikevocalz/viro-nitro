#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include <glm/glm.hpp>

namespace kimoyooju {

enum class GeometryType {
    Box,
    Sphere,
    Plane,
    Cylinder
};

class Geometry {
public:
    Geometry();
    virtual ~Geometry();

    virtual void draw();

    static std::shared_ptr<Geometry> createBox(float width, float height, float length);
    static std::shared_ptr<Geometry> createSphere(float radius, int segments = 32);
    static std::shared_ptr<Geometry> createPlane(float width, float height);

protected:
    void setData(const std::vector<float>& vertices, 
                 const std::vector<float>& normals,
                 const std::vector<unsigned int>& indices);
    void createGLBuffers();

    std::vector<float> pendingVertices_;
    std::vector<float> pendingNormals_;
    std::vector<unsigned int> pendingIndices_;
    bool hasPendingData_ = false;
    
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint nbo_ = 0;
    GLuint ebo_ = 0;
    size_t indexCount_ = 0;
    bool initialized_ = false;
};

} // namespace kimoyooju

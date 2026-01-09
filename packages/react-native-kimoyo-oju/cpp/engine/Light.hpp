#pragma once

#include <glm/glm.hpp>

namespace kimoyooju {

enum class LightType {
    Ambient,
    Directional,
    Point,
    Spot
};

class Light {
public:
    Light(LightType type = LightType::Directional);

    void setType(LightType type) { type_ = type; }
    LightType getType() const { return type_; }

    void setColor(const glm::vec3& color) { color_ = color; }
    glm::vec3 getColor() const { return color_; }

    void setIntensity(float intensity) { intensity_ = intensity; }
    float getIntensity() const { return intensity_; }

    void setDirection(const glm::vec3& direction) { direction_ = glm::normalize(direction); }
    glm::vec3 getDirection() const { return direction_; }

    void setPosition(const glm::vec3& position) { position_ = position; }
    glm::vec3 getPosition() const { return position_; }

    void setRange(float range) { range_ = range; }
    float getRange() const { return range_; }

    void setInnerConeAngle(float angle) { innerConeAngle_ = angle; }
    float getInnerConeAngle() const { return innerConeAngle_; }

    void setOuterConeAngle(float angle) { outerConeAngle_ = angle; }
    float getOuterConeAngle() const { return outerConeAngle_; }

private:
    LightType type_;
    glm::vec3 color_{1.0f, 1.0f, 1.0f};
    float intensity_ = 1.0f;
    glm::vec3 direction_{0.0f, -1.0f, 0.0f};
    glm::vec3 position_{0.0f, 0.0f, 0.0f};
    float range_ = 10.0f;
    float innerConeAngle_ = 30.0f;
    float outerConeAngle_ = 45.0f;
};

} // namespace kimoyooju

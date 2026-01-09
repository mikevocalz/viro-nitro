#include "kimoyooju/CommandBuffer.hpp"
#include <cmath>

namespace kimoyooju {

CommandBufferValidator::ValidationResult CommandBufferValidator::validate(const CommandBuffer& buffer) const {
    // 1. Version check
    if (buffer.version != constants::COMMAND_BUFFER_VERSION) {
        return ValidationResult::failure(
            "INVALID_VERSION",
            "Expected version " + std::to_string(constants::COMMAND_BUFFER_VERSION) + 
            ", got " + std::to_string(buffer.version),
            0
        );
    }
    
    // 2. Size limits
    if (buffer.commands.size() > constants::MAX_COMMANDS_PER_BUFFER) {
        return ValidationResult::failure(
            "TOO_MANY_COMMANDS",
            "Max " + std::to_string(constants::MAX_COMMANDS_PER_BUFFER) + 
            " commands, got " + std::to_string(buffer.commands.size()),
            0
        );
    }
    
    // 3. Per-command validation
    for (size_t i = 0; i < buffer.commands.size(); ++i) {
        auto result = validateCommand(buffer.commands[i], i);
        if (!result.valid) {
            return result;
        }
    }
    
    return ValidationResult::success();
}

CommandBufferValidator::ValidationResult 
CommandBufferValidator::validateCommand(const Command& cmd, size_t index) const {
    return std::visit([this, index](const auto& c) -> ValidationResult {
        using T = std::decay_t<decltype(c)>;
        
        if constexpr (std::is_same_v<T, CreateNodeCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in CreateNode", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, DestroyNodeCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in DestroyNode", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, SetTransformCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in SetTransform", index);
            }
            if (!validateVec3(c.position)) {
                return ValidationResult::failure("INVALID_POSITION", "Position contains NaN or Infinity", index);
            }
            if (!validateQuat(c.rotation)) {
                return ValidationResult::failure("INVALID_ROTATION", "Rotation contains NaN or Infinity", index);
            }
            if (!validateVec3(c.scale)) {
                return ValidationResult::failure("INVALID_SCALE", "Scale contains NaN or Infinity", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, SetVisibilityCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in SetVisibility", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, ReparentCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in Reparent", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, SetMaterialCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in SetMaterial", index);
            }
            if (!validateColor(c.diffuseColor)) {
                return ValidationResult::failure("INVALID_COLOR", "Diffuse color invalid", index);
            }
            if (!validateColor(c.specularColor)) {
                return ValidationResult::failure("INVALID_COLOR", "Specular color invalid", index);
            }
            if (!validateFloat(c.shininess)) {
                return ValidationResult::failure("INVALID_FLOAT", "Shininess is NaN or Infinity", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, SetLightCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in SetLight", index);
            }
            if (!validateColor(c.color)) {
                return ValidationResult::failure("INVALID_COLOR", "Light color invalid", index);
            }
            if (!validateFloat(c.intensity) || c.intensity < 0) {
                return ValidationResult::failure("INVALID_INTENSITY", "Light intensity invalid", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, SetCameraCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in SetCamera", index);
            }
            if (!validateFloat(c.fov) || c.fov <= 0 || c.fov >= 180) {
                return ValidationResult::failure("INVALID_FOV", "FOV must be between 0 and 180", index);
            }
            if (!validateFloat(c.nearClip) || c.nearClip <= 0) {
                return ValidationResult::failure("INVALID_NEAR_CLIP", "Near clip must be positive", index);
            }
            if (!validateFloat(c.farClip) || c.farClip <= c.nearClip) {
                return ValidationResult::failure("INVALID_FAR_CLIP", "Far clip must be greater than near", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, LoadAssetCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in LoadAsset", index);
            }
            if (!validateString(c.uri, constants::MAX_STRING_LENGTH)) {
                return ValidationResult::failure("INVALID_URI", "URI too long or empty", index);
            }
            if (!validateString(c.assetType, 32)) {
                return ValidationResult::failure("INVALID_ASSET_TYPE", "Asset type invalid", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, SetTextCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in SetText", index);
            }
            if (!validateString(c.text, constants::MAX_STRING_LENGTH)) {
                return ValidationResult::failure("INVALID_TEXT", "Text too long", index);
            }
            if (!validateFloat(c.fontSize) || c.fontSize <= 0) {
                return ValidationResult::failure("INVALID_FONT_SIZE", "Font size must be positive", index);
            }
            return ValidationResult::success();
        }
        else if constexpr (std::is_same_v<T, SetGeometryCmd>) {
            if (!validateHandle(c.handle)) {
                return ValidationResult::failure("INVALID_HANDLE", "Invalid handle in SetGeometry", index);
            }
            if (!validateString(c.geometryType, 32)) {
                return ValidationResult::failure("INVALID_GEOMETRY_TYPE", "Geometry type invalid", index);
            }
            if (!validateVec3(c.dimensions)) {
                return ValidationResult::failure("INVALID_DIMENSIONS", "Dimensions invalid", index);
            }
            return ValidationResult::success();
        }
        else {
            return ValidationResult::success();
        }
    }, cmd);
}

bool CommandBufferValidator::validateHandle(const Handle& h) const {
    // Handle ID 0 is reserved but may be valid for "no parent" cases
    // Generation 0 is always invalid
    return h.generation > 0;
}

bool CommandBufferValidator::validateFloat(float f) const {
    return !std::isnan(f) && !std::isinf(f);
}

bool CommandBufferValidator::validateVec3(const Vec3& v) const {
    return validateFloat(v[0]) && validateFloat(v[1]) && validateFloat(v[2]);
}

bool CommandBufferValidator::validateQuat(const Quat& q) const {
    if (!validateFloat(q[0]) || !validateFloat(q[1]) || 
        !validateFloat(q[2]) || !validateFloat(q[3])) {
        return false;
    }
    // Quaternion should be normalized (length ~= 1)
    float len = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
    return len > 0.9f && len < 1.1f; // Allow some tolerance
}

bool CommandBufferValidator::validateColor(const Color& c) const {
    for (int i = 0; i < 4; ++i) {
        if (!validateFloat(c[i]) || c[i] < 0.0f || c[i] > 1.0f) {
            return false;
        }
    }
    return true;
}

bool CommandBufferValidator::validateString(const std::string& s, size_t maxLen) const {
    return !s.empty() && s.size() <= maxLen;
}

} // namespace kimoyooju

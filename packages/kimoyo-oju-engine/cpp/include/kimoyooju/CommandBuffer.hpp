#pragma once

#include "kimoyooju/Types.hpp"
#include <vector>
#include <string>
#include <variant>

namespace kimoyooju {

struct CreateNodeCmd {
    Handle handle;
    NodeType nodeType;
    Handle parentHandle;
};

struct DestroyNodeCmd {
    Handle handle;
};

struct SetTransformCmd {
    Handle handle;
    Vec3 position;
    Quat rotation;
    Vec3 scale;
};

struct SetVisibilityCmd {
    Handle handle;
    bool visible;
};

struct ReparentCmd {
    Handle handle;
    Handle newParentHandle;
};

struct SetMaterialCmd {
    Handle handle;
    Color diffuseColor;
    Color specularColor;
    float shininess;
    Handle diffuseTexture;
    Handle normalTexture;
};

struct SetLightCmd {
    Handle handle;
    LightType lightType;
    Color color;
    float intensity;
    float range;
    float innerConeAngle;
    float outerConeAngle;
};

struct SetCameraCmd {
    Handle handle;
    float fov;
    float nearClip;
    float farClip;
};

struct LoadAssetCmd {
    Handle handle;
    std::string uri;
    std::string assetType; // "model", "texture", "audio"
};

struct SetTextCmd {
    Handle handle;
    std::string text;
    float fontSize;
    Color color;
    std::string fontFamily;
};

struct SetGeometryCmd {
    Handle handle;
    std::string geometryType; // "box", "sphere", "plane", "cylinder"
    Vec3 dimensions;
};

using Command = std::variant<
    CreateNodeCmd,
    DestroyNodeCmd,
    SetTransformCmd,
    SetVisibilityCmd,
    ReparentCmd,
    SetMaterialCmd,
    SetLightCmd,
    SetCameraCmd,
    LoadAssetCmd,
    SetTextCmd,
    SetGeometryCmd
>;

struct CommandBuffer {
    uint32_t version = constants::COMMAND_BUFFER_VERSION;
    uint64_t txId = 0;
    uint64_t timestamp = 0;
    std::vector<Command> commands;
    
    void clear() {
        commands.clear();
        txId = 0;
        timestamp = 0;
    }
    
    size_t size() const {
        return commands.size();
    }
    
    bool empty() const {
        return commands.empty();
    }
};

class CommandBufferValidator {
public:
    struct ValidationResult {
        bool valid = true;
        std::string errorCode;
        std::string errorMessage;
        size_t errorCommandIndex = 0;
        
        static ValidationResult success() {
            return ValidationResult{true, "", "", 0};
        }
        
        static ValidationResult failure(std::string code, std::string message, size_t index) {
            return ValidationResult{false, std::move(code), std::move(message), index};
        }
    };
    
    ValidationResult validate(const CommandBuffer& buffer) const;
    
private:
    ValidationResult validateCommand(const Command& cmd, size_t index) const;
    bool validateHandle(const Handle& h) const;
    bool validateFloat(float f) const;
    bool validateVec3(const Vec3& v) const;
    bool validateQuat(const Quat& q) const;
    bool validateColor(const Color& c) const;
    bool validateString(const std::string& s, size_t maxLen) const;
};

} // namespace kimoyooju

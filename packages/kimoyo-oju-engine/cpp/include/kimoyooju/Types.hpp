#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <optional>
#include <array>

namespace kimoyooju {

struct Handle {
    uint32_t id = 0;
    uint32_t generation = 0;
    
    bool operator==(const Handle& other) const {
        return id == other.id && generation == other.generation;
    }
    
    bool operator!=(const Handle& other) const {
        return !(*this == other);
    }
    
    bool isValid() const {
        return id != 0;
    }
    
    static Handle invalid() {
        return Handle{0, 0};
    }
};

struct Error {
    std::string code;
    std::string message;
    
    Error() = default;
    Error(std::string c, std::string m) : code(std::move(c)), message(std::move(m)) {}
};

template<typename T>
struct Result {
    bool ok;
    std::variant<T, Error> data;
    
    static Result success(T value) {
        Result r;
        r.ok = true;
        r.data = std::move(value);
        return r;
    }
    
    static Result failure(std::string code, std::string message) {
        Result r;
        r.ok = false;
        r.data = Error{std::move(code), std::move(message)};
        return r;
    }
    
    const T& value() const {
        return std::get<T>(data);
    }
    
    T& value() {
        return std::get<T>(data);
    }
    
    const Error& error() const {
        return std::get<Error>(data);
    }
};

template<>
struct Result<void> {
    bool ok;
    std::optional<Error> error_;
    
    static Result success() {
        Result r;
        r.ok = true;
        return r;
    }
    
    static Result failure(std::string code, std::string message) {
        Result r;
        r.ok = false;
        r.error_ = Error{std::move(code), std::move(message)};
        return r;
    }
    
    const Error& error() const {
        return *error_;
    }
};

enum class RendererState : uint8_t {
    CREATED,
    RUNNING,
    PAUSED,
    SURFACE_LOST,
    DESTROYED
};

enum class XRMode : uint8_t {
    FLAT,
    IMMERSIVE_VR,
    IMMERSIVE_MR
};

enum class XRSessionState : uint8_t {
    IDLE,
    READY,
    SYNCHRONIZED,
    VISIBLE,
    FOCUSED,
    STOPPING,
    LOSS_PENDING,
    EXITING
};

enum class NodeType : uint8_t {
    GROUP,
    MESH,
    LIGHT,
    CAMERA,
    TEXT,
    MODEL
};

enum class LightType : uint8_t {
    AMBIENT,
    DIRECTIONAL,
    POINT,
    SPOT
};

enum class EngineEvent : uint8_t {
    SURFACE_CREATED,
    SURFACE_DESTROYED,
    XR_SESSION_STARTED,
    XR_SESSION_ENDED,
    XR_SESSION_LOST,
    ERROR
};

using Vec3 = std::array<float, 3>;
using Vec4 = std::array<float, 4>;
using Quat = std::array<float, 4>;  // x, y, z, w
using Mat4 = std::array<float, 16>;
using Color = std::array<float, 4>; // r, g, b, a

struct Transform {
    Vec3 position = {0.0f, 0.0f, 0.0f};
    Quat rotation = {0.0f, 0.0f, 0.0f, 1.0f};
    Vec3 scale = {1.0f, 1.0f, 1.0f};
};

namespace constants {
    constexpr uint32_t MAX_COMMANDS_PER_BUFFER = 10000;
    constexpr uint32_t MAX_STRING_LENGTH = 4096;
    constexpr uint32_t MAX_NODES = 100000;
    constexpr uint32_t MAX_TEXTURES = 1000;
    constexpr uint32_t COMMAND_BUFFER_VERSION = 1;
    constexpr int64_t SWAPCHAIN_WAIT_TIMEOUT_NS = 100'000'000;  // 100ms
}

} // namespace kimoyooju

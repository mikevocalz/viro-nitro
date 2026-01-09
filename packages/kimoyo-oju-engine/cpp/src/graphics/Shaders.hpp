#pragma once

#include <string>

namespace kimoyooju {

// ============================================================================
// Default PBR Vertex Shader
// ============================================================================
const std::string DEFAULT_VERTEX_SHADER = R"(#version 300 es
precision highp float;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_normalMatrix;

out vec3 v_worldPosition;
out vec3 v_normal;
out vec2 v_texCoord;

void main() {
    vec4 worldPos = u_modelMatrix * vec4(a_position, 1.0);
    v_worldPosition = worldPos.xyz;
    v_normal = mat3(u_normalMatrix) * a_normal;
    v_texCoord = a_texCoord;
    
    gl_Position = u_projectionMatrix * u_viewMatrix * worldPos;
}
)";

// ============================================================================
// Default PBR Fragment Shader
// ============================================================================
const std::string DEFAULT_FRAGMENT_SHADER = R"(#version 300 es
precision highp float;

in vec3 v_worldPosition;
in vec3 v_normal;
in vec2 v_texCoord;

uniform vec4 u_diffuseColor;
uniform vec4 u_specularColor;
uniform float u_shininess;
uniform vec3 u_cameraPosition;

// Lighting
uniform vec3 u_ambientLight;
uniform vec3 u_lightDirection;
uniform vec3 u_lightColor;
uniform float u_lightIntensity;

// Textures
uniform sampler2D u_diffuseTexture;
uniform bool u_hasDiffuseTexture;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(u_cameraPosition - v_worldPosition);
    vec3 lightDir = normalize(-u_lightDirection);
    
    // Ambient
    vec3 ambient = u_ambientLight * u_diffuseColor.rgb;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor * u_lightIntensity * u_diffuseColor.rgb;
    
    // Specular (Blinn-Phong)
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), u_shininess);
    vec3 specular = spec * u_lightColor * u_lightIntensity * u_specularColor.rgb;
    
    // Combine
    vec3 result = ambient + diffuse + specular;
    
    // Apply texture if available
    if (u_hasDiffuseTexture) {
        vec4 texColor = texture(u_diffuseTexture, v_texCoord);
        result *= texColor.rgb;
    }
    
    fragColor = vec4(result, u_diffuseColor.a);
}
)";

// ============================================================================
// Unlit Vertex Shader (for simple colored objects)
// ============================================================================
const std::string UNLIT_VERTEX_SHADER = R"(#version 300 es
precision highp float;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec2 v_texCoord;

void main() {
    v_texCoord = a_texCoord;
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0);
}
)";

// ============================================================================
// Unlit Fragment Shader
// ============================================================================
const std::string UNLIT_FRAGMENT_SHADER = R"(#version 300 es
precision highp float;

in vec2 v_texCoord;

uniform vec4 u_diffuseColor;
uniform sampler2D u_diffuseTexture;
uniform bool u_hasDiffuseTexture;

out vec4 fragColor;

void main() {
    vec4 color = u_diffuseColor;
    
    if (u_hasDiffuseTexture) {
        color *= texture(u_diffuseTexture, v_texCoord);
    }
    
    fragColor = color;
}
)";

// ============================================================================
// Skybox Vertex Shader
// ============================================================================
const std::string SKYBOX_VERTEX_SHADER = R"(#version 300 es
precision highp float;

layout(location = 0) in vec3 a_position;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec3 v_texCoord;

void main() {
    v_texCoord = a_position;
    vec4 pos = u_projectionMatrix * mat4(mat3(u_viewMatrix)) * vec4(a_position, 1.0);
    gl_Position = pos.xyww;
}
)";

// ============================================================================
// Skybox Fragment Shader
// ============================================================================
const std::string SKYBOX_FRAGMENT_SHADER = R"(#version 300 es
precision highp float;

in vec3 v_texCoord;

uniform samplerCube u_skyboxTexture;
uniform vec4 u_skyboxColor;
uniform bool u_hasSkyboxTexture;

out vec4 fragColor;

void main() {
    if (u_hasSkyboxTexture) {
        fragColor = texture(u_skyboxTexture, v_texCoord);
    } else {
        fragColor = u_skyboxColor;
    }
}
)";

// ============================================================================
// Text Vertex Shader
// ============================================================================
const std::string TEXT_VERTEX_SHADER = R"(#version 300 es
precision highp float;

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texCoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec2 v_texCoord;

void main() {
    v_texCoord = a_texCoord;
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0);
}
)";

// ============================================================================
// Text Fragment Shader (SDF-based)
// ============================================================================
const std::string TEXT_FRAGMENT_SHADER = R"(#version 300 es
precision highp float;

in vec2 v_texCoord;

uniform sampler2D u_fontTexture;
uniform vec4 u_textColor;

out vec4 fragColor;

void main() {
    float distance = texture(u_fontTexture, v_texCoord).a;
    float smoothWidth = fwidth(distance);
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);
    fragColor = vec4(u_textColor.rgb, u_textColor.a * alpha);
}
)";

} // namespace kimoyooju

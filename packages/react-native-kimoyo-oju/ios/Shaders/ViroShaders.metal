#include <metal_stdlib>
using namespace metal;

// ============================================================================
// Vertex Input/Output Structures
// ============================================================================

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 worldPosition;
    float3 normal;
    float2 texCoord;
};

// ============================================================================
// Uniform Buffers
// ============================================================================

struct Uniforms {
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 normalMatrix;
    float4 diffuseColor;
    float4 specularColor;
    float shininess;
    float3 cameraPosition;
};

struct LightUniforms {
    float3 ambientLight;
    float _pad1;
    float3 lightDirection;
    float _pad2;
    float3 lightColor;
    float lightIntensity;
};

// ============================================================================
// Default PBR Shader
// ============================================================================

vertex VertexOut vertexMain(VertexIn in [[stage_in]],
                            constant Uniforms& uniforms [[buffer(1)]]) {
    VertexOut out;
    
    float4 worldPos = uniforms.modelMatrix * float4(in.position, 1.0);
    out.worldPosition = worldPos.xyz;
    out.normal = (uniforms.normalMatrix * float4(in.normal, 0.0)).xyz;
    out.texCoord = in.texCoord;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    
    return out;
}

fragment float4 fragmentMain(VertexOut in [[stage_in]],
                             constant Uniforms& uniforms [[buffer(1)]],
                             constant LightUniforms& lights [[buffer(2)]],
                             texture2d<float> diffuseTexture [[texture(0)]],
                             sampler textureSampler [[sampler(0)]]) {
    
    float3 normal = normalize(in.normal);
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPosition);
    float3 lightDir = normalize(-lights.lightDirection);
    
    // Ambient
    float3 ambient = lights.ambientLight * uniforms.diffuseColor.rgb;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * lights.lightColor * lights.lightIntensity * uniforms.diffuseColor.rgb;
    
    // Specular (Blinn-Phong)
    float3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), uniforms.shininess);
    float3 specular = spec * lights.lightColor * lights.lightIntensity * uniforms.specularColor.rgb;
    
    // Combine
    float3 result = ambient + diffuse + specular;
    
    return float4(result, uniforms.diffuseColor.a);
}

// ============================================================================
// Unlit Shader
// ============================================================================

vertex VertexOut unlitVertexMain(VertexIn in [[stage_in]],
                                  constant Uniforms& uniforms [[buffer(1)]]) {
    VertexOut out;
    
    out.texCoord = in.texCoord;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * float4(in.position, 1.0);
    out.worldPosition = float3(0);
    out.normal = float3(0);
    
    return out;
}

fragment float4 unlitFragmentMain(VertexOut in [[stage_in]],
                                   constant Uniforms& uniforms [[buffer(1)]],
                                   texture2d<float> diffuseTexture [[texture(0)]],
                                   sampler textureSampler [[sampler(0)]]) {
    return uniforms.diffuseColor;
}

// ============================================================================
// Textured Shader
// ============================================================================

fragment float4 texturedFragmentMain(VertexOut in [[stage_in]],
                                      constant Uniforms& uniforms [[buffer(1)]],
                                      constant LightUniforms& lights [[buffer(2)]],
                                      texture2d<float> diffuseTexture [[texture(0)]],
                                      sampler textureSampler [[sampler(0)]]) {
    
    float4 texColor = diffuseTexture.sample(textureSampler, in.texCoord);
    
    float3 normal = normalize(in.normal);
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPosition);
    float3 lightDir = normalize(-lights.lightDirection);
    
    // Ambient
    float3 ambient = lights.ambientLight * texColor.rgb;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * lights.lightColor * lights.lightIntensity * texColor.rgb;
    
    // Specular
    float3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), uniforms.shininess);
    float3 specular = spec * lights.lightColor * lights.lightIntensity * uniforms.specularColor.rgb;
    
    float3 result = ambient + diffuse + specular;
    
    return float4(result, texColor.a * uniforms.diffuseColor.a);
}

// ============================================================================
// AR Camera Background Shader
// ============================================================================

struct ARCameraVertexOut {
    float4 position [[position]];
    float2 texCoord;
};

vertex ARCameraVertexOut arCameraVertexMain(uint vertexID [[vertex_id]]) {
    // Full-screen quad
    float2 positions[4] = {
        float2(-1, -1),
        float2( 1, -1),
        float2(-1,  1),
        float2( 1,  1)
    };
    
    float2 texCoords[4] = {
        float2(0, 1),
        float2(1, 1),
        float2(0, 0),
        float2(1, 0)
    };
    
    ARCameraVertexOut out;
    out.position = float4(positions[vertexID], 0, 1);
    out.texCoord = texCoords[vertexID];
    
    return out;
}

fragment float4 arCameraFragmentMain(ARCameraVertexOut in [[stage_in]],
                                      texture2d<float> cameraTextureY [[texture(0)]],
                                      texture2d<float> cameraTextureCbCr [[texture(1)]],
                                      sampler textureSampler [[sampler(0)]]) {
    
    // YCbCr to RGB conversion
    float y = cameraTextureY.sample(textureSampler, in.texCoord).r;
    float2 cbcr = cameraTextureCbCr.sample(textureSampler, in.texCoord).rg;
    
    // BT.601 conversion matrix
    float3x3 ycbcrToRGB = float3x3(
        float3(1.0, 1.0, 1.0),
        float3(0.0, -0.344, 1.772),
        float3(1.402, -0.714, 0.0)
    );
    
    float3 ycbcr = float3(y, cbcr.x - 0.5, cbcr.y - 0.5);
    float3 rgb = ycbcrToRGB * ycbcr;
    
    return float4(rgb, 1.0);
}

// ============================================================================
// Skybox Shader
// ============================================================================

struct SkyboxVertexOut {
    float4 position [[position]];
    float3 texCoord;
};

vertex SkyboxVertexOut skyboxVertexMain(VertexIn in [[stage_in]],
                                         constant Uniforms& uniforms [[buffer(1)]]) {
    SkyboxVertexOut out;
    
    // Remove translation from view matrix
    float4x4 viewNoTranslation = uniforms.viewMatrix;
    viewNoTranslation[3] = float4(0, 0, 0, 1);
    
    float4 pos = uniforms.projectionMatrix * viewNoTranslation * float4(in.position, 1.0);
    out.position = pos.xyww; // Set z = w for depth = 1
    out.texCoord = in.position;
    
    return out;
}

fragment float4 skyboxFragmentMain(SkyboxVertexOut in [[stage_in]],
                                    texturecube<float> skyboxTexture [[texture(0)]],
                                    sampler textureSampler [[sampler(0)]]) {
    
    return skyboxTexture.sample(textureSampler, in.texCoord);
}

// ============================================================================
// Text SDF Shader
// ============================================================================

fragment float4 textFragmentMain(VertexOut in [[stage_in]],
                                  constant Uniforms& uniforms [[buffer(1)]],
                                  texture2d<float> fontTexture [[texture(0)]],
                                  sampler textureSampler [[sampler(0)]]) {
    
    float distance = fontTexture.sample(textureSampler, in.texCoord).a;
    
    // SDF smoothing
    float smoothWidth = fwidth(distance);
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);
    
    return float4(uniforms.diffuseColor.rgb, uniforms.diffuseColor.a * alpha);
}

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "kimoyooju/Engine.hpp"
#include "graphics/MetalDevice.h"
#include "graphics/SceneRenderer.hpp"
#include "xr/ARKitSession.h"

namespace {

struct KimoyoOjuContext {
    std::shared_ptr<kimoyooju::Engine> engine;
    std::shared_ptr<kimoyooju::MetalDevice> device;
    std::shared_ptr<kimoyooju::SceneRenderer> renderer;
    std::unique_ptr<kimoyooju::SceneGraph> sceneGraph;
    std::unique_ptr<kimoyooju::ARKitSession> arSession;
    
    CAMetalLayer* metalLayer = nil;
    int width = 0;
    int height = 0;
    bool initialized = false;
    bool arEnabled = false;
    
    // AR camera matrices (set externally)
    float arViewMatrix[16];
    float arProjMatrix[16];
};

} // anonymous namespace

extern "C" {

void* kimoyooju_native_init(void* metalLayerPtr) {
    @autoreleasepool {
        NSLog(@"KimoyoOjuNativeBridge: Initializing");
        
        auto* context = new KimoyoOjuContext();
        context->metalLayer = (__bridge CAMetalLayer*)metalLayerPtr;
        
        // Create engine
        kimoyooju::EngineConfig config;
        config.enableValidation = true;
        config.maxNodes = 10000;
        config.maxTextures = 1000;
        
        context->engine = std::make_shared<kimoyooju::Engine>(config);
        context->device = std::make_shared<kimoyooju::MetalDevice>();
        context->renderer = std::make_shared<kimoyooju::SceneRenderer>();
        context->sceneGraph = std::make_unique<kimoyooju::SceneGraph>();
        
        // Initialize Metal device
        if (!context->device->initialize(metalLayerPtr)) {
            NSLog(@"KimoyoOjuNativeBridge: Failed to initialize Metal device");
            delete context;
            return nullptr;
        }
        
        // Initialize renderer
        if (!context->renderer->initialize(context->device)) {
            NSLog(@"KimoyoOjuNativeBridge: Failed to initialize renderer");
            delete context;
            return nullptr;
        }
        
        // Initialize identity matrices
        for (int i = 0; i < 16; i++) {
            context->arViewMatrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
            context->arProjMatrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        }
        
        context->initialized = true;
        NSLog(@"KimoyoOjuNativeBridge: Initialized successfully");
        
        return context;
    }
}

void kimoyooju_native_destroy(void* enginePtr) {
    @autoreleasepool {
        NSLog(@"KimoyoOjuNativeBridge: Destroying");
        
        auto* context = static_cast<KimoyoOjuContext*>(enginePtr);
        if (!context) return;
        
        if (context->arSession) {
            context->arSession->shutdown();
        }
        
        if (context->renderer) {
            context->renderer->shutdown();
        }
        
        if (context->device) {
            context->device->shutdown();
        }
        
        if (context->engine) {
            context->engine->shutdown();
        }
        
        delete context;
        NSLog(@"KimoyoOjuNativeBridge: Destroyed");
    }
}

void kimoyooju_native_surface_changed(void* enginePtr, int width, int height) {
    auto* context = static_cast<KimoyoOjuContext*>(enginePtr);
    if (!context || !context->device) return;
    
    NSLog(@"KimoyoOjuNativeBridge: Surface changed to %dx%d", width, height);
    
    context->width = width;
    context->height = height;
    context->device->setViewport(0, 0, width, height);
    
    // Update camera aspect ratio
    if (context->renderer) {
        kimoyooju::RenderCamera camera;
        camera.position = {0, 0, 5};
        camera.fov = 60.0f;
        camera.nearClip = 0.1f;
        camera.farClip = 1000.0f;
        camera.projectionMatrix = kimoyooju::mat4Perspective(60.0f,
            static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
        camera.viewMatrix = kimoyooju::mat4LookAt(camera.position, {0, 0, 0}, {0, 1, 0});
        context->renderer->setCamera(camera);
    }
}

void kimoyooju_native_draw_frame(void* enginePtr) {
    auto* context = static_cast<KimoyoOjuContext*>(enginePtr);
    if (!context || !context->initialized) return;
    
    @autoreleasepool {
        // Update camera from AR if enabled
        if (context->arEnabled && context->renderer) {
            kimoyooju::RenderCamera camera;
            memcpy(camera.viewMatrix.m, context->arViewMatrix, sizeof(float) * 16);
            memcpy(camera.projectionMatrix.m, context->arProjMatrix, sizeof(float) * 16);
            camera.position = {-context->arViewMatrix[12], -context->arViewMatrix[13], -context->arViewMatrix[14]};
            context->renderer->setCamera(camera);
        }
        
        // Begin frame
        context->device->beginFrame();
        context->renderer->beginFrame();
        
        // Render scene
        if (context->sceneGraph) {
            context->renderer->renderScene(*context->sceneGraph);
        }
        
        // End frame
        context->device->endFrame();
        context->device->swapBuffers();
    }
}

void kimoyooju_native_set_xr_mode(void* enginePtr, const char* mode) {
    auto* context = static_cast<KimoyoOjuContext*>(enginePtr);
    if (!context || !context->engine) return;
    
    NSLog(@"KimoyoOjuNativeBridge: Setting XR mode to %s", mode);
    
    std::string modeStr(mode);
    
    if (modeStr == "ar" || modeStr == "immersive-ar") {
        // Enable AR
        if (!context->arSession) {
            context->arSession = std::make_unique<kimoyooju::ARKitSession>();
            kimoyooju::ARKitSession::Config arConfig;
            arConfig.enablePlaneDetection = true;
            arConfig.enableLightEstimation = true;
            context->arSession->initialize(arConfig);
        }
        context->arSession->run();
        context->arEnabled = true;
    } else if (modeStr == "flat") {
        // Disable AR
        if (context->arSession) {
            context->arSession->pause();
        }
        context->arEnabled = false;
    }
}

void kimoyooju_native_submit_commands(void* enginePtr, const char* json) {
    auto* context = static_cast<KimoyoOjuContext*>(enginePtr);
    if (!context || !context->engine) return;
    
    // TODO: Parse JSON and submit commands to engine
    NSLog(@"KimoyoOjuNativeBridge: Received commands: %.100s...", json);
}

void kimoyooju_native_pause(void* enginePtr) {
    auto* context = static_cast<KimoyoOjuContext*>(enginePtr);
    if (!context) return;
    
    NSLog(@"KimoyoOjuNativeBridge: Pausing");
    
    if (context->engine) {
        context->engine->pause();
    }
    
    if (context->arSession) {
        context->arSession->pause();
    }
}

void kimoyooju_native_resume(void* enginePtr) {
    auto* context = static_cast<KimoyoOjuContext*>(enginePtr);
    if (!context) return;
    
    NSLog(@"KimoyoOjuNativeBridge: Resuming");
    
    if (context->engine) {
        context->engine->resume();
    }
    
    if (context->arEnabled && context->arSession) {
        context->arSession->run();
    }
}

void kimoyooju_native_set_ar_matrices(void* enginePtr, const void* viewMatrix, const void* projMatrix) {
    auto* context = static_cast<KimoyoOjuContext*>(enginePtr);
    if (!context) return;
    
    memcpy(context->arViewMatrix, viewMatrix, sizeof(float) * 16);
    memcpy(context->arProjMatrix, projMatrix, sizeof(float) * 16);
}

} // extern "C"

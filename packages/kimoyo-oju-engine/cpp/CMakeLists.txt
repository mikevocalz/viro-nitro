cmake_minimum_required(VERSION 3.18)
project(viro-engine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VIRO_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(VIRO_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(VIRO_ENABLE_OPENXR "Enable OpenXR support" ON)

# Source files
set(VIRO_ENGINE_SOURCES
    src/Engine.cpp
    src/Renderer.cpp
    src/CommandBuffer.cpp
    src/SceneGraph.cpp
    src/graphics/GLDevice.cpp
    src/graphics/SceneRenderer.cpp
    src/xr/XRSession.cpp
    src/xr/ARCoreSession.cpp
)

# Header files
set(VIRO_ENGINE_HEADERS
    include/viro/Engine.hpp
    include/viro/Renderer.hpp
    include/viro/Types.hpp
    include/viro/CommandBuffer.hpp
    include/viro/HandleRegistry.hpp
    include/viro/MemoryCounters.hpp
    include/viro/SceneGraph.hpp
    include/viro/GraphicsDevice.hpp
    include/viro/XRSession.hpp
    src/graphics/GLDevice.hpp
    src/graphics/Shaders.hpp
    src/graphics/Geometry.hpp
    src/graphics/SceneRenderer.hpp
    src/xr/ARCoreSession.hpp
)

# Create library
add_library(viro-engine STATIC ${VIRO_ENGINE_SOURCES} ${VIRO_ENGINE_HEADERS})

target_include_directories(viro-engine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(viro-engine PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -Werror=return-type
    )
endif()

# Sanitizers
if(VIRO_ENABLE_ASAN)
    target_compile_options(viro-engine PUBLIC -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(viro-engine PUBLIC -fsanitize=address)
endif()

if(VIRO_ENABLE_UBSAN)
    target_compile_options(viro-engine PUBLIC -fsanitize=undefined)
    target_link_options(viro-engine PUBLIC -fsanitize=undefined)
endif()

# Platform-specific configuration
if(ANDROID)
    # Android-specific settings
    target_compile_definitions(viro-engine PRIVATE VIRO_PLATFORM_ANDROID)
    
    if(VIRO_ENABLE_OPENXR)
        # OpenXR for Quest
        find_package(OpenXR QUIET)
        if(OpenXR_FOUND)
            target_link_libraries(viro-engine PRIVATE OpenXR::openxr_loader)
            target_compile_definitions(viro-engine PRIVATE VIRO_ENABLE_OPENXR)
        endif()
    endif()
    
    # EGL/GLES for Android
    find_library(EGL_LIBRARY EGL)
    find_library(GLES3_LIBRARY GLESv3)
    target_link_libraries(viro-engine PRIVATE ${EGL_LIBRARY} ${GLES3_LIBRARY})
    
elseif(APPLE)
    # iOS/macOS settings
    target_compile_definitions(viro-engine PRIVATE VIRO_PLATFORM_APPLE)
    
    find_library(METAL_LIBRARY Metal)
    find_library(METALKIT_LIBRARY MetalKit)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    
    target_link_libraries(viro-engine PRIVATE
        ${METAL_LIBRARY}
        ${METALKIT_LIBRARY}
        ${QUARTZCORE_LIBRARY}
    )
endif()

# Install rules
install(TARGETS viro-engine
    EXPORT viro-engine-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

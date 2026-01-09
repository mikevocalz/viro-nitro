cmake_minimum_required(VERSION 3.22.1)
project(KimoyoOjuNitro)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Package root directory
set(PACKAGE_ROOT "${CMAKE_SOURCE_DIR}/../../../..")

# Find required packages
find_library(LOG_LIB log)
find_library(ANDROID_LIB android)
find_library(GLES3_LIB GLESv3)
find_library(EGL_LIB EGL)

# Required for Nitro
find_package(fbjni REQUIRED)
find_package(ReactAndroid REQUIRED)
find_package(react-native-nitro-modules REQUIRED)

# Fetch GLM (header-only math library)
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Fetch nlohmann/json (header-only JSON library)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Create shared library with our implementation
add_library(KimoyoOjuNitro SHARED
    ${PACKAGE_ROOT}/cpp/HybridKimoyoOjuEngine.cpp
    ${PACKAGE_ROOT}/cpp/HybridKimoyoOjuEngineFactory.cpp
    ${PACKAGE_ROOT}/cpp/engine/Engine.cpp
    ${PACKAGE_ROOT}/cpp/engine/Renderer.cpp
    ${PACKAGE_ROOT}/cpp/engine/SceneNode.cpp
    ${PACKAGE_ROOT}/cpp/engine/Camera.cpp
    ${PACKAGE_ROOT}/cpp/engine/Light.cpp
    ${PACKAGE_ROOT}/cpp/engine/Geometry.cpp
    ${PACKAGE_ROOT}/nitrogen/generated/android/KimoyoOjuNitroOnLoad.cpp
    ${PACKAGE_ROOT}/nitrogen/generated/shared/c++/HybridKimoyoOjuEngineSpec.cpp
    KimoyoOjuOnLoad.cpp
    KimoyoOjuJNI.cpp
)

# Include directories
target_include_directories(KimoyoOjuNitro PRIVATE
    ${PACKAGE_ROOT}/cpp
    ${PACKAGE_ROOT}/cpp/engine
    ${PACKAGE_ROOT}/nitrogen/generated/shared/c++
    ${PACKAGE_ROOT}/nitrogen/generated/android/c++
    ${PACKAGE_ROOT}/nitrogen/generated/android
    ${CMAKE_SOURCE_DIR}
)

# Define flag for nitrogen generated code
add_definitions(-DBUILDING_KIMOYOOJUNITRO_WITH_GENERATED_CMAKE_PROJECT)

# Compiler definitions
target_compile_definitions(KimoyoOjuNitro PRIVATE
    VIRO_PLATFORM_ANDROID
    FOLLY_NO_CONFIG=1
    FOLLY_HAVE_CLOCK_GETTIME=1
    FOLLY_USE_LIBCPP=1
    FOLLY_CFG_NO_COROUTINES=1
    FOLLY_MOBILE=1
    FOLLY_HAVE_RECVMMSG=1
    FOLLY_HAVE_PTHREAD=1
    FOLLY_HAVE_XSI_STRERROR_R=1
)

# Compiler options
target_compile_options(KimoyoOjuNitro PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -fexceptions
    -frtti
)

# Link libraries
target_link_libraries(KimoyoOjuNitro
    ${LOG_LIB}
    ${ANDROID_LIB}
    ${GLES3_LIB}
    ${EGL_LIB}
    glm::glm
    nlohmann_json::nlohmann_json
    fbjni::fbjni
    ReactAndroid::jsi
    react-native-nitro-modules::NitroModules
)

# Link react-native (different prefab between RN 0.75 and RN 0.76)
if(ReactAndroid_VERSION_MINOR GREATER_EQUAL 76)
    target_link_libraries(KimoyoOjuNitro ReactAndroid::reactnative)
else()
    target_link_libraries(KimoyoOjuNitro ReactAndroid::react_nativemodule_core)
endif()

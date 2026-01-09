#include <jni.h>
#include <android/log.h>
#include <string>

#include "engine/Engine.hpp"

#define LOG_TAG "KimoyoOjuJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativeInit(JNIEnv* env, jobject thiz) {
    LOGI("KimoyoOjuSurfaceView nativeInit");
    kimoyooju::Engine::getInstance().initialize();
    return 1;
}

JNIEXPORT void JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativeDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    LOGI("KimoyoOjuSurfaceView nativeDestroy");
    kimoyooju::Engine::getInstance().onSurfaceDestroyed();
}

JNIEXPORT void JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativeSurfaceChanged(
    JNIEnv* env, jobject thiz, jlong handle, jint width, jint height) {
    
    LOGI("KimoyoOjuSurfaceView nativeSurfaceChanged: %dx%d", width, height);
    
    auto state = kimoyooju::Engine::getInstance().getState();
    if (state == kimoyooju::EngineState::Created || state == kimoyooju::EngineState::SurfaceLost) {
        kimoyooju::Engine::getInstance().onSurfaceCreated(width, height);
    } else {
        kimoyooju::Engine::getInstance().onSurfaceChanged(width, height);
    }
}

JNIEXPORT void JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativeDrawFrame(JNIEnv* env, jobject thiz, jlong handle) {
    kimoyooju::Engine::getInstance().drawFrame();
}

JNIEXPORT void JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativeSetXRMode(
    JNIEnv* env, jobject thiz, jlong handle, jstring mode) {
    
    const char* modeStr = env->GetStringUTFChars(mode, nullptr);
    std::string modeString(modeStr);
    env->ReleaseStringUTFChars(mode, modeStr);
    
    LOGI("Setting XR mode: %s", modeString.c_str());
    
    kimoyooju::XRModeType xrMode = kimoyooju::XRModeType::Flat;
    if (modeString == "immersive-vr") {
        xrMode = kimoyooju::XRModeType::ImmersiveVR;
    } else if (modeString == "immersive-mr") {
        xrMode = kimoyooju::XRModeType::ImmersiveMR;
    }
    
    kimoyooju::Engine::getInstance().setXRMode(xrMode);
}

JNIEXPORT void JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativeSubmitCommands(
    JNIEnv* env, jobject thiz, jlong handle, jstring bufferJson) {
    
    const char* jsonStr = env->GetStringUTFChars(bufferJson, nullptr);
    std::string json(jsonStr);
    env->ReleaseStringUTFChars(bufferJson, jsonStr);
    
    kimoyooju::Engine::getInstance().processCommandBuffer(json);
}

JNIEXPORT void JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativePause(JNIEnv* env, jobject thiz, jlong handle) {
    LOGI("KimoyoOjuSurfaceView nativePause");
    kimoyooju::Engine::getInstance().pause();
}

JNIEXPORT void JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativeResume(JNIEnv* env, jobject thiz, jlong handle) {
    LOGI("KimoyoOjuSurfaceView nativeResume");
    kimoyooju::Engine::getInstance().resume();
}

JNIEXPORT jintArray JNICALL
Java_com_kimoyooju_nitro_KimoyoOjuSurfaceView_nativeHandleTouch(
    JNIEnv* env, jobject thiz, jlong handle, jfloat x, jfloat y, jint action) {
    
    auto result = kimoyooju::Engine::getInstance().handleTouch(x, y, action);
    
    // Return [hit, nodeId, worldX*1000, worldY*1000, worldZ*1000]
    jintArray arr = env->NewIntArray(5);
    jint values[5] = {
        result.hit ? 1 : 0,
        static_cast<jint>(result.nodeId),
        static_cast<jint>(result.worldX * 1000),
        static_cast<jint>(result.worldY * 1000),
        static_cast<jint>(result.worldZ * 1000)
    };
    env->SetIntArrayRegion(arr, 0, 5, values);
    return arr;
}

} // extern "C"

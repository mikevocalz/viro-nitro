#include <jni.h>
#include "KimoyoOjuInit.hpp"
#include "KimoyoOjuNitroOnLoad.hpp"

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    // Register HybridObjects with Nitro before initialization
    margelo::nitro::kimoyooju::registerKimoyoOjuHybridObjects();
    
    // Call the generated initialize function
    return margelo::nitro::kimoyooju::initialize(vm);
}

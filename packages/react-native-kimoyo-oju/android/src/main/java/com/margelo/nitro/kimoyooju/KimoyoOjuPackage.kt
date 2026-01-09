package com.margelo.nitro.kimoyooju

import com.facebook.react.ReactPackage
import com.facebook.react.bridge.NativeModule
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.uimanager.ViewManager
import com.kimoyooju.nitro.KimoyoOjuViewManager

class KimoyoOjuPackage : ReactPackage {
    
    companion object {
        init {
            // Initialize Nitro native library
            KimoyoOjuNitroOnLoad.initializeNative()
        }
    }

    override fun createNativeModules(reactContext: ReactApplicationContext): List<NativeModule> {
        return emptyList()
    }

    override fun createViewManagers(reactContext: ReactApplicationContext): List<ViewManager<*, *>> {
        return listOf(KimoyoOjuViewManager())
    }
}

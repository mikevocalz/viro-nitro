package com.kimoyooju.nitro

import com.facebook.react.uimanager.SimpleViewManager
import com.facebook.react.uimanager.ThemedReactContext
import com.facebook.react.uimanager.annotations.ReactProp

class KimoyoOjuViewManager : SimpleViewManager<KimoyoOjuARView>() {
    
    override fun getName(): String = REACT_CLASS
    
    override fun createViewInstance(reactContext: ThemedReactContext): KimoyoOjuARView {
        return KimoyoOjuARView(reactContext)
    }
    
    @ReactProp(name = "mode")
    fun setMode(view: KimoyoOjuARView, mode: String?) {
        mode?.let { view.setXRMode(it) }
    }

    @ReactProp(name = "passthrough")
    fun setPassthrough(view: KimoyoOjuARView, passthrough: String?) {
        view.setPassthroughMode(passthrough ?: "native")
    }
    
    override fun onDropViewInstance(view: KimoyoOjuARView) {
        view.getGLSurface()?.pause()
        super.onDropViewInstance(view)
    }
    
    companion object {
        const val REACT_CLASS = "KimoyoOjuView"
    }
}

package com.kimoyooju.nitro

import android.content.Context
import android.graphics.PixelFormat
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.FrameLayout

/**
 * AR View that combines camera passthrough with GL rendering.
 * Camera preview is shown behind a transparent GL surface.
 */
class KimoyoOjuARView(context: Context) : FrameLayout(context) {

    private var cameraPreview: CameraPreviewView? = null
    private var glSurface: KimoyoOjuSurfaceView? = null
    private var arEnabled = false
    private val handler = Handler(Looper.getMainLooper())

    init {
        // Create GL surface (will be on top)
        glSurface = KimoyoOjuSurfaceView(context).apply {
            setZOrderOnTop(true)
            holder.setFormat(PixelFormat.TRANSLUCENT)
        }
        addView(glSurface, LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT))
        
        Log.i("KimoyoOjuARView", "ARView initialized")
    }

    fun enableAR() {
        if (arEnabled) return
        arEnabled = true
        Log.i("KimoyoOjuARView", "Enabling AR mode - starting camera")

        // Create and add camera preview behind GL surface
        handler.post {
            if (cameraPreview == null) {
                cameraPreview = CameraPreviewView(context)
                addView(cameraPreview, 0, LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT))
                Log.i("KimoyoOjuARView", "Camera preview view added")
            }
            cameraPreview?.startPreview()
            
            // Make GL surface transparent
            glSurface?.setZOrderOnTop(true)
            glSurface?.holder?.setFormat(PixelFormat.TRANSLUCENT)
        }
    }

    fun disableAR() {
        if (!arEnabled) return
        arEnabled = false

        cameraPreview?.stopPreview()
        cameraPreview?.let { removeView(it) }
        cameraPreview = null

        // Make GL surface opaque again
        glSurface?.setZOrderOnTop(false)
        glSurface?.holder?.setFormat(PixelFormat.OPAQUE)
    }

    fun getGLSurface(): KimoyoOjuSurfaceView? = glSurface

    fun setXRMode(mode: String) {
        Log.i("KimoyoOjuARView", "setXRMode called with: $mode")
        when (mode) {
            "immersive-mr" -> enableAR()
            else -> disableAR()
        }
        glSurface?.setXRMode(mode)
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        disableAR()
    }
}

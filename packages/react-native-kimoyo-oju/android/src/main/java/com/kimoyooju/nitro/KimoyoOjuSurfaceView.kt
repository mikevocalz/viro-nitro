package com.kimoyooju.nitro

import android.content.Context
import android.graphics.PixelFormat
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.view.MotionEvent
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

/**
 * Custom GLSurfaceView for Kimoyo Oju rendering.
 * Handles OpenGL ES context and surface lifecycle.
 */
class KimoyoOjuSurfaceView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs), GLSurfaceView.Renderer {

    private var nativeHandle: Long = 0
    private var surfaceWidth: Int = 0
    private var surfaceHeight: Int = 0
    
    var onSurfaceCreatedListener: ((Int, Int) -> Unit)? = null
    var onSurfaceChangedListener: ((Int, Int) -> Unit)? = null
    var onSurfaceDestroyedListener: (() -> Unit)? = null
    var onTouchListener: ((Float, Float, Int) -> Boolean)? = null

    init {
        // Configure OpenGL ES 3.0 with alpha for AR transparency
        setEGLContextClientVersion(3)
        setEGLConfigChooser(8, 8, 8, 8, 24, 0)
        holder.setFormat(PixelFormat.TRANSLUCENT)
        setZOrderOnTop(true)

        isFocusable = true
        isFocusableInTouchMode = true
        isClickable = true
        requestFocus()
        
        setRenderer(this)
        renderMode = RENDERMODE_CONTINUOUSLY
    }
    
    fun setTransparent(transparent: Boolean) {
        if (transparent) {
            setZOrderOnTop(true)
            holder.setFormat(PixelFormat.TRANSLUCENT)
        } else {
            setZOrderOnTop(false)
            holder.setFormat(PixelFormat.OPAQUE)
        }
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        nativeHandle = nativeInit()
        onSurfaceCreatedListener?.invoke(surfaceWidth, surfaceHeight)
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        surfaceWidth = width
        surfaceHeight = height
        
        if (nativeHandle != 0L) {
            nativeSurfaceChanged(nativeHandle, width, height)
        }
        
        onSurfaceChangedListener?.invoke(width, height)
    }

    override fun onDrawFrame(gl: GL10?) {
        if (nativeHandle != 0L) {
            nativeDrawFrame(nativeHandle)
        }
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        
        queueEvent {
            if (nativeHandle != 0L) {
                nativeDestroy(nativeHandle)
                nativeHandle = 0
            }
        }
        
        onSurfaceDestroyedListener?.invoke()
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val action = when (event.action) {
            MotionEvent.ACTION_DOWN -> 0
            MotionEvent.ACTION_UP -> 1
            MotionEvent.ACTION_MOVE -> 2
            else -> -1
        }
        if (nativeHandle != 0L) {
            if (action >= 0) {
                val result = nativeHandleTouch(nativeHandle, event.x, event.y, action)
                if (result[0] == 1) { // hit
                    onTouchListener?.invoke(event.x, event.y, action)
                    return true
                }
            }
        }
        val handled = onTouchListener?.invoke(event.x, event.y, action) ?: false
        return handled || super.onTouchEvent(event)
    }

    fun setXRMode(mode: String) {
        queueEvent {
            if (nativeHandle != 0L) {
                nativeSetXRMode(nativeHandle, mode)
            }
        }
    }

    fun submitCommandBuffer(bufferJson: String) {
        queueEvent {
            if (nativeHandle != 0L) {
                nativeSubmitCommands(nativeHandle, bufferJson)
            }
        }
    }

    fun pause() {
        queueEvent {
            if (nativeHandle != 0L) {
                nativePause(nativeHandle)
            }
        }
        onPause()
    }

    fun resume() {
        onResume()
        queueEvent {
            if (nativeHandle != 0L) {
                nativeResume(nativeHandle)
            }
        }
    }

    // Native methods
    private external fun nativeInit(): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativeSurfaceChanged(handle: Long, width: Int, height: Int)
    private external fun nativeDrawFrame(handle: Long)
    private external fun nativeSetXRMode(handle: Long, mode: String)
    private external fun nativeSubmitCommands(handle: Long, bufferJson: String)
    private external fun nativePause(handle: Long)
    private external fun nativeResume(handle: Long)
    private external fun nativeHandleTouch(handle: Long, x: Float, y: Float, action: Int): IntArray

    companion object {
        init {
            System.loadLibrary("KimoyoOjuNitro")
        }
    }
}

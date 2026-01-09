import { HybridObject } from 'react-native-nitro-modules'
import type { Handle, VoidResult, XRMode } from './KimoyoOjuEngine.nitro'

/**
 * View lifecycle events
 */
export type ViewEvent = 
  | 'attached'
  | 'detached'
  | 'surface_created'
  | 'surface_destroyed'
  | 'surface_changed'

/**
 * Surface information
 */
export interface SurfaceInfo {
  width: number
  height: number
  pixelRatio: number
}

/**
 * KimoyoOju View Hybrid Object
 * 
 * Represents the native view that hosts the renderer.
 * Handles surface lifecycle and bridges to the Engine.
 */
export interface KimoyoOjuView extends HybridObject<{ ios: 'c++'; android: 'c++' }> {
  /**
   * Get the current surface info
   */
  getSurfaceInfo(): SurfaceInfo | null

  /**
   * Check if surface is valid and ready for rendering
   */
  isSurfaceReady(): boolean

  /**
   * Get current XR mode
   */
  getMode(): XRMode

  /**
   * Set XR mode (triggers mode transition)
   */
  setMode(mode: XRMode): VoidResult

  /**
   * Register view event callback
   */
  setViewEventCallback(callback: (event: ViewEvent, info?: SurfaceInfo) => void): void

  /**
   * Force a redraw (usually not needed)
   */
  requestRedraw(): void
}

/**
 * KimoyoOjuView Factory - creates native views
 */
export interface KimoyoOjuViewFactory extends HybridObject<{ ios: 'c++'; android: 'c++' }> {
  /**
   * Create a new KimoyoOjuView instance
   */
  createView(config: KimoyoOjuViewConfig): KimoyoOjuView
}

/**
 * Configuration for creating a KimoyoOjuView
 */
export interface KimoyoOjuViewConfig {
  mode: XRMode
  enableMSAA?: boolean
  msaaSamples?: number
}

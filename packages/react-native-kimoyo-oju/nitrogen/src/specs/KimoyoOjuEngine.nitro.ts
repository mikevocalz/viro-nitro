import { HybridObject } from 'react-native-nitro-modules'

/**
 * Opaque handle to a native resource with generation counter for safety
 */
export interface Handle {
  id: number
  gen: number
}

/**
 * Void result for operations that don't return a value
 */
export interface VoidResult {
  ok: boolean
  code: string
  message: string
}

/**
 * Result with handle value
 */
export interface HandleResult {
  ok: boolean
  code: string
  message: string
  handle: Handle | null
}

/**
 * XR rendering mode
 */
export type XRMode = 'flat' | 'immersive-vr' | 'immersive-mr'

/**
 * Renderer state
 */
export type RendererState = 'created' | 'running' | 'paused' | 'surface_lost' | 'destroyed'

/**
 * Node types in the scene graph
 */
export type NodeType = 'group' | 'mesh' | 'light' | 'camera' | 'text' | 'model'

/**
 * Light types
 */
export type LightType = 'ambient' | 'directional' | 'point' | 'spot'

/**
 * Engine events
 */
export type EngineEvent = 
  | 'surface_created'
  | 'surface_destroyed'
  | 'xr_session_started'
  | 'xr_session_ended'
  | 'xr_session_lost'
  | 'error'

/**
 * Memory counter snapshot for leak detection
 */
export interface MemoryCounters {
  liveNodes: number
  liveTextures: number
  liveBuffers: number
  liveSwapchains: number
  liveShaders: number
  gpuMemoryBytes: number
  cpuMemoryBytes: number
}

/**
 * Engine configuration
 */
export interface EngineConfig {
  enableValidation?: boolean
  enableMemoryTracking?: boolean
  maxNodes?: number
  maxTextures?: number
}

/**
 * Main KimoyoOju Engine Hybrid Object
 * 
 * Owns the render thread and all GPU/XR resources.
 * All methods are safe to call from JS thread - they either:
 * 1. Enqueue commands to render thread
 * 2. Return cached state
 * 3. Return Result objects (never throw)
 */
export interface KimoyoOjuEngine extends HybridObject<{ ios: 'c++'; android: 'c++' }> {
  /**
   * Submit a command buffer as JSON string to the render thread
   * Commands are validated before submission
   */
  submitJson(bufferJson: string): VoidResult

  /**
   * Allocate a handle for a new node (does not create the node yet)
   * Call submit() with a CreateNode command to actually create it
   */
  allocateHandle(): HandleResult

  /**
   * Get current renderer state
   */
  getState(): RendererState

  /**
   * Get current XR mode
   */
  getMode(): XRMode

  /**
   * Get memory counters for leak detection
   */
  getMemoryCounters(): MemoryCounters

  /**
   * Lifecycle: pause rendering (call on app background)
   */
  pause(): VoidResult

  /**
   * Lifecycle: resume rendering (call on app foreground)
   */
  resume(): VoidResult

  /**
   * Enter XR mode (VR or MR)
   */
  enterXR(mode: XRMode): VoidResult

  /**
   * Exit XR mode and return to flat rendering
   */
  exitXR(): VoidResult

  /**
   * Explicitly destroy a native resource
   * Safe to call multiple times (idempotent)
   */
  destroyHandle(handle: Handle): VoidResult
}

import { NitroModules, type HybridObject } from 'react-native-nitro-modules'
import type {
  Handle,
  HandleResult,
  VoidResult,
  XRMode,
  RendererState,
  MemoryCounters,
  CommandBuffer,
} from './types'

/**
 * Native KimoyoOjuEngine interface (matches C++ HybridObject)
 */
interface NativeKimoyoOjuEngine extends HybridObject<{ ios: 'c++'; android: 'c++' }> {
  submitJson(bufferJson: string): VoidResult
  allocateHandle(): HandleResult
  getState(): RendererState
  getMode(): XRMode
  getMemoryCounters(): MemoryCounters
  pause(): VoidResult
  resume(): VoidResult
  enterXR(mode: XRMode): VoidResult
  exitXR(): VoidResult
  destroyHandle(handle: Handle): VoidResult
}

let engineInstance: NativeKimoyoOjuEngine | null = null

/**
 * Get or create the KimoyoOjuEngine singleton
 */
export function getKimoyoOjuEngine(): NativeKimoyoOjuEngine {
  if (!engineInstance) {
    engineInstance = NitroModules.createHybridObject<NativeKimoyoOjuEngine>('KimoyoOjuEngine')
  }
  return engineInstance
}

/**
 * KimoyoOjuEngine API wrapper with additional safety and convenience
 */
export const KimoyoOjuEngine = {
  /**
   * Submit a command buffer to the render thread
   */
  submit(buffer: CommandBuffer): VoidResult {
    return getKimoyoOjuEngine().submitJson(JSON.stringify(buffer))
  },

  /**
   * Allocate a new handle for a node
   */
  allocateHandle(): HandleResult {
    return getKimoyoOjuEngine().allocateHandle()
  },

  /**
   * Get current renderer state
   */
  getState(): RendererState {
    return getKimoyoOjuEngine().getState()
  },

  /**
   * Get current XR mode
   */
  getMode(): XRMode {
    return getKimoyoOjuEngine().getMode()
  },

  /**
   * Get memory counters for leak detection
   */
  getMemoryCounters(): MemoryCounters {
    return getKimoyoOjuEngine().getMemoryCounters()
  },

  /**
   * Pause rendering (call on app background)
   */
  pause(): VoidResult {
    return getKimoyoOjuEngine().pause()
  },

  /**
   * Resume rendering (call on app foreground)
   */
  resume(): VoidResult {
    return getKimoyoOjuEngine().resume()
  },

  /**
   * Enter XR mode
   */
  enterXR(mode: XRMode): VoidResult {
    if (mode === 'flat') {
      return { ok: false, code: 'INVALID_MODE', message: 'Use exitXR to return to flat mode' }
    }
    return getKimoyoOjuEngine().enterXR(mode)
  },

  /**
   * Exit XR mode
   */
  exitXR(): VoidResult {
    return getKimoyoOjuEngine().exitXR()
  },

  /**
   * Destroy a native resource handle
   */
  destroyHandle(handle: Handle): VoidResult {
    return getKimoyoOjuEngine().destroyHandle(handle)
  },
}

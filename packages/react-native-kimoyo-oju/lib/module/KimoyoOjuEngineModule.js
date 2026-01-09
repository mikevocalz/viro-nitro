"use strict";

import { NitroModules } from 'react-native-nitro-modules';

/**
 * Native KimoyoOjuEngine interface (matches C++ HybridObject)
 */

let engineInstance = null;

/**
 * Get or create the KimoyoOjuEngine singleton
 */
export function getKimoyoOjuEngine() {
  if (!engineInstance) {
    engineInstance = NitroModules.createHybridObject('KimoyoOjuEngine');
  }
  return engineInstance;
}

/**
 * KimoyoOjuEngine API wrapper with additional safety and convenience
 */
export const KimoyoOjuEngine = {
  /**
   * Submit a command buffer to the render thread
   */
  submit(buffer) {
    return getKimoyoOjuEngine().submitJson(JSON.stringify(buffer));
  },
  /**
   * Allocate a new handle for a node
   */
  allocateHandle() {
    return getKimoyoOjuEngine().allocateHandle();
  },
  /**
   * Get current renderer state
   */
  getState() {
    return getKimoyoOjuEngine().getState();
  },
  /**
   * Get current XR mode
   */
  getMode() {
    return getKimoyoOjuEngine().getMode();
  },
  /**
   * Get memory counters for leak detection
   */
  getMemoryCounters() {
    return getKimoyoOjuEngine().getMemoryCounters();
  },
  /**
   * Pause rendering (call on app background)
   */
  pause() {
    return getKimoyoOjuEngine().pause();
  },
  /**
   * Resume rendering (call on app foreground)
   */
  resume() {
    return getKimoyoOjuEngine().resume();
  },
  /**
   * Enter XR mode
   */
  enterXR(mode) {
    if (mode === 'flat') {
      return {
        ok: false,
        code: 'INVALID_MODE',
        message: 'Use exitXR to return to flat mode'
      };
    }
    return getKimoyoOjuEngine().enterXR(mode);
  },
  /**
   * Exit XR mode
   */
  exitXR() {
    return getKimoyoOjuEngine().exitXR();
  },
  /**
   * Destroy a native resource handle
   */
  destroyHandle(handle) {
    return getKimoyoOjuEngine().destroyHandle(handle);
  }
};
//# sourceMappingURL=KimoyoOjuEngineModule.js.map
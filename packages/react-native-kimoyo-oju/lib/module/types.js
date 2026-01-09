"use strict";

/**
 * Opaque handle to a native resource with generation counter for safety
 */

/**
 * Void result for operations that don't return a value
 */

/**
 * Result with handle value
 */

/**
 * XR rendering mode
 */

/**
 * Renderer state
 */

/**
 * Node types in the scene graph
 */

/**
 * Light types
 */

/**
 * Engine events
 */

/**
 * Memory counter snapshot for leak detection
 */

/**
 * 3D Vector
 */

/**
 * Quaternion (x, y, z, w)
 */

/**
 * RGBA Color
 */

/**
 * Command buffer containing batched scene updates
 */

/**
 * All possible commands
 */

/**
 * Helper to create a successful void result
 */
export function successVoid() {
  return {
    ok: true,
    code: '',
    message: ''
  };
}

/**
 * Helper to create a failure void result
 */
export function failureVoid(code, message) {
  return {
    ok: false,
    code,
    message
  };
}

/**
 * Helper to check if handle is valid
 */
export function isValidHandle(handle) {
  return handle != null && handle.id > 0 && handle.gen > 0;
}

/**
 * Create an invalid/null handle
 */
export function nullHandle() {
  return {
    id: 0,
    gen: 0
  };
}
//# sourceMappingURL=types.js.map
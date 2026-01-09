"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRDimensions = exports.XRColors = void 0;
exports.normalizePadding = normalizePadding;
exports.normalizeScale = normalizeScale;
/**
 * Base props shared by all XR components
 */

/**
 * Interactive component props
 */

/**
 * Layout direction for stacks
 */

/**
 * Alignment options
 */

/**
 * Justify content options
 */

/**
 * Panel background style
 */

/**
 * Text style
 */

/**
 * Button variant
 */

/**
 * Button size
 */

/**
 * Slider orientation
 */

/**
 * Scroll direction
 */

/**
 * Interaction type
 */

/**
 * HUD anchor position
 */

/**
 * Video playback state
 */

/**
 * Cylinder mapping mode
 */

/**
 * Helper to normalize scale
 */
function normalizeScale(scale) {
  if (scale === undefined) return [1, 1, 1];
  if (typeof scale === 'number') return [scale, scale, scale];
  return scale;
}

/**
 * Helper to normalize padding
 */
function normalizePadding(padding) {
  if (padding === undefined) return [0, 0, 0, 0];
  if (typeof padding === 'number') return [padding, padding, padding, padding];
  return padding;
}

/**
 * Default colors
 */
const XRColors = exports.XRColors = {
  primary: [0.2, 0.5, 1.0, 1.0],
  secondary: [0.4, 0.4, 0.4, 1.0],
  background: [0.1, 0.1, 0.1, 0.9],
  surface: [0.15, 0.15, 0.15, 0.95],
  text: [1.0, 1.0, 1.0, 1.0],
  textSecondary: [0.7, 0.7, 0.7, 1.0],
  border: [0.3, 0.3, 0.3, 1.0],
  hover: [0.3, 0.6, 1.0, 1.0],
  active: [0.1, 0.4, 0.9, 1.0],
  danger: [0.9, 0.2, 0.2, 1.0],
  success: [0.2, 0.8, 0.3, 1.0],
  warning: [0.9, 0.7, 0.1, 1.0]
};

/**
 * Default dimensions (in meters)
 */
const XRDimensions = exports.XRDimensions = {
  buttonHeight: {
    sm: 0.04,
    md: 0.05,
    lg: 0.06
  },
  fontSize: {
    xs: 0.012,
    sm: 0.016,
    md: 0.02,
    lg: 0.028,
    xl: 0.04
  },
  spacing: {
    xs: 0.005,
    sm: 0.01,
    md: 0.02,
    lg: 0.04,
    xl: 0.08
  },
  cornerRadius: {
    none: 0,
    sm: 0.005,
    md: 0.01,
    lg: 0.02,
    full: 999
  }
};
//# sourceMappingURL=types.js.map
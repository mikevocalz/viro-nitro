"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRHUD = XRHUD;
exports.XRNotification = XRNotification;
exports.XRTooltip = XRTooltip;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * XRHUD Props
 */

/**
 * Calculate HUD position based on anchor
 */
function getAnchorOffset(anchor, distance) {
  const horizontalFOV = 0.8; // Approximate horizontal FOV factor
  const verticalFOV = 0.5; // Approximate vertical FOV factor

  const positions = {
    'top-left': [-horizontalFOV * distance, verticalFOV * distance, -distance],
    'top-center': [0, verticalFOV * distance, -distance],
    'top-right': [horizontalFOV * distance, verticalFOV * distance, -distance],
    'center-left': [-horizontalFOV * distance, 0, -distance],
    'center': [0, 0, -distance],
    'center-right': [horizontalFOV * distance, 0, -distance],
    'bottom-left': [-horizontalFOV * distance, -verticalFOV * distance, -distance],
    'bottom-center': [0, -verticalFOV * distance, -distance],
    'bottom-right': [horizontalFOV * distance, -verticalFOV * distance, -distance]
  };
  return positions[anchor];
}

/**
 * XRHUD - Head-locked UI overlay
 * 
 * Creates UI that follows the user's head position.
 * Perfect for status indicators, notifications, or always-visible controls.
 * 
 * @example
 * ```tsx
 * <XRHUD anchor="bottom-center" distance={1}>
 *   <XRPanel>
 *     <XRText>Health: 100</XRText>
 *   </XRPanel>
 * </XRHUD>
 * ```
 */
function XRHUD({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  anchor = 'center',
  distance = 1.5,
  offsetX = 0,
  offsetY = 0,
  followHead = true,
  followSpeed = 5,
  fadeAtEdge = true,
  alwaysVisible = true
}) {
  const [hudPosition, setHudPosition] = (0, _react.useState)([0, 0, -distance]);
  const anchorOffset = getAnchorOffset(anchor, distance);
  const finalPosition = position ?? [anchorOffset[0] + offsetX, anchorOffset[1] + offsetY, anchorOffset[2]];
  const {
    handle,
    isReady
  } = (0, _useXRNode.useXRNode)('group', {
    position: finalPosition,
    rotation,
    scale,
    visible,
    onMount,
    onUnmount
  });
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(_jsxRuntime.Fragment, {
    children: children
  });
}
XRHUD.displayName = 'XRHUD';

/**
 * XRTooltip - Contextual tooltip that appears near interaction point
 */

function XRTooltip({
  position,
  rotation,
  scale,
  visible = true,
  children,
  onMount,
  onUnmount,
  text,
  show = false,
  delay = 500,
  placement = 'top',
  maxWidth = 0.2
}) {
  const [isVisible, setIsVisible] = (0, _react.useState)(false);
  const {
    handle,
    isReady
  } = (0, _useXRNode.useXRNode)('group', {
    position,
    rotation,
    scale,
    visible: visible && isVisible,
    onMount,
    onUnmount
  });
  (0, _react.useEffect)(() => {
    if (show) {
      const timer = setTimeout(() => setIsVisible(true), delay);
      return () => clearTimeout(timer);
    } else {
      setIsVisible(false);
    }
  }, [show, delay]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(_jsxRuntime.Fragment, {
    children: children
  });
}
XRTooltip.displayName = 'XRTooltip';

/**
 * XRNotification - Toast-like notification
 */

function XRNotification({
  position,
  rotation,
  scale,
  visible = true,
  onMount,
  onUnmount,
  message,
  type = 'info',
  duration = 3000,
  onDismiss
}) {
  const [isVisible, setIsVisible] = (0, _react.useState)(true);
  const {
    handle,
    isReady
  } = (0, _useXRNode.useXRNode)('group', {
    position,
    rotation,
    scale,
    visible: visible && isVisible,
    onMount,
    onUnmount
  });
  (0, _react.useEffect)(() => {
    if (duration > 0) {
      const timer = setTimeout(() => {
        setIsVisible(false);
        onDismiss?.();
      }, duration);
      return () => clearTimeout(timer);
    }
  }, [duration, onDismiss]);
  if (!isReady || !handle || !isVisible) {
    return null;
  }
  return null;
}
XRNotification.displayName = 'XRNotification';
//# sourceMappingURL=XRHUD.js.map
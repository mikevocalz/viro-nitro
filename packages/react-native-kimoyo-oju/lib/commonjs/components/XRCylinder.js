"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRCurvedPanel = XRCurvedPanel;
exports.XRCylinder = XRCylinder;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _types = require("./types");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * XRCylinder Props
 */

/**
 * XRCylinder - Cylindrical geometry component
 * 
 * Perfect for curved displays, panoramic content, or curved UI panels.
 * 
 * @example
 * ```tsx
 * // Curved display panel
 * <XRCylinder
 *   radius={2}
 *   height={1}
 *   thetaLength={Math.PI / 2}
 *   mapping="inside"
 *   texture="panorama.jpg"
 * />
 * 
 * // Simple cylinder
 * <XRCylinder
 *   radius={0.1}
 *   height={0.5}
 *   color={[0.8, 0.2, 0.2, 1]}
 * />
 * ```
 */
function XRCylinder({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  radius = 0.5,
  height = 1.0,
  radialSegments = 32,
  heightSegments = 1,
  openEnded = false,
  thetaStart = 0,
  thetaLength = Math.PI * 2,
  color = _types.XRColors.surface,
  texture,
  mapping = 'outside',
  lightingEnabled = true
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const submitCommands = (0, _useXRNode.useSubmitCommands)();
  const {
    handle,
    isReady
  } = (0, _useXRNode.useXRNode)('mesh', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount
  });

  // Set geometry
  (0, _react.useEffect)(() => {
    if (!handle || !isReady) return;
    submitCommands([{
      type: 'SET_GEOMETRY',
      handle,
      geometryType: 'cylinder',
      dimensions: [radius, height, radialSegments]
    }]);
  }, [handle, isReady, radius, height, radialSegments]);

  // Set material
  (0, _react.useEffect)(() => {
    if (!handle || !isReady) return;
    submitCommands([{
      type: 'SET_MATERIAL',
      handle,
      diffuseColor: color,
      specularColor: [1, 1, 1, 1],
      shininess: 0.5,
      diffuseTexture: null,
      normalTexture: null
    }]);
  }, [handle, isReady, color]);
  if (!isReady || !handle) {
    return null;
  }
  return null;
}
XRCylinder.displayName = 'XRCylinder';

/**
 * XRCurvedPanel - Convenience component for curved UI panels
 */

function XRCurvedPanel({
  width = 1,
  height = 0.5,
  curveRadius = 2,
  curveAngle = Math.PI / 3,
  children,
  ...props
}) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRCylinder, {
    ...props,
    radius: curveRadius,
    height: height,
    thetaStart: -curveAngle / 2,
    thetaLength: curveAngle,
    mapping: "inside",
    openEnded: true,
    children: children
  });
}
XRCurvedPanel.displayName = 'XRCurvedPanel';
//# sourceMappingURL=XRCylinder.js.map
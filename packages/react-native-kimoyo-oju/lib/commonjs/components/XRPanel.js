"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRPanel = XRPanel;
exports.XRPanelContext = void 0;
exports.useParentPanel = useParentPanel;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _types = require("./types");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * Context to pass panel handle to children
 */
const XRPanelContext = exports.XRPanelContext = /*#__PURE__*/(0, _react.createContext)(null);

/**
 * Hook to get parent panel handle
 */
function useParentPanel() {
  return (0, _react.useContext)(XRPanelContext);
}

/**
 * XRPanel Props
 */

/**
 * XRPanel - A 3D panel container for XR UI elements
 * 
 * Used as a base container for spatial UI. Can be flat or curved,
 * and optionally follows the user's gaze.
 * 
 * @example
 * ```tsx
 * <XRPanel 
 *   width={0.5} 
 *   height={0.3} 
 *   position={[0, 1.5, -1]}
 *   style={{ backgroundColor: [0.1, 0.1, 0.1, 0.9], cornerRadius: 0.02 }}
 * >
 *   <XRText>Hello World</XRText>
 * </XRPanel>
 * ```
 */
function XRPanel({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  width = 0.4,
  height = 0.3,
  depth = 0.01,
  style = {},
  curved = false,
  curveRadius = 1,
  followGaze = false,
  billboardMode = 'none'
}) {
  const parentHandle = useParentPanel();
  const {
    handle,
    isReady
  } = (0, _useXRNode.useXRNode)('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount
  });
  const mergedStyle = (0, _react.useMemo)(() => ({
    backgroundColor: _types.XRColors.surface,
    backgroundOpacity: 0.95,
    cornerRadius: 0.01,
    padding: 0.02,
    ...style
  }), [style]);
  const padding = (0, _types.normalizePadding)(mergedStyle.padding);
  const contextValue = (0, _react.useMemo)(() => handle, [handle]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRPanelContext.Provider, {
    value: contextValue,
    children: children
  });
}
XRPanel.displayName = 'XRPanel';
//# sourceMappingURL=XRPanel.js.map
"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRHeading = XRHeading;
exports.XRLabel = XRLabel;
exports.XRText = XRText;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _types = require("./types");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * XRText Props
 */

/**
 * XRText - 3D text rendering component
 * 
 * Renders text in 3D space with customizable styling.
 * 
 * @example
 * ```tsx
 * <XRText 
 *   text="Hello World"
 *   style={{ fontSize: 0.05, color: [1, 1, 1, 1] }}
 *   position={[0, 1.5, -1]}
 * />
 * ```
 */
function XRText({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  onMount,
  onUnmount,
  children,
  text,
  style = {}
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const submitCommands = (0, _useXRNode.useSubmitCommands)();
  const {
    handle,
    isReady
  } = (0, _useXRNode.useXRNode)('text', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount
  });
  const displayText = text ?? (typeof children === 'string' ? children : '');
  const mergedStyle = {
    fontSize: _types.XRDimensions.fontSize.md,
    fontFamily: 'system',
    fontWeight: 'normal',
    color: _types.XRColors.text,
    textAlign: 'center',
    ...style
  };

  // Update text content when it changes
  (0, _react.useEffect)(() => {
    if (!handle || !isReady) return;
    submitCommands([{
      type: 'SET_TEXT',
      handle,
      text: displayText,
      fontSize: mergedStyle.fontSize,
      color: mergedStyle.color,
      fontFamily: mergedStyle.fontFamily
    }]);
  }, [handle, isReady, displayText, mergedStyle.fontSize, mergedStyle.color, mergedStyle.fontFamily]);
  if (!isReady || !handle) {
    return null;
  }
  return null;
}
XRText.displayName = 'XRText';

/**
 * XRHeading - Large heading text
 */
function XRHeading(props) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRText, {
    ...props,
    style: {
      fontSize: _types.XRDimensions.fontSize.xl,
      fontWeight: 'bold',
      ...props.style
    }
  });
}
XRHeading.displayName = 'XRHeading';

/**
 * XRLabel - Small label text
 */
function XRLabel(props) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRText, {
    ...props,
    style: {
      fontSize: _types.XRDimensions.fontSize.sm,
      color: _types.XRColors.textSecondary,
      ...props.style
    }
  });
}
XRLabel.displayName = 'XRLabel';
//# sourceMappingURL=XRText.js.map
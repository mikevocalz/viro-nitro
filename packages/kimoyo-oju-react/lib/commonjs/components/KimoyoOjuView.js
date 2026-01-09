"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuView = KimoyoOjuView;
var _react = _interopRequireWildcard(require("react"));
var _reactNative = require("react-native");
var _reactNativeKimoyoOju = require("react-native-kimoyo-oju");
var _KimoyoOjuContext = require("../context/KimoyoOjuContext");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * KimoyoOjuView - Main container for KimoyoOju 3D/XR content.
 * 
 * This component:
 * 1. Creates the native OpenGL view that hosts the renderer
 * 2. Provides KimoyoOjuContext to all children
 * 3. Handles lifecycle events
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuView mode="flat" style={{ flex: 1 }}>
 *   <KimoyoOjuScene>
 *     <KimoyoOjuBox position={[0, 0, -5]} />
 *   </KimoyoOjuScene>
 * </KimoyoOjuView>
 * ```
 */
function KimoyoOjuView({
  style,
  mode = 'flat',
  children,
  onError
}) {
  const handleError = (0, _react.useCallback)((code, message) => {
    onError?.(code, message);
  }, [onError]);
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(_KimoyoOjuContext.KimoyoOjuProvider, {
    onError: handleError,
    children: /*#__PURE__*/(0, _jsxRuntime.jsxs)(_reactNative.View, {
      style: [styles.container, style],
      children: [/*#__PURE__*/(0, _jsxRuntime.jsx)(_reactNativeKimoyoOju.KimoyoOjuNativeView, {
        style: styles.glView,
        mode: mode
      }), /*#__PURE__*/(0, _jsxRuntime.jsx)(_reactNative.View, {
        style: styles.overlay,
        children: children
      })]
    })
  });
}
const styles = _reactNative.StyleSheet.create({
  container: {
    flex: 1
  },
  glView: {
    ..._reactNative.StyleSheet.absoluteFillObject
  },
  overlay: {
    ..._reactNative.StyleSheet.absoluteFillObject
  }
});
//# sourceMappingURL=KimoyoOjuView.js.map
"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuNativeView = KimoyoOjuNativeView;
var _react = _interopRequireDefault(require("react"));
var _reactNative = require("react-native");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireDefault(e) { return e && e.__esModule ? e : { default: e }; }
const NativeView = (0, _reactNative.requireNativeComponent)('KimoyoOjuView');
function KimoyoOjuNativeView({
  style,
  mode = 'flat'
}) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(NativeView, {
    style: style,
    mode: mode
  });
}
//# sourceMappingURL=KimoyoOjuNativeView.js.map
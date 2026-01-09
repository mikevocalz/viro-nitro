"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuScene = KimoyoOjuScene;
exports.useKimoyoOjuSceneContext = useKimoyoOjuSceneContext;
var _react = _interopRequireWildcard(require("react"));
var _useKimoyoOjuNode = require("../hooks/useKimoyoOjuNode");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
const KimoyoOjuSceneContext = /*#__PURE__*/(0, _react.createContext)(null);
function useKimoyoOjuSceneContext() {
  const context = (0, _react.useContext)(KimoyoOjuSceneContext);
  if (!context) {
    throw new Error('KimoyoOju components must be used within a KimoyoOjuScene');
  }
  return context;
}
/**
 * KimoyoOjuScene - Root container for 3D scene content.
 * 
 * All KimoyoOjuNode, KimoyoOjuBox, etc. components must be descendants of KimoyoOjuScene.
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuView>
 *   <KimoyoOjuScene>
 *     <KimoyoOjuBox position={[0, 0, -5]} />
 *     <KimoyoOjuLight type="ambient" />
 *   </KimoyoOjuScene>
 * </KimoyoOjuView>
 * ```
 */
function KimoyoOjuScene({
  children
}) {
  const {
    handle
  } = (0, _useKimoyoOjuNode.useKimoyoOjuNode)({
    nodeType: 'group'
  });
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(KimoyoOjuSceneContext.Provider, {
    value: {
      sceneHandle: handle
    },
    children: children
  });
}
//# sourceMappingURL=KimoyoOjuScene.js.map
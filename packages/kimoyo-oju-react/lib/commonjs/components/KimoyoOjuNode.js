"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuNode = KimoyoOjuNode;
exports.useKimoyoOjuParentHandle = useKimoyoOjuParentHandle;
var _react = _interopRequireWildcard(require("react"));
var _useKimoyoOjuNode = require("../hooks/useKimoyoOjuNode");
var _KimoyoOjuScene = require("./KimoyoOjuScene");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
const KimoyoOjuNodeContext = /*#__PURE__*/(0, _react.createContext)(null);
function useKimoyoOjuParentHandle() {
  const nodeContext = (0, _react.useContext)(KimoyoOjuNodeContext);
  const sceneContext = (0, _KimoyoOjuScene.useKimoyoOjuSceneContext)();
  return nodeContext?.parentHandle ?? sceneContext.sceneHandle;
}
/**
 * KimoyoOjuNode - Generic container node in the scene graph.
 * 
 * Used for grouping and transforming child nodes.
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuNode position={[0, 1, 0]}>
 *   <KimoyoOjuBox />
 *   <KimoyoOjuSphere position={[1, 0, 0]} />
 * </KimoyoOjuNode>
 * ```
 */
function KimoyoOjuNode({
  position,
  rotation,
  scale,
  visible = true,
  children
}) {
  const parentHandle = useKimoyoOjuParentHandle();
  const {
    handle
  } = (0, _useKimoyoOjuNode.useKimoyoOjuNode)({
    nodeType: 'group',
    parentHandle,
    position,
    rotation,
    scale,
    visible
  });
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(KimoyoOjuNodeContext.Provider, {
    value: {
      parentHandle: handle
    },
    children: children
  });
}
//# sourceMappingURL=KimoyoOjuNode.js.map
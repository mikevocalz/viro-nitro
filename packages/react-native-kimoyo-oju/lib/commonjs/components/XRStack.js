"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRHStack = XRHStack;
exports.XRStack = XRStack;
exports.XRVStack = XRVStack;
exports.XRZStack = XRZStack;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _types = require("./types");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * XRStack Props
 */

/**
 * Calculate child positions based on layout
 */
function calculateChildPositions(childCount, direction, spacing, alignment, justifyContent) {
  const positions = [];
  for (let i = 0; i < childCount; i++) {
    let x = 0;
    let y = 0;
    let z = 0;
    const offset = i * spacing;
    switch (direction) {
      case 'horizontal':
        x = offset;
        break;
      case 'vertical':
        y = -offset; // Negative because Y grows upward in 3D
        break;
      case 'depth':
        z = -offset; // Negative to go away from viewer
        break;
    }
    positions.push([x, y, z]);
  }

  // Center the stack based on total size
  if (childCount > 0) {
    const totalSize = (childCount - 1) * spacing;
    const centerOffset = totalSize / 2;
    for (let i = 0; i < positions.length; i++) {
      switch (direction) {
        case 'horizontal':
          positions[i][0] -= centerOffset;
          break;
        case 'vertical':
          positions[i][1] += centerOffset;
          break;
        case 'depth':
          positions[i][2] += centerOffset;
          break;
      }
    }
  }
  return positions;
}

/**
 * XRStack - Layout children in a horizontal, vertical, or depth stack
 * 
 * Similar to CSS flexbox but for 3D space.
 * 
 * @example
 * ```tsx
 * <XRStack direction="horizontal" spacing={0.05}>
 *   <XRButton>Button 1</XRButton>
 *   <XRButton>Button 2</XRButton>
 *   <XRButton>Button 3</XRButton>
 * </XRStack>
 * ```
 */
function XRStack({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  direction = 'horizontal',
  spacing = _types.XRDimensions.spacing.md,
  alignment = 'center',
  justifyContent = 'center',
  wrap = false,
  maxWidth,
  maxHeight
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
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
  const childArray = _react.Children.toArray(children);
  const childPositions = (0, _react.useMemo)(() => calculateChildPositions(childArray.length, direction, spacing, alignment, justifyContent), [childArray.length, direction, spacing, alignment, justifyContent]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(_jsxRuntime.Fragment, {
    children: childArray.map((child, index) => {
      if (! /*#__PURE__*/(0, _react.isValidElement)(child)) return child;
      const childPosition = childPositions[index] || [0, 0, 0];
      return /*#__PURE__*/(0, _react.cloneElement)(child, {
        key: index,
        position: childPosition
      });
    })
  });
}
XRStack.displayName = 'XRStack';

/**
 * Convenience component for horizontal stack
 */
function XRHStack(props) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRStack, {
    ...props,
    direction: "horizontal"
  });
}
XRHStack.displayName = 'XRHStack';

/**
 * Convenience component for vertical stack
 */
function XRVStack(props) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRStack, {
    ...props,
    direction: "vertical"
  });
}
XRVStack.displayName = 'XRVStack';

/**
 * Convenience component for depth stack
 */
function XRZStack(props) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRStack, {
    ...props,
    direction: "depth"
  });
}
XRZStack.displayName = 'XRZStack';
//# sourceMappingURL=XRStack.js.map
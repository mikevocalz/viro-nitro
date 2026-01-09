"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRGrid = XRGrid;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _types = require("./types");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * XRGrid Props
 */

/**
 * Calculate grid cell positions
 */
function calculateGridPositions(childCount, columns, cellWidth, cellHeight, spacingX, spacingY) {
  const positions = [];
  const rows = Math.ceil(childCount / columns);
  const totalWidth = columns * cellWidth + (columns - 1) * spacingX;
  const totalHeight = rows * cellHeight + (rows - 1) * spacingY;
  for (let i = 0; i < childCount; i++) {
    const col = i % columns;
    const row = Math.floor(i / columns);
    const x = col * (cellWidth + spacingX) - totalWidth / 2 + cellWidth / 2;
    const y = -(row * (cellHeight + spacingY) - totalHeight / 2 + cellHeight / 2);
    positions.push([x, y, 0]);
  }
  return positions;
}

/**
 * XRGrid - Layout children in a 2D grid
 * 
 * Perfect for icon grids, image galleries, or any grid-based UI.
 * 
 * @example
 * ```tsx
 * <XRGrid columns={3} cellWidth={0.1} cellHeight={0.1} spacingX={0.02} spacingY={0.02}>
 *   <XRButton>1</XRButton>
 *   <XRButton>2</XRButton>
 *   <XRButton>3</XRButton>
 *   <XRButton>4</XRButton>
 *   <XRButton>5</XRButton>
 *   <XRButton>6</XRButton>
 * </XRGrid>
 * ```
 */
function XRGrid({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  columns = 3,
  rows,
  cellWidth = 0.1,
  cellHeight = 0.1,
  spacingX = _types.XRDimensions.spacing.sm,
  spacingY = _types.XRDimensions.spacing.sm,
  alignItems = 'center',
  justifyItems = 'center'
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
  const gridPositions = (0, _react.useMemo)(() => calculateGridPositions(childArray.length, columns, cellWidth, cellHeight, spacingX, spacingY), [childArray.length, columns, cellWidth, cellHeight, spacingX, spacingY]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(_jsxRuntime.Fragment, {
    children: childArray.map((child, index) => {
      if (! /*#__PURE__*/(0, _react.isValidElement)(child)) return child;
      const cellPosition = gridPositions[index] || [0, 0, 0];
      return /*#__PURE__*/(0, _react.cloneElement)(child, {
        key: index,
        position: cellPosition
      });
    })
  });
}
XRGrid.displayName = 'XRGrid';
//# sourceMappingURL=XRGrid.js.map
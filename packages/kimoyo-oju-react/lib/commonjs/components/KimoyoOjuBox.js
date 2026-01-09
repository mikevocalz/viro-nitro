"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuBox = KimoyoOjuBox;
var _react = require("react");
var _useKimoyoOjuNode = require("../hooks/useKimoyoOjuNode");
var _KimoyoOjuNode = require("./KimoyoOjuNode");
var _KimoyoOjuContext = require("../context/KimoyoOjuContext");
/**
 * KimoyoOjuBox - 3D box/cube primitive.
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuBox 
 *   position={[0, 0, -5]} 
 *   width={1} 
 *   height={1} 
 *   length={1}
 *   color={[1, 0, 0, 1]} 
 * />
 * ```
 */
function KimoyoOjuBox({
  position,
  rotation,
  scale,
  visible = true,
  width = 1,
  height = 1,
  length = 1,
  color = [1, 1, 1, 1]
}) {
  const parentHandle = (0, _KimoyoOjuNode.useKimoyoOjuParentHandle)();
  const {
    getCommandBuffer,
    flushCommands
  } = (0, _KimoyoOjuContext.useKimoyoOjuContext)();
  const {
    handle
  } = (0, _useKimoyoOjuNode.useKimoyoOjuNode)({
    nodeType: 'mesh',
    parentHandle,
    position,
    rotation,
    scale,
    visible
  });

  // Set geometry and material when props change
  (0, _react.useEffect)(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setGeometry(handle, 'box', [width, height, length]);
      buffer.setMaterial(handle, color, [1, 1, 1, 1], 32, null, null);
      flushCommands();
    }
  }, [handle, width, height, length, color[0], color[1], color[2], color[3]]);
  return null;
}
//# sourceMappingURL=KimoyoOjuBox.js.map
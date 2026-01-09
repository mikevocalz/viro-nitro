"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuSphere = KimoyoOjuSphere;
var _react = require("react");
var _useKimoyoOjuNode = require("../hooks/useKimoyoOjuNode");
var _KimoyoOjuNode = require("./KimoyoOjuNode");
var _KimoyoOjuContext = require("../context/KimoyoOjuContext");
/**
 * KimoyoOjuSphere - 3D sphere primitive.
 */
function KimoyoOjuSphere({
  position,
  rotation,
  scale,
  visible = true,
  radius = 0.5,
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
  (0, _react.useEffect)(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setGeometry(handle, 'sphere', [radius * 2, radius * 2, radius * 2]);
      buffer.setMaterial(handle, color, [1, 1, 1, 1], 32, null, null);
      flushCommands();
    }
  }, [handle, radius, color[0], color[1], color[2], color[3]]);
  return null;
}
//# sourceMappingURL=KimoyoOjuSphere.js.map
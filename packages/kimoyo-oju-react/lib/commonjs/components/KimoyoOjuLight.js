"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuLight = KimoyoOjuLight;
var _react = require("react");
var _useKimoyoOjuNode = require("../hooks/useKimoyoOjuNode");
var _KimoyoOjuNode = require("./KimoyoOjuNode");
var _KimoyoOjuContext = require("../context/KimoyoOjuContext");
/**
 * KimoyoOjuLight - Light source in the scene.
 */
function KimoyoOjuLight({
  type,
  position,
  rotation,
  color = [1, 1, 1, 1],
  intensity = 1,
  range = 10,
  innerConeAngle = 0,
  outerConeAngle = 45
}) {
  const parentHandle = (0, _KimoyoOjuNode.useKimoyoOjuParentHandle)();
  const {
    getCommandBuffer,
    flushCommands
  } = (0, _KimoyoOjuContext.useKimoyoOjuContext)();
  const {
    handle
  } = (0, _useKimoyoOjuNode.useKimoyoOjuNode)({
    nodeType: 'light',
    parentHandle,
    position,
    rotation
  });
  (0, _react.useEffect)(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setLight(handle, type, color, intensity, range, innerConeAngle, outerConeAngle);
      flushCommands();
    }
  }, [handle, type, color[0], color[1], color[2], color[3], intensity, range, innerConeAngle, outerConeAngle]);
  return null;
}
//# sourceMappingURL=KimoyoOjuLight.js.map
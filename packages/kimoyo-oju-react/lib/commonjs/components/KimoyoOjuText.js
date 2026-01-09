"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuText = KimoyoOjuText;
var _react = require("react");
var _useKimoyoOjuNode = require("../hooks/useKimoyoOjuNode");
var _KimoyoOjuNode = require("./KimoyoOjuNode");
var _KimoyoOjuContext = require("../context/KimoyoOjuContext");
/**
 * KimoyoOjuText - 3D text rendered in the scene.
 */
function KimoyoOjuText({
  text,
  position,
  rotation,
  scale,
  visible = true,
  fontSize = 16,
  color = [1, 1, 1, 1],
  fontFamily = 'system'
}) {
  const parentHandle = (0, _KimoyoOjuNode.useKimoyoOjuParentHandle)();
  const {
    getCommandBuffer,
    flushCommands
  } = (0, _KimoyoOjuContext.useKimoyoOjuContext)();
  const {
    handle
  } = (0, _useKimoyoOjuNode.useKimoyoOjuNode)({
    nodeType: 'text',
    parentHandle,
    position,
    rotation,
    scale,
    visible
  });
  (0, _react.useEffect)(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setText(handle, text, fontSize, color, fontFamily);
      flushCommands();
    }
  }, [handle, text, fontSize, color[0], color[1], color[2], color[3], fontFamily]);
  return null;
}
//# sourceMappingURL=KimoyoOjuText.js.map
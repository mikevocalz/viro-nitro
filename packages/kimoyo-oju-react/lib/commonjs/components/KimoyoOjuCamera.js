"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuCamera = KimoyoOjuCamera;
var _react = require("react");
var _useKimoyoOjuNode = require("../hooks/useKimoyoOjuNode");
var _KimoyoOjuNode = require("./KimoyoOjuNode");
var _KimoyoOjuContext = require("../context/KimoyoOjuContext");
/**
 * KimoyoOjuCamera - Camera in the scene (for flat mode).
 * In XR mode, the camera is controlled by the headset.
 */
function KimoyoOjuCamera({
  position,
  rotation,
  fov = 60,
  nearClip = 0.1,
  farClip = 1000
}) {
  const parentHandle = (0, _KimoyoOjuNode.useKimoyoOjuParentHandle)();
  const {
    getCommandBuffer,
    flushCommands
  } = (0, _KimoyoOjuContext.useKimoyoOjuContext)();
  const {
    handle
  } = (0, _useKimoyoOjuNode.useKimoyoOjuNode)({
    nodeType: 'camera',
    parentHandle,
    position,
    rotation
  });
  (0, _react.useEffect)(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setCamera(handle, fov, nearClip, farClip);
      flushCommands();
    }
  }, [handle, fov, nearClip, farClip]);
  return null;
}
//# sourceMappingURL=KimoyoOjuCamera.js.map
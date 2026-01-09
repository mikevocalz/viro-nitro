"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.useKimoyoOjuEngine = useKimoyoOjuEngine;
var _react = require("react");
var _reactNativeKimoyoOju = require("react-native-kimoyo-oju");
/**
 * Hook to access KimoyoOjuEngine state and controls
 */
function useKimoyoOjuEngine() {
  const getState = (0, _react.useCallback)(() => {
    return _reactNativeKimoyoOju.KimoyoOjuEngine.getState();
  }, []);
  const getMode = (0, _react.useCallback)(() => {
    return _reactNativeKimoyoOju.KimoyoOjuEngine.getMode();
  }, []);
  const getMemoryCounters = (0, _react.useCallback)(() => {
    return _reactNativeKimoyoOju.KimoyoOjuEngine.getMemoryCounters();
  }, []);
  const pause = (0, _react.useCallback)(() => {
    const result = _reactNativeKimoyoOju.KimoyoOjuEngine.pause();
    return result.ok;
  }, []);
  const resume = (0, _react.useCallback)(() => {
    const result = _reactNativeKimoyoOju.KimoyoOjuEngine.resume();
    return result.ok;
  }, []);
  const enterXR = (0, _react.useCallback)(mode => {
    const result = _reactNativeKimoyoOju.KimoyoOjuEngine.enterXR(mode);
    return result.ok;
  }, []);
  const exitXR = (0, _react.useCallback)(() => {
    const result = _reactNativeKimoyoOju.KimoyoOjuEngine.exitXR();
    return result.ok;
  }, []);
  return {
    getState,
    getMode,
    getMemoryCounters,
    pause,
    resume,
    enterXR,
    exitXR
  };
}
//# sourceMappingURL=useKimoyoOjuEngine.js.map
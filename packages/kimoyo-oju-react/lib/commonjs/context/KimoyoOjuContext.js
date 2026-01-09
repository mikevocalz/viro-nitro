"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.KimoyoOjuProvider = KimoyoOjuProvider;
exports.useKimoyoOjuContext = useKimoyoOjuContext;
var _react = _interopRequireWildcard(require("react"));
var _reactNativeKimoyoOju = require("react-native-kimoyo-oju");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
const KimoyoOjuContext = /*#__PURE__*/(0, _react.createContext)(null);
/**
 * KimoyoOjuProvider - provides access to the KimoyoOju engine throughout the component tree.
 * 
 * Manages command buffer batching and automatic flushing on React commit.
 */
function KimoyoOjuProvider({
  children,
  onError
}) {
  const commandBufferRef = (0, _react.useRef)(new _reactNativeKimoyoOju.CommandBufferBuilder());
  const flushScheduledRef = (0, _react.useRef)(false);
  const allocateHandle = (0, _react.useCallback)(() => {
    const result = _reactNativeKimoyoOju.KimoyoOjuEngine.allocateHandle();
    if (result.ok && result.handle) {
      return result.handle;
    }
    if (!result.ok) {
      onError?.(result.code, result.message);
    }
    return null;
  }, [onError]);
  const getCommandBuffer = (0, _react.useCallback)(() => {
    return commandBufferRef.current;
  }, []);
  const scheduleFlush = (0, _react.useCallback)(() => {
    if (!flushScheduledRef.current) {
      flushScheduledRef.current = true;
      // Use microtask to batch all updates in the same React commit
      queueMicrotask(() => {
        flushScheduledRef.current = false;
        const buffer = commandBufferRef.current.flush();
        if (buffer.commands.length > 0) {
          const result = _reactNativeKimoyoOju.KimoyoOjuEngine.submit(buffer);
          if (!result.ok) {
            onError?.(result.code, result.message);
          }
        }
      });
    }
  }, [onError]);
  const flushCommands = (0, _react.useCallback)(() => {
    scheduleFlush();
  }, [scheduleFlush]);
  const getState = (0, _react.useCallback)(() => {
    return _reactNativeKimoyoOju.KimoyoOjuEngine.getState();
  }, []);
  const getMode = (0, _react.useCallback)(() => {
    return _reactNativeKimoyoOju.KimoyoOjuEngine.getMode();
  }, []);
  const enterXR = (0, _react.useCallback)(mode => {
    const result = _reactNativeKimoyoOju.KimoyoOjuEngine.enterXR(mode);
    if (!result.ok) {
      onError?.(result.code, result.message);
      return false;
    }
    return true;
  }, [onError]);
  const exitXR = (0, _react.useCallback)(() => {
    const result = _reactNativeKimoyoOju.KimoyoOjuEngine.exitXR();
    if (!result.ok) {
      onError?.(result.code, result.message);
      return false;
    }
    return true;
  }, [onError]);
  const dispose = (0, _react.useCallback)(handle => {
    _reactNativeKimoyoOju.KimoyoOjuEngine.destroyHandle(handle);
  }, []);
  const value = (0, _react.useMemo)(() => ({
    allocateHandle,
    getCommandBuffer,
    flushCommands,
    getState,
    getMode,
    enterXR,
    exitXR,
    dispose
  }), [allocateHandle, getCommandBuffer, flushCommands, getState, getMode, enterXR, exitXR, dispose]);
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(KimoyoOjuContext.Provider, {
    value: value,
    children: children
  });
}

/**
 * Hook to access the KimoyoOju context
 */
function useKimoyoOjuContext() {
  const context = (0, _react.useContext)(KimoyoOjuContext);
  if (!context) {
    throw new Error('useKimoyoOjuContext must be used within a KimoyoOjuProvider');
  }
  return context;
}
//# sourceMappingURL=KimoyoOjuContext.js.map
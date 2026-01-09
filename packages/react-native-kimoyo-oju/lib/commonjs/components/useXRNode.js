"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.useSubmitCommands = useSubmitCommands;
exports.useXRNode = useXRNode;
var _react = require("react");
var _KimoyoOjuEngineModule = require("../KimoyoOjuEngineModule");
var _types = require("./types");
let txIdCounter = 0;
function getNextTxId() {
  return ++txIdCounter;
}

/**
 * Hook for managing an XR node's lifecycle and transforms
 */
function useXRNode(nodeType, options = {}) {
  const handleRef = (0, _react.useRef)(null);
  const mountedRef = (0, _react.useRef)(false);
  const {
    position = [0, 0, 0],
    rotation = [0, 0, 0, 1],
    scale = [1, 1, 1],
    visible = true,
    parentHandle = null,
    onMount,
    onUnmount
  } = options;
  const normalizedScale = (0, _types.normalizeScale)(scale);

  // Allocate handle and create node on mount
  (0, _react.useEffect)(() => {
    const result = _KimoyoOjuEngineModule.KimoyoOjuEngine.allocateHandle();
    if (!result.ok) {
      console.error('Failed to allocate handle:', result.message);
      return;
    }
    const handle = result.handle;
    if (!handle) {
      console.error('Failed to get handle from result');
      return;
    }
    handleRef.current = handle;
    mountedRef.current = true;
    const buffer = {
      version: 1,
      txId: getNextTxId(),
      timestamp: Date.now(),
      commands: [{
        type: 'CREATE_NODE',
        handle,
        nodeType,
        parentHandle
      }, {
        type: 'SET_TRANSFORM',
        handle,
        position,
        rotation,
        scale: normalizedScale
      }, {
        type: 'SET_VISIBILITY',
        handle,
        visible
      }]
    };
    _KimoyoOjuEngineModule.KimoyoOjuEngine.submit(buffer);
    onMount?.(handle);
    return () => {
      if (handleRef.current && mountedRef.current) {
        const destroyBuffer = {
          version: 1,
          txId: getNextTxId(),
          timestamp: Date.now(),
          commands: [{
            type: 'DESTROY_NODE',
            handle: handleRef.current
          }]
        };
        _KimoyoOjuEngineModule.KimoyoOjuEngine.submit(destroyBuffer);
        _KimoyoOjuEngineModule.KimoyoOjuEngine.destroyHandle(handleRef.current);
        mountedRef.current = false;
        onUnmount?.();
      }
    };
  }, []); // Only run on mount/unmount

  // Update transform when props change
  (0, _react.useEffect)(() => {
    if (!handleRef.current || !mountedRef.current) return;
    const buffer = {
      version: 1,
      txId: getNextTxId(),
      timestamp: Date.now(),
      commands: [{
        type: 'SET_TRANSFORM',
        handle: handleRef.current,
        position,
        rotation,
        scale: normalizedScale
      }]
    };
    _KimoyoOjuEngineModule.KimoyoOjuEngine.submit(buffer);
  }, [position[0], position[1], position[2], rotation[0], rotation[1], rotation[2], rotation[3], normalizedScale[0], normalizedScale[1], normalizedScale[2]]);

  // Update visibility when prop changes
  (0, _react.useEffect)(() => {
    if (!handleRef.current || !mountedRef.current) return;
    const buffer = {
      version: 1,
      txId: getNextTxId(),
      timestamp: Date.now(),
      commands: [{
        type: 'SET_VISIBILITY',
        handle: handleRef.current,
        visible
      }]
    };
    _KimoyoOjuEngineModule.KimoyoOjuEngine.submit(buffer);
  }, [visible]);
  const getHandle = (0, _react.useCallback)(() => handleRef.current, []);
  return {
    handle: handleRef.current,
    getHandle,
    isReady: mountedRef.current && handleRef.current !== null
  };
}

/**
 * Hook for submitting commands
 */
function useSubmitCommands() {
  return (0, _react.useCallback)(commands => {
    const buffer = {
      version: 1,
      txId: getNextTxId(),
      timestamp: Date.now(),
      commands
    };
    return _KimoyoOjuEngineModule.KimoyoOjuEngine.submit(buffer);
  }, []);
}
//# sourceMappingURL=useXRNode.js.map
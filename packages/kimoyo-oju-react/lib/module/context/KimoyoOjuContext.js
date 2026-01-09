"use strict";

import React, { createContext, useContext, useRef, useCallback, useMemo } from 'react';
import { KimoyoOjuEngine, CommandBufferBuilder } from 'react-native-kimoyo-oju';
import { jsx as _jsx } from "react/jsx-runtime";
const KimoyoOjuContext = /*#__PURE__*/createContext(null);
/**
 * KimoyoOjuProvider - provides access to the KimoyoOju engine throughout the component tree.
 * 
 * Manages command buffer batching and automatic flushing on React commit.
 */
export function KimoyoOjuProvider({
  children,
  onError
}) {
  const commandBufferRef = useRef(new CommandBufferBuilder());
  const flushScheduledRef = useRef(false);
  const allocateHandle = useCallback(() => {
    const result = KimoyoOjuEngine.allocateHandle();
    if (result.ok && result.handle) {
      return result.handle;
    }
    if (!result.ok) {
      onError?.(result.code, result.message);
    }
    return null;
  }, [onError]);
  const getCommandBuffer = useCallback(() => {
    return commandBufferRef.current;
  }, []);
  const scheduleFlush = useCallback(() => {
    if (!flushScheduledRef.current) {
      flushScheduledRef.current = true;
      // Use microtask to batch all updates in the same React commit
      queueMicrotask(() => {
        flushScheduledRef.current = false;
        const buffer = commandBufferRef.current.flush();
        if (buffer.commands.length > 0) {
          const result = KimoyoOjuEngine.submit(buffer);
          if (!result.ok) {
            onError?.(result.code, result.message);
          }
        }
      });
    }
  }, [onError]);
  const flushCommands = useCallback(() => {
    scheduleFlush();
  }, [scheduleFlush]);
  const getState = useCallback(() => {
    return KimoyoOjuEngine.getState();
  }, []);
  const getMode = useCallback(() => {
    return KimoyoOjuEngine.getMode();
  }, []);
  const enterXR = useCallback(mode => {
    const result = KimoyoOjuEngine.enterXR(mode);
    if (!result.ok) {
      onError?.(result.code, result.message);
      return false;
    }
    return true;
  }, [onError]);
  const exitXR = useCallback(() => {
    const result = KimoyoOjuEngine.exitXR();
    if (!result.ok) {
      onError?.(result.code, result.message);
      return false;
    }
    return true;
  }, [onError]);
  const dispose = useCallback(handle => {
    KimoyoOjuEngine.destroyHandle(handle);
  }, []);
  const value = useMemo(() => ({
    allocateHandle,
    getCommandBuffer,
    flushCommands,
    getState,
    getMode,
    enterXR,
    exitXR,
    dispose
  }), [allocateHandle, getCommandBuffer, flushCommands, getState, getMode, enterXR, exitXR, dispose]);
  return /*#__PURE__*/_jsx(KimoyoOjuContext.Provider, {
    value: value,
    children: children
  });
}

/**
 * Hook to access the KimoyoOju context
 */
export function useKimoyoOjuContext() {
  const context = useContext(KimoyoOjuContext);
  if (!context) {
    throw new Error('useKimoyoOjuContext must be used within a KimoyoOjuProvider');
  }
  return context;
}
//# sourceMappingURL=KimoyoOjuContext.js.map
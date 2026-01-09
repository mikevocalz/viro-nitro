"use strict";

import { useEffect, useRef, useCallback, useState } from 'react';
import { useKimoyoOjuContext } from '../context/KimoyoOjuContext';
/**
 * Hook to manage a KimoyoOju scene node's lifecycle.
 * 
 * Automatically creates the node on mount, updates on prop changes,
 * and destroys on unmount.
 */
export function useKimoyoOjuNode(options) {
  const {
    nodeType,
    parentHandle,
    position,
    rotation,
    scale,
    visible = true
  } = options;
  const {
    allocateHandle,
    getCommandBuffer,
    flushCommands,
    dispose
  } = useKimoyoOjuContext();
  const [handle, setHandle] = useState(null);
  const handleRef = useRef(null);
  const mountedRef = useRef(false);

  // Allocate handle and create node on mount
  useEffect(() => {
    if (!mountedRef.current) {
      mountedRef.current = true;
      const newHandle = allocateHandle();
      if (newHandle) {
        handleRef.current = newHandle;
        setHandle(newHandle);
        const buffer = getCommandBuffer();
        buffer.createNode(newHandle, nodeType, parentHandle ?? null);
        if (position || rotation || scale) {
          buffer.setTransform(newHandle, position ?? [0, 0, 0], rotation ?? [0, 0, 0, 1], scale ?? [1, 1, 1]);
        }
        if (!visible) {
          buffer.setVisibility(newHandle, false);
        }
        flushCommands();
      }
    }
    return () => {
      if (handleRef.current) {
        dispose(handleRef.current);
        handleRef.current = null;
        setHandle(null);
      }
    };
  }, []); // Only run on mount/unmount

  // Update transform when props change
  useEffect(() => {
    if (handleRef.current && mountedRef.current) {
      const buffer = getCommandBuffer();
      buffer.setTransform(handleRef.current, position ?? [0, 0, 0], rotation ?? [0, 0, 0, 1], scale ?? [1, 1, 1]);
      flushCommands();
    }
  }, [position?.[0], position?.[1], position?.[2], rotation?.[0], rotation?.[1], rotation?.[2], rotation?.[3], scale?.[0], scale?.[1], scale?.[2]]);

  // Update visibility when prop changes
  useEffect(() => {
    if (handleRef.current && mountedRef.current) {
      const buffer = getCommandBuffer();
      buffer.setVisibility(handleRef.current, visible);
      flushCommands();
    }
  }, [visible]);
  const setTransform = useCallback((pos, rot, scl) => {
    if (handleRef.current) {
      const buffer = getCommandBuffer();
      buffer.setTransform(handleRef.current, pos ?? [0, 0, 0], rot ?? [0, 0, 0, 1], scl ?? [1, 1, 1]);
      flushCommands();
    }
  }, [getCommandBuffer, flushCommands]);
  const setVisibility = useCallback(vis => {
    if (handleRef.current) {
      const buffer = getCommandBuffer();
      buffer.setVisibility(handleRef.current, vis);
      flushCommands();
    }
  }, [getCommandBuffer, flushCommands]);
  const destroy = useCallback(() => {
    if (handleRef.current) {
      dispose(handleRef.current);
      handleRef.current = null;
    }
  }, [dispose]);
  return {
    handle,
    setTransform,
    setVisibility,
    destroy
  };
}
//# sourceMappingURL=useKimoyoOjuNode.js.map
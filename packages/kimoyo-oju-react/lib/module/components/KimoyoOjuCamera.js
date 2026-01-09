"use strict";

import { useEffect } from 'react';
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode';
import { useKimoyoOjuParentHandle } from './KimoyoOjuNode';
import { useKimoyoOjuContext } from '../context/KimoyoOjuContext';
/**
 * KimoyoOjuCamera - Camera in the scene (for flat mode).
 * In XR mode, the camera is controlled by the headset.
 */
export function KimoyoOjuCamera({
  position,
  rotation,
  fov = 60,
  nearClip = 0.1,
  farClip = 1000
}) {
  const parentHandle = useKimoyoOjuParentHandle();
  const {
    getCommandBuffer,
    flushCommands
  } = useKimoyoOjuContext();
  const {
    handle
  } = useKimoyoOjuNode({
    nodeType: 'camera',
    parentHandle,
    position,
    rotation
  });
  useEffect(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setCamera(handle, fov, nearClip, farClip);
      flushCommands();
    }
  }, [handle, fov, nearClip, farClip]);
  return null;
}
//# sourceMappingURL=KimoyoOjuCamera.js.map
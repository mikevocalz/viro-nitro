"use strict";

import { useEffect } from 'react';
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode';
import { useKimoyoOjuParentHandle } from './KimoyoOjuNode';
import { useKimoyoOjuContext } from '../context/KimoyoOjuContext';
/**
 * KimoyoOjuLight - Light source in the scene.
 */
export function KimoyoOjuLight({
  type,
  position,
  rotation,
  color = [1, 1, 1, 1],
  intensity = 1,
  range = 10,
  innerConeAngle = 0,
  outerConeAngle = 45
}) {
  const parentHandle = useKimoyoOjuParentHandle();
  const {
    getCommandBuffer,
    flushCommands
  } = useKimoyoOjuContext();
  const {
    handle
  } = useKimoyoOjuNode({
    nodeType: 'light',
    parentHandle,
    position,
    rotation
  });
  useEffect(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setLight(handle, type, color, intensity, range, innerConeAngle, outerConeAngle);
      flushCommands();
    }
  }, [handle, type, color[0], color[1], color[2], color[3], intensity, range, innerConeAngle, outerConeAngle]);
  return null;
}
//# sourceMappingURL=KimoyoOjuLight.js.map
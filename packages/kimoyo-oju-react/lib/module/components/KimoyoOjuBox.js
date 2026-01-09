"use strict";

import { useEffect } from 'react';
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode';
import { useKimoyoOjuParentHandle } from './KimoyoOjuNode';
import { useKimoyoOjuContext } from '../context/KimoyoOjuContext';
/**
 * KimoyoOjuBox - 3D box/cube primitive.
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuBox 
 *   position={[0, 0, -5]} 
 *   width={1} 
 *   height={1} 
 *   length={1}
 *   color={[1, 0, 0, 1]} 
 * />
 * ```
 */
export function KimoyoOjuBox({
  position,
  rotation,
  scale,
  visible = true,
  width = 1,
  height = 1,
  length = 1,
  color = [1, 1, 1, 1]
}) {
  const parentHandle = useKimoyoOjuParentHandle();
  const {
    getCommandBuffer,
    flushCommands
  } = useKimoyoOjuContext();
  const {
    handle
  } = useKimoyoOjuNode({
    nodeType: 'mesh',
    parentHandle,
    position,
    rotation,
    scale,
    visible
  });

  // Set geometry and material when props change
  useEffect(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setGeometry(handle, 'box', [width, height, length]);
      buffer.setMaterial(handle, color, [1, 1, 1, 1], 32, null, null);
      flushCommands();
    }
  }, [handle, width, height, length, color[0], color[1], color[2], color[3]]);
  return null;
}
//# sourceMappingURL=KimoyoOjuBox.js.map
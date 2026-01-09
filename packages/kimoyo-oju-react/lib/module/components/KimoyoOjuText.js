"use strict";

import { useEffect } from 'react';
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode';
import { useKimoyoOjuParentHandle } from './KimoyoOjuNode';
import { useKimoyoOjuContext } from '../context/KimoyoOjuContext';
/**
 * KimoyoOjuText - 3D text rendered in the scene.
 */
export function KimoyoOjuText({
  text,
  position,
  rotation,
  scale,
  visible = true,
  fontSize = 16,
  color = [1, 1, 1, 1],
  fontFamily = 'system'
}) {
  const parentHandle = useKimoyoOjuParentHandle();
  const {
    getCommandBuffer,
    flushCommands
  } = useKimoyoOjuContext();
  const {
    handle
  } = useKimoyoOjuNode({
    nodeType: 'text',
    parentHandle,
    position,
    rotation,
    scale,
    visible
  });
  useEffect(() => {
    if (handle) {
      const buffer = getCommandBuffer();
      buffer.setText(handle, text, fontSize, color, fontFamily);
      flushCommands();
    }
  }, [handle, text, fontSize, color[0], color[1], color[2], color[3], fontFamily]);
  return null;
}
//# sourceMappingURL=KimoyoOjuText.js.map
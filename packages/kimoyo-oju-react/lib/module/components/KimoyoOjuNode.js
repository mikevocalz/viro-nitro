"use strict";

import React, { createContext, useContext } from 'react';
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode';
import { useKimoyoOjuSceneContext } from './KimoyoOjuScene';
import { jsx as _jsx } from "react/jsx-runtime";
const KimoyoOjuNodeContext = /*#__PURE__*/createContext(null);
export function useKimoyoOjuParentHandle() {
  const nodeContext = useContext(KimoyoOjuNodeContext);
  const sceneContext = useKimoyoOjuSceneContext();
  return nodeContext?.parentHandle ?? sceneContext.sceneHandle;
}
/**
 * KimoyoOjuNode - Generic container node in the scene graph.
 * 
 * Used for grouping and transforming child nodes.
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuNode position={[0, 1, 0]}>
 *   <KimoyoOjuBox />
 *   <KimoyoOjuSphere position={[1, 0, 0]} />
 * </KimoyoOjuNode>
 * ```
 */
export function KimoyoOjuNode({
  position,
  rotation,
  scale,
  visible = true,
  children
}) {
  const parentHandle = useKimoyoOjuParentHandle();
  const {
    handle
  } = useKimoyoOjuNode({
    nodeType: 'group',
    parentHandle,
    position,
    rotation,
    scale,
    visible
  });
  return /*#__PURE__*/_jsx(KimoyoOjuNodeContext.Provider, {
    value: {
      parentHandle: handle
    },
    children: children
  });
}
//# sourceMappingURL=KimoyoOjuNode.js.map
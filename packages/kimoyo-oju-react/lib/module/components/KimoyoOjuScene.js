"use strict";

import React, { createContext, useContext } from 'react';
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode';
import { jsx as _jsx } from "react/jsx-runtime";
const KimoyoOjuSceneContext = /*#__PURE__*/createContext(null);
export function useKimoyoOjuSceneContext() {
  const context = useContext(KimoyoOjuSceneContext);
  if (!context) {
    throw new Error('KimoyoOju components must be used within a KimoyoOjuScene');
  }
  return context;
}
/**
 * KimoyoOjuScene - Root container for 3D scene content.
 * 
 * All KimoyoOjuNode, KimoyoOjuBox, etc. components must be descendants of KimoyoOjuScene.
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuView>
 *   <KimoyoOjuScene>
 *     <KimoyoOjuBox position={[0, 0, -5]} />
 *     <KimoyoOjuLight type="ambient" />
 *   </KimoyoOjuScene>
 * </KimoyoOjuView>
 * ```
 */
export function KimoyoOjuScene({
  children
}) {
  const {
    handle
  } = useKimoyoOjuNode({
    nodeType: 'group'
  });
  return /*#__PURE__*/_jsx(KimoyoOjuSceneContext.Provider, {
    value: {
      sceneHandle: handle
    },
    children: children
  });
}
//# sourceMappingURL=KimoyoOjuScene.js.map
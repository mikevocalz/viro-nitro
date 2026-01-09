"use strict";

import React, { createContext, useContext, useMemo } from 'react';
import { useXRNode } from './useXRNode';
import { XRColors, normalizePadding } from './types';

/**
 * Context to pass panel handle to children
 */
import { jsx as _jsx } from "react/jsx-runtime";
export const XRPanelContext = /*#__PURE__*/createContext(null);

/**
 * Hook to get parent panel handle
 */
export function useParentPanel() {
  return useContext(XRPanelContext);
}

/**
 * XRPanel Props
 */

/**
 * XRPanel - A 3D panel container for XR UI elements
 * 
 * Used as a base container for spatial UI. Can be flat or curved,
 * and optionally follows the user's gaze.
 * 
 * @example
 * ```tsx
 * <XRPanel 
 *   width={0.5} 
 *   height={0.3} 
 *   position={[0, 1.5, -1]}
 *   style={{ backgroundColor: [0.1, 0.1, 0.1, 0.9], cornerRadius: 0.02 }}
 * >
 *   <XRText>Hello World</XRText>
 * </XRPanel>
 * ```
 */
export function XRPanel({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  width = 0.4,
  height = 0.3,
  depth = 0.01,
  style = {},
  curved = false,
  curveRadius = 1,
  followGaze = false,
  billboardMode = 'none'
}) {
  const parentHandle = useParentPanel();
  const {
    handle,
    isReady
  } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount
  });
  const mergedStyle = useMemo(() => ({
    backgroundColor: XRColors.surface,
    backgroundOpacity: 0.95,
    cornerRadius: 0.01,
    padding: 0.02,
    ...style
  }), [style]);
  const padding = normalizePadding(mergedStyle.padding);
  const contextValue = useMemo(() => handle, [handle]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/_jsx(XRPanelContext.Provider, {
    value: contextValue,
    children: children
  });
}
XRPanel.displayName = 'XRPanel';
//# sourceMappingURL=XRPanel.js.map
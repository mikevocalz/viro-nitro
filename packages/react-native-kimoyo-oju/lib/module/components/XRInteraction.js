"use strict";

import React, { createContext, useContext, useCallback, useState } from 'react';
import { useXRNode } from './useXRNode';
import { useParentPanel } from './XRPanel';

/**
 * Interaction context for nested components
 */
import { jsx as _jsx } from "react/jsx-runtime";
const InteractionContext = /*#__PURE__*/createContext({
  isHovered: false,
  isSelected: false,
  interactionType: null,
  hitPoint: null,
  hitNormal: null
});
export function useInteraction() {
  return useContext(InteractionContext);
}

/**
 * XRNearInteraction Props
 */

/**
 * XRNearInteraction - Near-field hand interaction component
 * 
 * Enables direct touch/poke interactions with hands.
 * Wrap interactive elements to enable near interaction.
 * 
 * @example
 * ```tsx
 * <XRNearInteraction onNearSelect={(hand) => console.log(`${hand} hand selected`)}>
 *   <XRButton>Touch Me</XRButton>
 * </XRNearInteraction>
 * ```
 */
export function XRNearInteraction({
  position,
  rotation,
  scale,
  visible = true,
  children,
  onMount,
  onUnmount,
  enabled = true,
  hoverDistance = 0.05,
  selectDistance = 0.02,
  onNearEnter,
  onNearExit,
  onNearSelect,
  onNearSelectEnd,
  hapticOnHover = true,
  hapticOnSelect = true
}) {
  const parentHandle = useParentPanel();
  const [state, setState] = useState({
    isHovered: false,
    isSelected: false,
    interactionType: null,
    hitPoint: null,
    hitNormal: null
  });
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
  const handleNearEnter = useCallback(hand => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      isHovered: true,
      interactionType: 'near'
    }));
    onNearEnter?.(hand);
  }, [enabled, onNearEnter]);
  const handleNearExit = useCallback(hand => {
    setState(prev => ({
      ...prev,
      isHovered: false,
      interactionType: null
    }));
    onNearExit?.(hand);
  }, [onNearExit]);
  const handleNearSelect = useCallback((hand, point) => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      isSelected: true,
      hitPoint: point
    }));
    onNearSelect?.(hand, point);
  }, [enabled, onNearSelect]);
  const handleNearSelectEnd = useCallback(hand => {
    setState(prev => ({
      ...prev,
      isSelected: false
    }));
    onNearSelectEnd?.(hand);
  }, [onNearSelectEnd]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/_jsx(InteractionContext.Provider, {
    value: state,
    children: children
  });
}
XRNearInteraction.displayName = 'XRNearInteraction';

/**
 * XRRayInteraction Props
 */

/**
 * XRRayInteraction - Ray-based pointing interaction
 * 
 * Enables controller/hand ray interactions for distant objects.
 * 
 * @example
 * ```tsx
 * <XRRayInteraction 
 *   showRay
 *   onRaySelect={(hand, point) => console.log(`Selected at ${point}`)}
 * >
 *   <XRPanel position={[0, 1.5, -2]}>
 *     <XRButton>Click Me</XRButton>
 *   </XRPanel>
 * </XRRayInteraction>
 * ```
 */
export function XRRayInteraction({
  position,
  rotation,
  scale,
  visible = true,
  children,
  onMount,
  onUnmount,
  enabled = true,
  maxDistance = 10,
  showRay = true,
  rayColor = [0.2, 0.5, 1.0, 0.8],
  rayWidth = 0.002,
  cursorSize = 0.01,
  cursorColor = [1, 1, 1, 1],
  onRayEnter,
  onRayExit,
  onRaySelect,
  onRaySelectEnd,
  onRayMove
}) {
  const parentHandle = useParentPanel();
  const [state, setState] = useState({
    isHovered: false,
    isSelected: false,
    interactionType: null,
    hitPoint: null,
    hitNormal: null
  });
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
  const handleRayEnter = useCallback(hand => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      isHovered: true,
      interactionType: 'ray'
    }));
    onRayEnter?.(hand);
  }, [enabled, onRayEnter]);
  const handleRayExit = useCallback(hand => {
    setState(prev => ({
      ...prev,
      isHovered: false,
      interactionType: null,
      hitPoint: null
    }));
    onRayExit?.(hand);
  }, [onRayExit]);
  const handleRaySelect = useCallback((hand, point, normal) => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      isSelected: true,
      hitPoint: point,
      hitNormal: normal
    }));
    onRaySelect?.(hand, point, normal);
  }, [enabled, onRaySelect]);
  const handleRaySelectEnd = useCallback(hand => {
    setState(prev => ({
      ...prev,
      isSelected: false
    }));
    onRaySelectEnd?.(hand);
  }, [onRaySelectEnd]);
  const handleRayMove = useCallback((hand, point) => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      hitPoint: point
    }));
    onRayMove?.(hand, point);
  }, [enabled, onRayMove]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/_jsx(InteractionContext.Provider, {
    value: state,
    children: children
  });
}
XRRayInteraction.displayName = 'XRRayInteraction';
//# sourceMappingURL=XRInteraction.js.map
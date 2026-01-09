"use strict";

import { useState, useCallback } from 'react';
import { useXRNode } from './useXRNode';
import { useParentPanel } from './XRPanel';
import { XRColors } from './types';

/**
 * XRSlider Props
 */

/**
 * XRSlider - Interactive slider for value selection
 * 
 * Supports both horizontal and vertical orientations.
 * Works with near and ray interactions.
 * 
 * @example
 * ```tsx
 * <XRSlider
 *   min={0}
 *   max={100}
 *   value={volume}
 *   onChange={setVolume}
 *   showValue
 * />
 * ```
 */
export function XRSlider({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  onMount,
  onUnmount,
  disabled = false,
  onHover,
  onHoverEnd,
  onSelect,
  onSelectStart,
  onSelectEnd,
  value: controlledValue,
  defaultValue = 0,
  min = 0,
  max = 1,
  step = 0.01,
  orientation = 'horizontal',
  width = 0.2,
  showValue = false,
  formatValue = v => v.toFixed(2),
  onChange,
  onChangeEnd
}) {
  const parentHandle = useParentPanel();
  const [internalValue, setInternalValue] = useState(defaultValue);
  const [isDragging, setIsDragging] = useState(false);
  const [isHovered, setIsHovered] = useState(false);
  const value = controlledValue !== undefined ? controlledValue : internalValue;
  const normalizedValue = (value - min) / (max - min);
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
  const handleValueChange = useCallback(newValue => {
    const clampedValue = Math.max(min, Math.min(max, newValue));
    const steppedValue = Math.round(clampedValue / step) * step;
    if (controlledValue === undefined) {
      setInternalValue(steppedValue);
    }
    onChange?.(steppedValue);
  }, [min, max, step, controlledValue, onChange]);
  const handleDragStart = useCallback(() => {
    if (disabled) return;
    setIsDragging(true);
    onSelectStart?.();
  }, [disabled, onSelectStart]);
  const handleDragEnd = useCallback(() => {
    setIsDragging(false);
    onChangeEnd?.(value);
    onSelectEnd?.();
  }, [value, onChangeEnd, onSelectEnd]);
  const handleHover = useCallback(() => {
    if (disabled) return;
    setIsHovered(true);
    onHover?.();
  }, [disabled, onHover]);
  const handleHoverEnd = useCallback(() => {
    setIsHovered(false);
    onHoverEnd?.();
  }, [onHoverEnd]);

  // Track dimensions
  const trackHeight = 0.008;
  const thumbSize = 0.02;
  const trackColor = disabled ? XRColors.secondary : XRColors.border;
  const fillColor = disabled ? XRColors.secondary : XRColors.primary;
  const thumbColor = disabled ? XRColors.secondary : isHovered || isDragging ? XRColors.hover : XRColors.primary;

  // Calculate thumb position
  const thumbOffset = normalizedValue * width - width / 2;
  if (!isReady || !handle) {
    return null;
  }
  return null;
}
XRSlider.displayName = 'XRSlider';

/**
 * XRRangeSlider - Slider with two thumbs for range selection
 */

export function XRRangeSlider({
  defaultValue = [0, 1],
  minDistance = 0.1,
  ...props
}) {
  const [range, setRange] = useState(defaultValue);

  // Implementation would handle two thumbs
  return null;
}
XRRangeSlider.displayName = 'XRRangeSlider';
//# sourceMappingURL=XRSlider.js.map
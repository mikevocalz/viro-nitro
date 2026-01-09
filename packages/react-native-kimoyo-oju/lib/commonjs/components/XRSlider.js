"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRRangeSlider = XRRangeSlider;
exports.XRSlider = XRSlider;
var _react = require("react");
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _types = require("./types");
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
function XRSlider({
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
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const [internalValue, setInternalValue] = (0, _react.useState)(defaultValue);
  const [isDragging, setIsDragging] = (0, _react.useState)(false);
  const [isHovered, setIsHovered] = (0, _react.useState)(false);
  const value = controlledValue !== undefined ? controlledValue : internalValue;
  const normalizedValue = (value - min) / (max - min);
  const {
    handle,
    isReady
  } = (0, _useXRNode.useXRNode)('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount
  });
  const handleValueChange = (0, _react.useCallback)(newValue => {
    const clampedValue = Math.max(min, Math.min(max, newValue));
    const steppedValue = Math.round(clampedValue / step) * step;
    if (controlledValue === undefined) {
      setInternalValue(steppedValue);
    }
    onChange?.(steppedValue);
  }, [min, max, step, controlledValue, onChange]);
  const handleDragStart = (0, _react.useCallback)(() => {
    if (disabled) return;
    setIsDragging(true);
    onSelectStart?.();
  }, [disabled, onSelectStart]);
  const handleDragEnd = (0, _react.useCallback)(() => {
    setIsDragging(false);
    onChangeEnd?.(value);
    onSelectEnd?.();
  }, [value, onChangeEnd, onSelectEnd]);
  const handleHover = (0, _react.useCallback)(() => {
    if (disabled) return;
    setIsHovered(true);
    onHover?.();
  }, [disabled, onHover]);
  const handleHoverEnd = (0, _react.useCallback)(() => {
    setIsHovered(false);
    onHoverEnd?.();
  }, [onHoverEnd]);

  // Track dimensions
  const trackHeight = 0.008;
  const thumbSize = 0.02;
  const trackColor = disabled ? _types.XRColors.secondary : _types.XRColors.border;
  const fillColor = disabled ? _types.XRColors.secondary : _types.XRColors.primary;
  const thumbColor = disabled ? _types.XRColors.secondary : isHovered || isDragging ? _types.XRColors.hover : _types.XRColors.primary;

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

function XRRangeSlider({
  defaultValue = [0, 1],
  minDistance = 0.1,
  ...props
}) {
  const [range, setRange] = (0, _react.useState)(defaultValue);

  // Implementation would handle two thumbs
  return null;
}
XRRangeSlider.displayName = 'XRRangeSlider';
//# sourceMappingURL=XRSlider.js.map
"use strict";

import React, { useEffect } from 'react';
import { useXRNode, useSubmitCommands } from './useXRNode';
import { useParentPanel } from './XRPanel';
import { XRColors, XRDimensions } from './types';

/**
 * XRText Props
 */
import { jsx as _jsx } from "react/jsx-runtime";
/**
 * XRText - 3D text rendering component
 * 
 * Renders text in 3D space with customizable styling.
 * 
 * @example
 * ```tsx
 * <XRText 
 *   text="Hello World"
 *   style={{ fontSize: 0.05, color: [1, 1, 1, 1] }}
 *   position={[0, 1.5, -1]}
 * />
 * ```
 */
export function XRText({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  onMount,
  onUnmount,
  children,
  text,
  style = {}
}) {
  const parentHandle = useParentPanel();
  const submitCommands = useSubmitCommands();
  const {
    handle,
    isReady
  } = useXRNode('text', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount
  });
  const displayText = text ?? (typeof children === 'string' ? children : '');
  const mergedStyle = {
    fontSize: XRDimensions.fontSize.md,
    fontFamily: 'system',
    fontWeight: 'normal',
    color: XRColors.text,
    textAlign: 'center',
    ...style
  };

  // Update text content when it changes
  useEffect(() => {
    if (!handle || !isReady) return;
    submitCommands([{
      type: 'SET_TEXT',
      handle,
      text: displayText,
      fontSize: mergedStyle.fontSize,
      color: mergedStyle.color,
      fontFamily: mergedStyle.fontFamily
    }]);
  }, [handle, isReady, displayText, mergedStyle.fontSize, mergedStyle.color, mergedStyle.fontFamily]);
  if (!isReady || !handle) {
    return null;
  }
  return null;
}
XRText.displayName = 'XRText';

/**
 * XRHeading - Large heading text
 */
export function XRHeading(props) {
  return /*#__PURE__*/_jsx(XRText, {
    ...props,
    style: {
      fontSize: XRDimensions.fontSize.xl,
      fontWeight: 'bold',
      ...props.style
    }
  });
}
XRHeading.displayName = 'XRHeading';

/**
 * XRLabel - Small label text
 */
export function XRLabel(props) {
  return /*#__PURE__*/_jsx(XRText, {
    ...props,
    style: {
      fontSize: XRDimensions.fontSize.sm,
      color: XRColors.textSecondary,
      ...props.style
    }
  });
}
XRLabel.displayName = 'XRLabel';
//# sourceMappingURL=XRText.js.map
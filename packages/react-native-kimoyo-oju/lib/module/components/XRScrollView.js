"use strict";

import React, { useState, useCallback, useRef, Children, cloneElement, isValidElement } from 'react';
import { useXRNode } from './useXRNode';
import { useParentPanel } from './XRPanel';
import { XRColors } from './types';

/**
 * XRScrollView Props
 */
import { Fragment as _Fragment, jsx as _jsx } from "react/jsx-runtime";
/**
 * XRScrollView - Scrollable container for content
 * 
 * Supports horizontal, vertical, or both directions.
 * Works with near interaction (drag) and controllers (thumbstick).
 * 
 * @example
 * ```tsx
 * <XRScrollView width={0.4} height={0.3} contentHeight={1.0}>
 *   <XRStack direction="vertical" spacing={0.02}>
 *     {items.map(item => <XRButton key={item.id}>{item.label}</XRButton>)}
 *   </XRStack>
 * </XRScrollView>
 * ```
 */
export function XRScrollView({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  disabled = false,
  width = 0.4,
  height = 0.3,
  contentWidth,
  contentHeight,
  direction = 'vertical',
  showScrollbar = true,
  scrollbarWidth = 0.005,
  bounces = true,
  pagingEnabled = false,
  snapToInterval,
  onScroll,
  onScrollEnd
}) {
  const parentHandle = useParentPanel();
  const [scrollOffset, setScrollOffset] = useState({
    x: 0,
    y: 0
  });
  const [isDragging, setIsDragging] = useState(false);
  const velocityRef = useRef({
    x: 0,
    y: 0
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
  const effectiveContentWidth = contentWidth ?? width;
  const effectiveContentHeight = contentHeight ?? height;
  const maxScrollX = Math.max(0, effectiveContentWidth - width);
  const maxScrollY = Math.max(0, effectiveContentHeight - height);
  const canScrollX = direction === 'horizontal' || direction === 'both';
  const canScrollY = direction === 'vertical' || direction === 'both';
  const handleScroll = useCallback((deltaX, deltaY) => {
    if (disabled) return;
    setScrollOffset(prev => {
      let newX = prev.x;
      let newY = prev.y;
      if (canScrollX) {
        newX = Math.max(0, Math.min(maxScrollX, prev.x + deltaX));
      }
      if (canScrollY) {
        newY = Math.max(0, Math.min(maxScrollY, prev.y + deltaY));
      }
      const newOffset = {
        x: newX,
        y: newY
      };
      onScroll?.(newOffset);
      return newOffset;
    });
  }, [disabled, canScrollX, canScrollY, maxScrollX, maxScrollY, onScroll]);
  const handleDragStart = useCallback(() => {
    if (disabled) return;
    setIsDragging(true);
  }, [disabled]);
  const handleDragEnd = useCallback(() => {
    setIsDragging(false);
    onScrollEnd?.(scrollOffset);

    // Apply snapping if enabled
    if (snapToInterval) {
      setScrollOffset(prev => ({
        x: Math.round(prev.x / snapToInterval) * snapToInterval,
        y: Math.round(prev.y / snapToInterval) * snapToInterval
      }));
    }
  }, [scrollOffset, onScrollEnd, snapToInterval]);

  // Calculate content position based on scroll
  const contentPosition = [canScrollX ? scrollOffset.x : 0, canScrollY ? -scrollOffset.y : 0, 0];

  // Scrollbar calculations
  const scrollbarTrackColor = XRColors.border;
  const scrollbarThumbColor = XRColors.secondary;
  const verticalScrollbarHeight = height * (height / effectiveContentHeight);
  const verticalScrollbarOffset = scrollOffset.y / maxScrollY * (height - verticalScrollbarHeight);
  const horizontalScrollbarWidth = width * (width / effectiveContentWidth);
  const horizontalScrollbarOffset = scrollOffset.x / maxScrollX * (width - horizontalScrollbarWidth);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/_jsx(_Fragment, {
    children: Children.map(children, (child, index) => {
      if (! /*#__PURE__*/isValidElement(child)) return child;
      return /*#__PURE__*/cloneElement(child, {
        key: index,
        position: contentPosition
      });
    })
  });
}
XRScrollView.displayName = 'XRScrollView';

/**
 * ScrollViewer - Alias for XRScrollView with more desktop-like defaults
 */
export function ScrollViewer(props) {
  return /*#__PURE__*/_jsx(XRScrollView, {
    ...props,
    showScrollbar: props.showScrollbar ?? true,
    bounces: props.bounces ?? false
  });
}
ScrollViewer.displayName = 'ScrollViewer';
//# sourceMappingURL=XRScrollView.js.map
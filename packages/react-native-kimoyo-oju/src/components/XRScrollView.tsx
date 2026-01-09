import React, { useState, useCallback, useRef, Children, cloneElement, isValidElement } from 'react'
import type { Handle, Vec3 } from '../types'
import { useXRNode } from './useXRNode'
import { useParentPanel } from './XRPanel'
import type { XRInteractiveProps, ScrollDirection } from './types'
import { XRColors } from './types'

/**
 * XRScrollView Props
 */
export interface XRScrollViewProps extends XRInteractiveProps {
  width?: number
  height?: number
  contentWidth?: number
  contentHeight?: number
  direction?: ScrollDirection
  showScrollbar?: boolean
  scrollbarWidth?: number
  bounces?: boolean
  pagingEnabled?: boolean
  snapToInterval?: number
  onScroll?: (offset: { x: number; y: number }) => void
  onScrollEnd?: (offset: { x: number; y: number }) => void
}

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
  onScrollEnd,
}: XRScrollViewProps) {
  const parentHandle = useParentPanel()
  const [scrollOffset, setScrollOffset] = useState({ x: 0, y: 0 })
  const [isDragging, setIsDragging] = useState(false)
  const velocityRef = useRef({ x: 0, y: 0 })
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  const effectiveContentWidth = contentWidth ?? width
  const effectiveContentHeight = contentHeight ?? height
  
  const maxScrollX = Math.max(0, effectiveContentWidth - width)
  const maxScrollY = Math.max(0, effectiveContentHeight - height)
  
  const canScrollX = direction === 'horizontal' || direction === 'both'
  const canScrollY = direction === 'vertical' || direction === 'both'

  const handleScroll = useCallback((deltaX: number, deltaY: number) => {
    if (disabled) return
    
    setScrollOffset(prev => {
      let newX = prev.x
      let newY = prev.y
      
      if (canScrollX) {
        newX = Math.max(0, Math.min(maxScrollX, prev.x + deltaX))
      }
      if (canScrollY) {
        newY = Math.max(0, Math.min(maxScrollY, prev.y + deltaY))
      }
      
      const newOffset = { x: newX, y: newY }
      onScroll?.(newOffset)
      return newOffset
    })
  }, [disabled, canScrollX, canScrollY, maxScrollX, maxScrollY, onScroll])

  const handleDragStart = useCallback(() => {
    if (disabled) return
    setIsDragging(true)
  }, [disabled])

  const handleDragEnd = useCallback(() => {
    setIsDragging(false)
    onScrollEnd?.(scrollOffset)
    
    // Apply snapping if enabled
    if (snapToInterval) {
      setScrollOffset(prev => ({
        x: Math.round(prev.x / snapToInterval) * snapToInterval,
        y: Math.round(prev.y / snapToInterval) * snapToInterval,
      }))
    }
  }, [scrollOffset, onScrollEnd, snapToInterval])

  // Calculate content position based on scroll
  const contentPosition: Vec3 = [
    canScrollX ? scrollOffset.x : 0,
    canScrollY ? -scrollOffset.y : 0,
    0,
  ]

  // Scrollbar calculations
  const scrollbarTrackColor = XRColors.border
  const scrollbarThumbColor = XRColors.secondary
  
  const verticalScrollbarHeight = height * (height / effectiveContentHeight)
  const verticalScrollbarOffset = (scrollOffset.y / maxScrollY) * (height - verticalScrollbarHeight)
  
  const horizontalScrollbarWidth = width * (width / effectiveContentWidth)
  const horizontalScrollbarOffset = (scrollOffset.x / maxScrollX) * (width - horizontalScrollbarWidth)

  if (!isReady || !handle) {
    return null
  }

  return (
    <>
      {Children.map(children, (child, index: number) => {
        if (!isValidElement(child)) return child
        
        return cloneElement(child as React.ReactElement<{ position?: Vec3 }>, {
          key: index,
          position: contentPosition,
        })
      })}
    </>
  )
}

XRScrollView.displayName = 'XRScrollView'

/**
 * ScrollViewer - Alias for XRScrollView with more desktop-like defaults
 */
export function ScrollViewer(props: XRScrollViewProps) {
  return (
    <XRScrollView
      {...props}
      showScrollbar={props.showScrollbar ?? true}
      bounces={props.bounces ?? false}
    />
  )
}

ScrollViewer.displayName = 'ScrollViewer'

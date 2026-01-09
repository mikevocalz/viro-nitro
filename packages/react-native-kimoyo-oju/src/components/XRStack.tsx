import React, { Children, cloneElement, isValidElement, useMemo } from 'react'
import type { Handle, Vec3 } from '../types'
import { useXRNode } from './useXRNode'
import { useParentPanel } from './XRPanel'
import type { XRBaseProps, LayoutDirection, Alignment, JustifyContent } from './types'
import { XRDimensions } from './types'

/**
 * XRStack Props
 */
export interface XRStackProps extends XRBaseProps {
  direction?: LayoutDirection
  spacing?: number
  alignment?: Alignment
  justifyContent?: JustifyContent
  wrap?: boolean
  maxWidth?: number
  maxHeight?: number
}

/**
 * Calculate child positions based on layout
 */
function calculateChildPositions(
  childCount: number,
  direction: LayoutDirection,
  spacing: number,
  alignment: Alignment,
  justifyContent: JustifyContent
): Vec3[] {
  const positions: Vec3[] = []
  
  for (let i = 0; i < childCount; i++) {
    let x = 0
    let y = 0
    let z = 0
    
    const offset = i * spacing
    
    switch (direction) {
      case 'horizontal':
        x = offset
        break
      case 'vertical':
        y = -offset // Negative because Y grows upward in 3D
        break
      case 'depth':
        z = -offset // Negative to go away from viewer
        break
    }
    
    positions.push([x, y, z])
  }
  
  // Center the stack based on total size
  if (childCount > 0) {
    const totalSize = (childCount - 1) * spacing
    const centerOffset = totalSize / 2
    
    for (let i = 0; i < positions.length; i++) {
      switch (direction) {
        case 'horizontal':
          positions[i][0] -= centerOffset
          break
        case 'vertical':
          positions[i][1] += centerOffset
          break
        case 'depth':
          positions[i][2] += centerOffset
          break
      }
    }
  }
  
  return positions
}

/**
 * XRStack - Layout children in a horizontal, vertical, or depth stack
 * 
 * Similar to CSS flexbox but for 3D space.
 * 
 * @example
 * ```tsx
 * <XRStack direction="horizontal" spacing={0.05}>
 *   <XRButton>Button 1</XRButton>
 *   <XRButton>Button 2</XRButton>
 *   <XRButton>Button 3</XRButton>
 * </XRStack>
 * ```
 */
export function XRStack({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  direction = 'horizontal',
  spacing = XRDimensions.spacing.md,
  alignment = 'center',
  justifyContent = 'center',
  wrap = false,
  maxWidth,
  maxHeight,
}: XRStackProps) {
  const parentHandle = useParentPanel()
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  const childArray = Children.toArray(children)
  const childPositions = useMemo(
    () => calculateChildPositions(childArray.length, direction, spacing, alignment, justifyContent),
    [childArray.length, direction, spacing, alignment, justifyContent]
  )

  if (!isReady || !handle) {
    return null
  }

  return (
    <>
      {childArray.map((child, index) => {
        if (!isValidElement(child)) return child
        
        const childPosition = childPositions[index] || [0, 0, 0]
        
        return cloneElement(child as React.ReactElement<{ position?: Vec3 }>, {
          key: index,
          position: childPosition,
        })
      })}
    </>
  )
}

XRStack.displayName = 'XRStack'

/**
 * Convenience component for horizontal stack
 */
export function XRHStack(props: Omit<XRStackProps, 'direction'>) {
  return <XRStack {...props} direction="horizontal" />
}

XRHStack.displayName = 'XRHStack'

/**
 * Convenience component for vertical stack
 */
export function XRVStack(props: Omit<XRStackProps, 'direction'>) {
  return <XRStack {...props} direction="vertical" />
}

XRVStack.displayName = 'XRVStack'

/**
 * Convenience component for depth stack
 */
export function XRZStack(props: Omit<XRStackProps, 'direction'>) {
  return <XRStack {...props} direction="depth" />
}

XRZStack.displayName = 'XRZStack'

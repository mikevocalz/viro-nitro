import React, { Children, cloneElement, isValidElement, useMemo } from 'react'
import type { Handle, Vec3 } from '../types'
import { useXRNode } from './useXRNode'
import { useParentPanel } from './XRPanel'
import type { XRBaseProps, Alignment, JustifyContent } from './types'
import { XRDimensions } from './types'

/**
 * XRGrid Props
 */
export interface XRGridProps extends XRBaseProps {
  columns?: number
  rows?: number
  cellWidth?: number
  cellHeight?: number
  spacingX?: number
  spacingY?: number
  alignItems?: Alignment
  justifyItems?: JustifyContent
}

/**
 * Calculate grid cell positions
 */
function calculateGridPositions(
  childCount: number,
  columns: number,
  cellWidth: number,
  cellHeight: number,
  spacingX: number,
  spacingY: number
): Vec3[] {
  const positions: Vec3[] = []
  
  const rows = Math.ceil(childCount / columns)
  const totalWidth = columns * cellWidth + (columns - 1) * spacingX
  const totalHeight = rows * cellHeight + (rows - 1) * spacingY
  
  for (let i = 0; i < childCount; i++) {
    const col = i % columns
    const row = Math.floor(i / columns)
    
    const x = col * (cellWidth + spacingX) - totalWidth / 2 + cellWidth / 2
    const y = -(row * (cellHeight + spacingY) - totalHeight / 2 + cellHeight / 2)
    
    positions.push([x, y, 0])
  }
  
  return positions
}

/**
 * XRGrid - Layout children in a 2D grid
 * 
 * Perfect for icon grids, image galleries, or any grid-based UI.
 * 
 * @example
 * ```tsx
 * <XRGrid columns={3} cellWidth={0.1} cellHeight={0.1} spacingX={0.02} spacingY={0.02}>
 *   <XRButton>1</XRButton>
 *   <XRButton>2</XRButton>
 *   <XRButton>3</XRButton>
 *   <XRButton>4</XRButton>
 *   <XRButton>5</XRButton>
 *   <XRButton>6</XRButton>
 * </XRGrid>
 * ```
 */
export function XRGrid({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  columns = 3,
  rows,
  cellWidth = 0.1,
  cellHeight = 0.1,
  spacingX = XRDimensions.spacing.sm,
  spacingY = XRDimensions.spacing.sm,
  alignItems = 'center',
  justifyItems = 'center',
}: XRGridProps) {
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
  const gridPositions = useMemo(
    () => calculateGridPositions(childArray.length, columns, cellWidth, cellHeight, spacingX, spacingY),
    [childArray.length, columns, cellWidth, cellHeight, spacingX, spacingY]
  )

  if (!isReady || !handle) {
    return null
  }

  return (
    <>
      {childArray.map((child, index: number) => {
        if (!isValidElement(child)) return child
        
        const cellPosition = gridPositions[index] || [0, 0, 0]
        
        return cloneElement(child as React.ReactElement<{ position?: Vec3 }>, {
          key: index,
          position: cellPosition,
        })
      })}
    </>
  )
}

XRGrid.displayName = 'XRGrid'

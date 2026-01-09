import React, { useEffect } from 'react'
import type { Handle, Color, CommandBuffer } from '../types'
import { useXRNode, useSubmitCommands } from './useXRNode'
import { useParentPanel } from './XRPanel'
import type { XRBaseProps, CylinderMapping } from './types'
import { XRColors } from './types'

/**
 * XRCylinder Props
 */
export interface XRCylinderProps extends XRBaseProps {
  radius?: number
  height?: number
  radialSegments?: number
  heightSegments?: number
  openEnded?: boolean
  thetaStart?: number
  thetaLength?: number
  color?: Color
  texture?: string
  mapping?: CylinderMapping
  lightingEnabled?: boolean
}

/**
 * XRCylinder - Cylindrical geometry component
 * 
 * Perfect for curved displays, panoramic content, or curved UI panels.
 * 
 * @example
 * ```tsx
 * // Curved display panel
 * <XRCylinder
 *   radius={2}
 *   height={1}
 *   thetaLength={Math.PI / 2}
 *   mapping="inside"
 *   texture="panorama.jpg"
 * />
 * 
 * // Simple cylinder
 * <XRCylinder
 *   radius={0.1}
 *   height={0.5}
 *   color={[0.8, 0.2, 0.2, 1]}
 * />
 * ```
 */
export function XRCylinder({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  radius = 0.5,
  height = 1.0,
  radialSegments = 32,
  heightSegments = 1,
  openEnded = false,
  thetaStart = 0,
  thetaLength = Math.PI * 2,
  color = XRColors.surface,
  texture,
  mapping = 'outside',
  lightingEnabled = true,
}: XRCylinderProps) {
  const parentHandle = useParentPanel()
  const submitCommands = useSubmitCommands()
  
  const { handle, isReady } = useXRNode('mesh', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  // Set geometry
  useEffect(() => {
    if (!handle || !isReady) return

    submitCommands([
      {
        type: 'SET_GEOMETRY',
        handle,
        geometryType: 'cylinder',
        dimensions: [radius, height, radialSegments],
      },
    ])
  }, [handle, isReady, radius, height, radialSegments])

  // Set material
  useEffect(() => {
    if (!handle || !isReady) return

    submitCommands([
      {
        type: 'SET_MATERIAL',
        handle,
        diffuseColor: color,
        specularColor: [1, 1, 1, 1],
        shininess: 0.5,
        diffuseTexture: null,
        normalTexture: null,
      },
    ])
  }, [handle, isReady, color])

  if (!isReady || !handle) {
    return null
  }

  return null
}

XRCylinder.displayName = 'XRCylinder'

/**
 * XRCurvedPanel - Convenience component for curved UI panels
 */
export interface XRCurvedPanelProps extends XRBaseProps {
  width?: number
  height?: number
  curveRadius?: number
  curveAngle?: number
  children?: React.ReactNode
}

export function XRCurvedPanel({
  width = 1,
  height = 0.5,
  curveRadius = 2,
  curveAngle = Math.PI / 3,
  children,
  ...props
}: XRCurvedPanelProps) {
  return (
    <XRCylinder
      {...props}
      radius={curveRadius}
      height={height}
      thetaStart={-curveAngle / 2}
      thetaLength={curveAngle}
      mapping="inside"
      openEnded
    >
      {children}
    </XRCylinder>
  )
}

XRCurvedPanel.displayName = 'XRCurvedPanel'

import React, { useEffect, useState } from 'react'
import type { Handle, Vec3 } from '../types'
import { useXRNode } from './useXRNode'
import type { XRBaseProps, HUDAnchor } from './types'

/**
 * XRHUD Props
 */
export interface XRHUDProps extends XRBaseProps {
  anchor?: HUDAnchor
  distance?: number
  offsetX?: number
  offsetY?: number
  followHead?: boolean
  followSpeed?: number
  fadeAtEdge?: boolean
  alwaysVisible?: boolean
}

/**
 * Calculate HUD position based on anchor
 */
function getAnchorOffset(anchor: HUDAnchor, distance: number): Vec3 {
  const horizontalFOV = 0.8 // Approximate horizontal FOV factor
  const verticalFOV = 0.5 // Approximate vertical FOV factor
  
  const positions: Record<HUDAnchor, Vec3> = {
    'top-left': [-horizontalFOV * distance, verticalFOV * distance, -distance],
    'top-center': [0, verticalFOV * distance, -distance],
    'top-right': [horizontalFOV * distance, verticalFOV * distance, -distance],
    'center-left': [-horizontalFOV * distance, 0, -distance],
    'center': [0, 0, -distance],
    'center-right': [horizontalFOV * distance, 0, -distance],
    'bottom-left': [-horizontalFOV * distance, -verticalFOV * distance, -distance],
    'bottom-center': [0, -verticalFOV * distance, -distance],
    'bottom-right': [horizontalFOV * distance, -verticalFOV * distance, -distance],
  }
  
  return positions[anchor]
}

/**
 * XRHUD - Head-locked UI overlay
 * 
 * Creates UI that follows the user's head position.
 * Perfect for status indicators, notifications, or always-visible controls.
 * 
 * @example
 * ```tsx
 * <XRHUD anchor="bottom-center" distance={1}>
 *   <XRPanel>
 *     <XRText>Health: 100</XRText>
 *   </XRPanel>
 * </XRHUD>
 * ```
 */
export function XRHUD({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  anchor = 'center',
  distance = 1.5,
  offsetX = 0,
  offsetY = 0,
  followHead = true,
  followSpeed = 5,
  fadeAtEdge = true,
  alwaysVisible = true,
}: XRHUDProps) {
  const [hudPosition, setHudPosition] = useState<Vec3>([0, 0, -distance])
  
  const anchorOffset = getAnchorOffset(anchor, distance)
  const finalPosition: Vec3 = position ?? [
    anchorOffset[0] + offsetX,
    anchorOffset[1] + offsetY,
    anchorOffset[2],
  ]
  
  const { handle, isReady } = useXRNode('group', {
    position: finalPosition,
    rotation,
    scale,
    visible,
    onMount,
    onUnmount,
  })

  if (!isReady || !handle) {
    return null
  }

  return <>{children}</>
}

XRHUD.displayName = 'XRHUD'

/**
 * XRTooltip - Contextual tooltip that appears near interaction point
 */
export interface XRTooltipProps extends XRBaseProps {
  text: string
  show?: boolean
  delay?: number
  placement?: 'top' | 'bottom' | 'left' | 'right'
  maxWidth?: number
}

export function XRTooltip({
  position,
  rotation,
  scale,
  visible = true,
  children,
  onMount,
  onUnmount,
  text,
  show = false,
  delay = 500,
  placement = 'top',
  maxWidth = 0.2,
}: XRTooltipProps) {
  const [isVisible, setIsVisible] = useState(false)
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible: visible && isVisible,
    onMount,
    onUnmount,
  })

  useEffect(() => {
    if (show) {
      const timer = setTimeout(() => setIsVisible(true), delay)
      return () => clearTimeout(timer)
    } else {
      setIsVisible(false)
    }
  }, [show, delay])

  if (!isReady || !handle) {
    return null
  }

  return <>{children}</>
}

XRTooltip.displayName = 'XRTooltip'

/**
 * XRNotification - Toast-like notification
 */
export interface XRNotificationProps extends XRBaseProps {
  message: string
  type?: 'info' | 'success' | 'warning' | 'error'
  duration?: number
  onDismiss?: () => void
}

export function XRNotification({
  position,
  rotation,
  scale,
  visible = true,
  onMount,
  onUnmount,
  message,
  type = 'info',
  duration = 3000,
  onDismiss,
}: XRNotificationProps) {
  const [isVisible, setIsVisible] = useState(true)
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible: visible && isVisible,
    onMount,
    onUnmount,
  })

  useEffect(() => {
    if (duration > 0) {
      const timer = setTimeout(() => {
        setIsVisible(false)
        onDismiss?.()
      }, duration)
      return () => clearTimeout(timer)
    }
  }, [duration, onDismiss])

  if (!isReady || !handle || !isVisible) {
    return null
  }

  return null
}

XRNotification.displayName = 'XRNotification'

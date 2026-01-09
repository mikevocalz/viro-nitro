import React, { createContext, useContext, useCallback, useState, useRef } from 'react'
import type { Handle, Vec3 } from '../types'
import { useXRNode } from './useXRNode'
import { useParentPanel } from './XRPanel'
import type { XRBaseProps, InteractionType } from './types'

/**
 * Interaction context for nested components
 */
export interface InteractionState {
  isHovered: boolean
  isSelected: boolean
  interactionType: InteractionType | null
  hitPoint: Vec3 | null
  hitNormal: Vec3 | null
}

const InteractionContext = createContext<InteractionState>({
  isHovered: false,
  isSelected: false,
  interactionType: null,
  hitPoint: null,
  hitNormal: null,
})

export function useInteraction(): InteractionState {
  return useContext(InteractionContext)
}

/**
 * XRNearInteraction Props
 */
export interface XRNearInteractionProps extends XRBaseProps {
  enabled?: boolean
  hoverDistance?: number
  selectDistance?: number
  onNearEnter?: (hand: 'left' | 'right') => void
  onNearExit?: (hand: 'left' | 'right') => void
  onNearSelect?: (hand: 'left' | 'right', point: Vec3) => void
  onNearSelectEnd?: (hand: 'left' | 'right') => void
  hapticOnHover?: boolean
  hapticOnSelect?: boolean
}

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
  hapticOnSelect = true,
}: XRNearInteractionProps) {
  const parentHandle = useParentPanel()
  const [state, setState] = useState<InteractionState>({
    isHovered: false,
    isSelected: false,
    interactionType: null,
    hitPoint: null,
    hitNormal: null,
  })
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  const handleNearEnter = useCallback((hand: 'left' | 'right') => {
    if (!enabled) return
    setState(prev => ({ ...prev, isHovered: true, interactionType: 'near' }))
    onNearEnter?.(hand)
  }, [enabled, onNearEnter])

  const handleNearExit = useCallback((hand: 'left' | 'right') => {
    setState(prev => ({ ...prev, isHovered: false, interactionType: null }))
    onNearExit?.(hand)
  }, [onNearExit])

  const handleNearSelect = useCallback((hand: 'left' | 'right', point: Vec3) => {
    if (!enabled) return
    setState(prev => ({ ...prev, isSelected: true, hitPoint: point }))
    onNearSelect?.(hand, point)
  }, [enabled, onNearSelect])

  const handleNearSelectEnd = useCallback((hand: 'left' | 'right') => {
    setState(prev => ({ ...prev, isSelected: false }))
    onNearSelectEnd?.(hand)
  }, [onNearSelectEnd])

  if (!isReady || !handle) {
    return null
  }

  return (
    <InteractionContext.Provider value={state}>
      {children}
    </InteractionContext.Provider>
  )
}

XRNearInteraction.displayName = 'XRNearInteraction'

/**
 * XRRayInteraction Props
 */
export interface XRRayInteractionProps extends XRBaseProps {
  enabled?: boolean
  maxDistance?: number
  showRay?: boolean
  rayColor?: [number, number, number, number]
  rayWidth?: number
  cursorSize?: number
  cursorColor?: [number, number, number, number]
  onRayEnter?: (hand: 'left' | 'right') => void
  onRayExit?: (hand: 'left' | 'right') => void
  onRaySelect?: (hand: 'left' | 'right', point: Vec3, normal: Vec3) => void
  onRaySelectEnd?: (hand: 'left' | 'right') => void
  onRayMove?: (hand: 'left' | 'right', point: Vec3) => void
}

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
  onRayMove,
}: XRRayInteractionProps) {
  const parentHandle = useParentPanel()
  const [state, setState] = useState<InteractionState>({
    isHovered: false,
    isSelected: false,
    interactionType: null,
    hitPoint: null,
    hitNormal: null,
  })
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  const handleRayEnter = useCallback((hand: 'left' | 'right') => {
    if (!enabled) return
    setState(prev => ({ ...prev, isHovered: true, interactionType: 'ray' }))
    onRayEnter?.(hand)
  }, [enabled, onRayEnter])

  const handleRayExit = useCallback((hand: 'left' | 'right') => {
    setState(prev => ({ ...prev, isHovered: false, interactionType: null, hitPoint: null }))
    onRayExit?.(hand)
  }, [onRayExit])

  const handleRaySelect = useCallback((hand: 'left' | 'right', point: Vec3, normal: Vec3) => {
    if (!enabled) return
    setState(prev => ({ ...prev, isSelected: true, hitPoint: point, hitNormal: normal }))
    onRaySelect?.(hand, point, normal)
  }, [enabled, onRaySelect])

  const handleRaySelectEnd = useCallback((hand: 'left' | 'right') => {
    setState(prev => ({ ...prev, isSelected: false }))
    onRaySelectEnd?.(hand)
  }, [onRaySelectEnd])

  const handleRayMove = useCallback((hand: 'left' | 'right', point: Vec3) => {
    if (!enabled) return
    setState(prev => ({ ...prev, hitPoint: point }))
    onRayMove?.(hand, point)
  }, [enabled, onRayMove])

  if (!isReady || !handle) {
    return null
  }

  return (
    <InteractionContext.Provider value={state}>
      {children}
    </InteractionContext.Provider>
  )
}

XRRayInteraction.displayName = 'XRRayInteraction'

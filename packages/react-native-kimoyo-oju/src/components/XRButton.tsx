import React, { useState, useCallback, useMemo } from 'react'
import type { Handle, Color } from '../types'
import { useXRNode } from './useXRNode'
import { useParentPanel } from './XRPanel'
import type { XRInteractiveProps, ButtonVariant, ButtonSize, TextStyle } from './types'
import { XRColors, XRDimensions } from './types'

/**
 * XRButton Props
 */
export interface XRButtonProps extends XRInteractiveProps {
  label?: string
  children?: string
  variant?: ButtonVariant
  size?: ButtonSize
  icon?: string
  iconPosition?: 'left' | 'right'
  fullWidth?: boolean
  loading?: boolean
  textStyle?: TextStyle
}

/**
 * Get button colors based on variant and state
 */
function getButtonColors(
  variant: ButtonVariant,
  isHovered: boolean,
  isPressed: boolean,
  disabled: boolean
): { bg: Color; text: Color; border: Color } {
  if (disabled) {
    return {
      bg: [0.3, 0.3, 0.3, 0.5],
      text: [0.5, 0.5, 0.5, 1.0],
      border: [0.3, 0.3, 0.3, 0.5],
    }
  }

  const colors: Record<ButtonVariant, { bg: Color; text: Color; border: Color }> = {
    primary: {
      bg: isPressed ? XRColors.active : isHovered ? XRColors.hover : XRColors.primary,
      text: XRColors.text,
      border: XRColors.primary,
    },
    secondary: {
      bg: isPressed ? [0.35, 0.35, 0.35, 1.0] : isHovered ? [0.3, 0.3, 0.3, 1.0] : XRColors.secondary,
      text: XRColors.text,
      border: XRColors.border,
    },
    ghost: {
      bg: isPressed ? [0.2, 0.2, 0.2, 0.8] : isHovered ? [0.15, 0.15, 0.15, 0.6] : [0, 0, 0, 0],
      text: XRColors.text,
      border: [0, 0, 0, 0],
    },
    danger: {
      bg: isPressed ? [0.7, 0.1, 0.1, 1.0] : isHovered ? [0.8, 0.15, 0.15, 1.0] : XRColors.danger,
      text: XRColors.text,
      border: XRColors.danger,
    },
  }

  return colors[variant]
}

/**
 * Get button dimensions based on size
 */
function getButtonDimensions(size: ButtonSize): { height: number; paddingX: number; fontSize: number } {
  const dimensions: Record<ButtonSize, { height: number; paddingX: number; fontSize: number }> = {
    sm: { height: XRDimensions.buttonHeight.sm, paddingX: 0.015, fontSize: XRDimensions.fontSize.sm },
    md: { height: XRDimensions.buttonHeight.md, paddingX: 0.02, fontSize: XRDimensions.fontSize.md },
    lg: { height: XRDimensions.buttonHeight.lg, paddingX: 0.025, fontSize: XRDimensions.fontSize.lg },
  }
  return dimensions[size]
}

/**
 * XRButton - Interactive 3D button component
 * 
 * Supports multiple variants, sizes, and interaction states.
 * 
 * @example
 * ```tsx
 * <XRButton 
 *   variant="primary" 
 *   size="md"
 *   onSelect={() => console.log('Clicked!')}
 * >
 *   Click Me
 * </XRButton>
 * ```
 */
export function XRButton({
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
  label,
  children,
  variant = 'primary',
  size = 'md',
  icon,
  iconPosition = 'left',
  fullWidth = false,
  loading = false,
  textStyle,
}: XRButtonProps) {
  const parentHandle = useParentPanel()
  const [isHovered, setIsHovered] = useState(false)
  const [isPressed, setIsPressed] = useState(false)
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  const displayText = label ?? (typeof children === 'string' ? children : '')
  const dimensions = getButtonDimensions(size)
  const colors = getButtonColors(variant, isHovered, isPressed, disabled)

  const handleHover = useCallback(() => {
    if (disabled) return
    setIsHovered(true)
    onHover?.()
  }, [disabled, onHover])

  const handleHoverEnd = useCallback(() => {
    setIsHovered(false)
    onHoverEnd?.()
  }, [onHoverEnd])

  const handleSelectStart = useCallback(() => {
    if (disabled) return
    setIsPressed(true)
    onSelectStart?.()
  }, [disabled, onSelectStart])

  const handleSelectEnd = useCallback(() => {
    setIsPressed(false)
    onSelectEnd?.()
  }, [onSelectEnd])

  const handleSelect = useCallback(() => {
    if (disabled || loading) return
    onSelect?.()
  }, [disabled, loading, onSelect])

  if (!isReady || !handle) {
    return null
  }

  return null
}

XRButton.displayName = 'XRButton'

/**
 * XRIconButton - Button with only an icon
 */
export interface XRIconButtonProps extends Omit<XRButtonProps, 'label' | 'children' | 'fullWidth'> {
  icon: string
  'aria-label': string
}

export function XRIconButton({ icon, size = 'md', ...props }: XRIconButtonProps) {
  return <XRButton {...props} icon={icon} size={size} variant={props.variant ?? 'ghost'} />
}

XRIconButton.displayName = 'XRIconButton'

import React, { useState, useCallback } from 'react'
import type { Handle } from '../types'
import { useXRNode } from './useXRNode'
import { useParentPanel } from './XRPanel'
import type { XRInteractiveProps } from './types'
import { XRColors, XRDimensions } from './types'

/**
 * XRCheckbox Props
 */
export interface XRCheckboxProps extends XRInteractiveProps {
  checked?: boolean
  defaultChecked?: boolean
  label?: string
  size?: 'sm' | 'md' | 'lg'
  onChange?: (checked: boolean) => void
}

/**
 * XRCheckbox - Toggle checkbox component
 * 
 * @example
 * ```tsx
 * <XRCheckbox
 *   checked={enabled}
 *   onChange={setEnabled}
 *   label="Enable feature"
 * />
 * ```
 */
export function XRCheckbox({
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
  checked: controlledChecked,
  defaultChecked = false,
  label,
  size = 'md',
  onChange,
}: XRCheckboxProps) {
  const parentHandle = useParentPanel()
  const [internalChecked, setInternalChecked] = useState(defaultChecked)
  const [isHovered, setIsHovered] = useState(false)
  
  const checked = controlledChecked !== undefined ? controlledChecked : internalChecked
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  const boxSize = {
    sm: 0.02,
    md: 0.025,
    lg: 0.03,
  }[size]

  const handleToggle = useCallback(() => {
    if (disabled) return
    const newValue = !checked
    if (controlledChecked === undefined) {
      setInternalChecked(newValue)
    }
    onChange?.(newValue)
    onSelect?.()
  }, [disabled, checked, controlledChecked, onChange, onSelect])

  const handleHover = useCallback(() => {
    if (disabled) return
    setIsHovered(true)
    onHover?.()
  }, [disabled, onHover])

  const handleHoverEnd = useCallback(() => {
    setIsHovered(false)
    onHoverEnd?.()
  }, [onHoverEnd])

  if (!isReady || !handle) {
    return null
  }

  return null
}

XRCheckbox.displayName = 'XRCheckbox'

/**
 * XRSwitch - Toggle switch variant
 */
export interface XRSwitchProps extends Omit<XRCheckboxProps, 'size'> {
  size?: 'sm' | 'md' | 'lg'
}

export function XRSwitch(props: XRSwitchProps) {
  // Switch is visually different but same behavior
  return <XRCheckbox {...props} />
}

XRSwitch.displayName = 'XRSwitch'

/**
 * XRRadio - Radio button component
 */
export interface XRRadioProps extends XRInteractiveProps {
  value: string
  selectedValue?: string
  label?: string
  size?: 'sm' | 'md' | 'lg'
  onSelect?: () => void
}

export function XRRadio({
  position,
  rotation,
  scale,
  visible = true,
  disabled = false,
  value,
  selectedValue,
  label,
  size = 'md',
  onSelect,
  onMount,
  onUnmount,
}: XRRadioProps) {
  const parentHandle = useParentPanel()
  const [isHovered, setIsHovered] = useState(false)
  const isSelected = value === selectedValue
  
  const { handle, isReady } = useXRNode('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  if (!isReady || !handle) {
    return null
  }

  return null
}

XRRadio.displayName = 'XRRadio'

import type { ReactNode } from 'react'
import type { Handle, Vec3, Quat, Color } from '../types'

/**
 * Base props shared by all XR components
 */
export interface XRBaseProps {
  position?: Vec3
  rotation?: Quat
  scale?: Vec3 | number
  visible?: boolean
  opacity?: number
  children?: ReactNode
  onMount?: (handle: Handle) => void
  onUnmount?: () => void
}

/**
 * Interactive component props
 */
export interface XRInteractiveProps extends XRBaseProps {
  disabled?: boolean
  onHover?: () => void
  onHoverEnd?: () => void
  onSelect?: () => void
  onSelectStart?: () => void
  onSelectEnd?: () => void
}

/**
 * Layout direction for stacks
 */
export type LayoutDirection = 'horizontal' | 'vertical' | 'depth'

/**
 * Alignment options
 */
export type Alignment = 'start' | 'center' | 'end' | 'stretch'

/**
 * Justify content options
 */
export type JustifyContent = 'start' | 'center' | 'end' | 'space-between' | 'space-around' | 'space-evenly'

/**
 * Panel background style
 */
export interface PanelStyle {
  backgroundColor?: Color
  backgroundOpacity?: number
  cornerRadius?: number
  borderColor?: Color
  borderWidth?: number
  padding?: number | [number, number, number, number]
  shadow?: boolean
  shadowColor?: Color
  shadowOffset?: Vec3
  shadowBlur?: number
  blurBackground?: boolean
  blurAmount?: number
}

/**
 * Text style
 */
export interface TextStyle {
  fontSize?: number
  fontFamily?: string
  fontWeight?: 'normal' | 'bold' | '100' | '200' | '300' | '400' | '500' | '600' | '700' | '800' | '900'
  color?: Color
  textAlign?: 'left' | 'center' | 'right'
  lineHeight?: number
  letterSpacing?: number
  maxWidth?: number
  maxLines?: number
  overflow?: 'visible' | 'clip' | 'ellipsis'
}

/**
 * Button variant
 */
export type ButtonVariant = 'primary' | 'secondary' | 'ghost' | 'danger'

/**
 * Button size
 */
export type ButtonSize = 'sm' | 'md' | 'lg'

/**
 * Slider orientation
 */
export type SliderOrientation = 'horizontal' | 'vertical'

/**
 * Scroll direction
 */
export type ScrollDirection = 'horizontal' | 'vertical' | 'both'

/**
 * Interaction type
 */
export type InteractionType = 'near' | 'ray' | 'gaze'

/**
 * HUD anchor position
 */
export type HUDAnchor = 
  | 'top-left' | 'top-center' | 'top-right'
  | 'center-left' | 'center' | 'center-right'
  | 'bottom-left' | 'bottom-center' | 'bottom-right'

/**
 * Video playback state
 */
export type VideoState = 'idle' | 'loading' | 'playing' | 'paused' | 'ended' | 'error'

/**
 * Cylinder mapping mode
 */
export type CylinderMapping = 'inside' | 'outside' | 'both'

/**
 * Helper to normalize scale
 */
export function normalizeScale(scale: Vec3 | number | undefined): Vec3 {
  if (scale === undefined) return [1, 1, 1]
  if (typeof scale === 'number') return [scale, scale, scale]
  return scale
}

/**
 * Helper to normalize padding
 */
export function normalizePadding(padding: number | [number, number, number, number] | undefined): [number, number, number, number] {
  if (padding === undefined) return [0, 0, 0, 0]
  if (typeof padding === 'number') return [padding, padding, padding, padding]
  return padding
}

/**
 * Default colors
 */
export const XRColors = {
  primary: [0.2, 0.5, 1.0, 1.0] as Color,
  secondary: [0.4, 0.4, 0.4, 1.0] as Color,
  background: [0.1, 0.1, 0.1, 0.9] as Color,
  surface: [0.15, 0.15, 0.15, 0.95] as Color,
  text: [1.0, 1.0, 1.0, 1.0] as Color,
  textSecondary: [0.7, 0.7, 0.7, 1.0] as Color,
  border: [0.3, 0.3, 0.3, 1.0] as Color,
  hover: [0.3, 0.6, 1.0, 1.0] as Color,
  active: [0.1, 0.4, 0.9, 1.0] as Color,
  danger: [0.9, 0.2, 0.2, 1.0] as Color,
  success: [0.2, 0.8, 0.3, 1.0] as Color,
  warning: [0.9, 0.7, 0.1, 1.0] as Color,
}

/**
 * Default dimensions (in meters)
 */
export const XRDimensions = {
  buttonHeight: {
    sm: 0.04,
    md: 0.05,
    lg: 0.06,
  },
  fontSize: {
    xs: 0.012,
    sm: 0.016,
    md: 0.02,
    lg: 0.028,
    xl: 0.04,
  },
  spacing: {
    xs: 0.005,
    sm: 0.01,
    md: 0.02,
    lg: 0.04,
    xl: 0.08,
  },
  cornerRadius: {
    none: 0,
    sm: 0.005,
    md: 0.01,
    lg: 0.02,
    full: 999,
  },
}

import type { ReactNode } from 'react';
import type { Handle, Vec3, Quat, Color } from '../types';
/**
 * Base props shared by all XR components
 */
export interface XRBaseProps {
    position?: Vec3;
    rotation?: Quat;
    scale?: Vec3 | number;
    visible?: boolean;
    opacity?: number;
    children?: ReactNode;
    onMount?: (handle: Handle) => void;
    onUnmount?: () => void;
}
/**
 * Interactive component props
 */
export interface XRInteractiveProps extends XRBaseProps {
    disabled?: boolean;
    onHover?: () => void;
    onHoverEnd?: () => void;
    onSelect?: () => void;
    onSelectStart?: () => void;
    onSelectEnd?: () => void;
}
/**
 * Layout direction for stacks
 */
export type LayoutDirection = 'horizontal' | 'vertical' | 'depth';
/**
 * Alignment options
 */
export type Alignment = 'start' | 'center' | 'end' | 'stretch';
/**
 * Justify content options
 */
export type JustifyContent = 'start' | 'center' | 'end' | 'space-between' | 'space-around' | 'space-evenly';
/**
 * Panel background style
 */
export interface PanelStyle {
    backgroundColor?: Color;
    backgroundOpacity?: number;
    cornerRadius?: number;
    borderColor?: Color;
    borderWidth?: number;
    padding?: number | [number, number, number, number];
    shadow?: boolean;
    shadowColor?: Color;
    shadowOffset?: Vec3;
    shadowBlur?: number;
    blurBackground?: boolean;
    blurAmount?: number;
}
/**
 * Text style
 */
export interface TextStyle {
    fontSize?: number;
    fontFamily?: string;
    fontWeight?: 'normal' | 'bold' | '100' | '200' | '300' | '400' | '500' | '600' | '700' | '800' | '900';
    color?: Color;
    textAlign?: 'left' | 'center' | 'right';
    lineHeight?: number;
    letterSpacing?: number;
    maxWidth?: number;
    maxLines?: number;
    overflow?: 'visible' | 'clip' | 'ellipsis';
}
/**
 * Button variant
 */
export type ButtonVariant = 'primary' | 'secondary' | 'ghost' | 'danger';
/**
 * Button size
 */
export type ButtonSize = 'sm' | 'md' | 'lg';
/**
 * Slider orientation
 */
export type SliderOrientation = 'horizontal' | 'vertical';
/**
 * Scroll direction
 */
export type ScrollDirection = 'horizontal' | 'vertical' | 'both';
/**
 * Interaction type
 */
export type InteractionType = 'near' | 'ray' | 'gaze';
/**
 * HUD anchor position
 */
export type HUDAnchor = 'top-left' | 'top-center' | 'top-right' | 'center-left' | 'center' | 'center-right' | 'bottom-left' | 'bottom-center' | 'bottom-right';
/**
 * Video playback state
 */
export type VideoState = 'idle' | 'loading' | 'playing' | 'paused' | 'ended' | 'error';
/**
 * Cylinder mapping mode
 */
export type CylinderMapping = 'inside' | 'outside' | 'both';
/**
 * Helper to normalize scale
 */
export declare function normalizeScale(scale: Vec3 | number | undefined): Vec3;
/**
 * Helper to normalize padding
 */
export declare function normalizePadding(padding: number | [number, number, number, number] | undefined): [number, number, number, number];
/**
 * Default colors
 */
export declare const XRColors: {
    primary: Color;
    secondary: Color;
    background: Color;
    surface: Color;
    text: Color;
    textSecondary: Color;
    border: Color;
    hover: Color;
    active: Color;
    danger: Color;
    success: Color;
    warning: Color;
};
/**
 * Default dimensions (in meters)
 */
export declare const XRDimensions: {
    buttonHeight: {
        sm: number;
        md: number;
        lg: number;
    };
    fontSize: {
        xs: number;
        sm: number;
        md: number;
        lg: number;
        xl: number;
    };
    spacing: {
        xs: number;
        sm: number;
        md: number;
        lg: number;
        xl: number;
    };
    cornerRadius: {
        none: number;
        sm: number;
        md: number;
        lg: number;
        full: number;
    };
};
//# sourceMappingURL=types.d.ts.map
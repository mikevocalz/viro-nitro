import React from 'react';
import type { XRInteractiveProps, ButtonVariant, ButtonSize, TextStyle } from './types';
/**
 * XRButton Props
 */
export interface XRButtonProps extends XRInteractiveProps {
    label?: string;
    children?: string;
    variant?: ButtonVariant;
    size?: ButtonSize;
    icon?: string;
    iconPosition?: 'left' | 'right';
    fullWidth?: boolean;
    loading?: boolean;
    textStyle?: TextStyle;
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
export declare function XRButton({ position, rotation, scale, visible, opacity, onMount, onUnmount, disabled, onHover, onHoverEnd, onSelect, onSelectStart, onSelectEnd, label, children, variant, size, icon, iconPosition, fullWidth, loading, textStyle, }: XRButtonProps): null;
export declare namespace XRButton {
    var displayName: string;
}
/**
 * XRIconButton - Button with only an icon
 */
export interface XRIconButtonProps extends Omit<XRButtonProps, 'label' | 'children' | 'fullWidth'> {
    icon: string;
    'aria-label': string;
}
export declare function XRIconButton({ icon, size, ...props }: XRIconButtonProps): React.JSX.Element;
export declare namespace XRIconButton {
    var displayName: string;
}
//# sourceMappingURL=XRButton.d.ts.map
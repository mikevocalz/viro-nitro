import React from 'react';
import type { XRBaseProps, LayoutDirection, Alignment, JustifyContent } from './types';
/**
 * XRStack Props
 */
export interface XRStackProps extends XRBaseProps {
    direction?: LayoutDirection;
    spacing?: number;
    alignment?: Alignment;
    justifyContent?: JustifyContent;
    wrap?: boolean;
    maxWidth?: number;
    maxHeight?: number;
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
export declare function XRStack({ position, rotation, scale, visible, opacity, children, onMount, onUnmount, direction, spacing, alignment, justifyContent, wrap, maxWidth, maxHeight, }: XRStackProps): React.JSX.Element | null;
export declare namespace XRStack {
    var displayName: string;
}
/**
 * Convenience component for horizontal stack
 */
export declare function XRHStack(props: Omit<XRStackProps, 'direction'>): React.JSX.Element;
export declare namespace XRHStack {
    var displayName: string;
}
/**
 * Convenience component for vertical stack
 */
export declare function XRVStack(props: Omit<XRStackProps, 'direction'>): React.JSX.Element;
export declare namespace XRVStack {
    var displayName: string;
}
/**
 * Convenience component for depth stack
 */
export declare function XRZStack(props: Omit<XRStackProps, 'direction'>): React.JSX.Element;
export declare namespace XRZStack {
    var displayName: string;
}
//# sourceMappingURL=XRStack.d.ts.map
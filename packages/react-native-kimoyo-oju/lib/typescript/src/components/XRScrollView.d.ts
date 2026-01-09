import React from 'react';
import type { XRInteractiveProps, ScrollDirection } from './types';
/**
 * XRScrollView Props
 */
export interface XRScrollViewProps extends XRInteractiveProps {
    width?: number;
    height?: number;
    contentWidth?: number;
    contentHeight?: number;
    direction?: ScrollDirection;
    showScrollbar?: boolean;
    scrollbarWidth?: number;
    bounces?: boolean;
    pagingEnabled?: boolean;
    snapToInterval?: number;
    onScroll?: (offset: {
        x: number;
        y: number;
    }) => void;
    onScrollEnd?: (offset: {
        x: number;
        y: number;
    }) => void;
}
/**
 * XRScrollView - Scrollable container for content
 *
 * Supports horizontal, vertical, or both directions.
 * Works with near interaction (drag) and controllers (thumbstick).
 *
 * @example
 * ```tsx
 * <XRScrollView width={0.4} height={0.3} contentHeight={1.0}>
 *   <XRStack direction="vertical" spacing={0.02}>
 *     {items.map(item => <XRButton key={item.id}>{item.label}</XRButton>)}
 *   </XRStack>
 * </XRScrollView>
 * ```
 */
export declare function XRScrollView({ position, rotation, scale, visible, opacity, children, onMount, onUnmount, disabled, width, height, contentWidth, contentHeight, direction, showScrollbar, scrollbarWidth, bounces, pagingEnabled, snapToInterval, onScroll, onScrollEnd, }: XRScrollViewProps): React.JSX.Element | null;
export declare namespace XRScrollView {
    var displayName: string;
}
/**
 * ScrollViewer - Alias for XRScrollView with more desktop-like defaults
 */
export declare function ScrollViewer(props: XRScrollViewProps): React.JSX.Element;
export declare namespace ScrollViewer {
    var displayName: string;
}
//# sourceMappingURL=XRScrollView.d.ts.map
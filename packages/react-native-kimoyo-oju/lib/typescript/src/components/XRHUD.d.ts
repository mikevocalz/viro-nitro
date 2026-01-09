import React from 'react';
import type { XRBaseProps, HUDAnchor } from './types';
/**
 * XRHUD Props
 */
export interface XRHUDProps extends XRBaseProps {
    anchor?: HUDAnchor;
    distance?: number;
    offsetX?: number;
    offsetY?: number;
    followHead?: boolean;
    followSpeed?: number;
    fadeAtEdge?: boolean;
    alwaysVisible?: boolean;
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
export declare function XRHUD({ position, rotation, scale, visible, opacity, children, onMount, onUnmount, anchor, distance, offsetX, offsetY, followHead, followSpeed, fadeAtEdge, alwaysVisible, }: XRHUDProps): React.JSX.Element | null;
export declare namespace XRHUD {
    var displayName: string;
}
/**
 * XRTooltip - Contextual tooltip that appears near interaction point
 */
export interface XRTooltipProps extends XRBaseProps {
    text: string;
    show?: boolean;
    delay?: number;
    placement?: 'top' | 'bottom' | 'left' | 'right';
    maxWidth?: number;
}
export declare function XRTooltip({ position, rotation, scale, visible, children, onMount, onUnmount, text, show, delay, placement, maxWidth, }: XRTooltipProps): React.JSX.Element | null;
export declare namespace XRTooltip {
    var displayName: string;
}
/**
 * XRNotification - Toast-like notification
 */
export interface XRNotificationProps extends XRBaseProps {
    message: string;
    type?: 'info' | 'success' | 'warning' | 'error';
    duration?: number;
    onDismiss?: () => void;
}
export declare function XRNotification({ position, rotation, scale, visible, onMount, onUnmount, message, type, duration, onDismiss, }: XRNotificationProps): null;
export declare namespace XRNotification {
    var displayName: string;
}
//# sourceMappingURL=XRHUD.d.ts.map
import React from 'react';
import type { Handle } from '../types';
import type { XRBaseProps, PanelStyle } from './types';
/**
 * Context to pass panel handle to children
 */
export declare const XRPanelContext: React.Context<Handle | null>;
/**
 * Hook to get parent panel handle
 */
export declare function useParentPanel(): Handle | null;
/**
 * XRPanel Props
 */
export interface XRPanelProps extends XRBaseProps {
    width?: number;
    height?: number;
    depth?: number;
    style?: PanelStyle;
    curved?: boolean;
    curveRadius?: number;
    followGaze?: boolean;
    billboardMode?: 'none' | 'all' | 'y-only';
}
/**
 * XRPanel - A 3D panel container for XR UI elements
 *
 * Used as a base container for spatial UI. Can be flat or curved,
 * and optionally follows the user's gaze.
 *
 * @example
 * ```tsx
 * <XRPanel
 *   width={0.5}
 *   height={0.3}
 *   position={[0, 1.5, -1]}
 *   style={{ backgroundColor: [0.1, 0.1, 0.1, 0.9], cornerRadius: 0.02 }}
 * >
 *   <XRText>Hello World</XRText>
 * </XRPanel>
 * ```
 */
export declare function XRPanel({ position, rotation, scale, visible, opacity, children, onMount, onUnmount, width, height, depth, style, curved, curveRadius, followGaze, billboardMode, }: XRPanelProps): React.JSX.Element | null;
export declare namespace XRPanel {
    var displayName: string;
}
//# sourceMappingURL=XRPanel.d.ts.map
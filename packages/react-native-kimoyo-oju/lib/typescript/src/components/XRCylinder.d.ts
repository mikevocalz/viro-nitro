import React from 'react';
import type { Color } from '../types';
import type { XRBaseProps, CylinderMapping } from './types';
/**
 * XRCylinder Props
 */
export interface XRCylinderProps extends XRBaseProps {
    radius?: number;
    height?: number;
    radialSegments?: number;
    heightSegments?: number;
    openEnded?: boolean;
    thetaStart?: number;
    thetaLength?: number;
    color?: Color;
    texture?: string;
    mapping?: CylinderMapping;
    lightingEnabled?: boolean;
}
/**
 * XRCylinder - Cylindrical geometry component
 *
 * Perfect for curved displays, panoramic content, or curved UI panels.
 *
 * @example
 * ```tsx
 * // Curved display panel
 * <XRCylinder
 *   radius={2}
 *   height={1}
 *   thetaLength={Math.PI / 2}
 *   mapping="inside"
 *   texture="panorama.jpg"
 * />
 *
 * // Simple cylinder
 * <XRCylinder
 *   radius={0.1}
 *   height={0.5}
 *   color={[0.8, 0.2, 0.2, 1]}
 * />
 * ```
 */
export declare function XRCylinder({ position, rotation, scale, visible, opacity, children, onMount, onUnmount, radius, height, radialSegments, heightSegments, openEnded, thetaStart, thetaLength, color, texture, mapping, lightingEnabled, }: XRCylinderProps): null;
export declare namespace XRCylinder {
    var displayName: string;
}
/**
 * XRCurvedPanel - Convenience component for curved UI panels
 */
export interface XRCurvedPanelProps extends XRBaseProps {
    width?: number;
    height?: number;
    curveRadius?: number;
    curveAngle?: number;
    children?: React.ReactNode;
}
export declare function XRCurvedPanel({ width, height, curveRadius, curveAngle, children, ...props }: XRCurvedPanelProps): React.JSX.Element;
export declare namespace XRCurvedPanel {
    var displayName: string;
}
//# sourceMappingURL=XRCylinder.d.ts.map
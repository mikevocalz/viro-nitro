import React from 'react';
import type { XRBaseProps, TextStyle } from './types';
/**
 * XRText Props
 */
export interface XRTextProps extends XRBaseProps {
    children?: string;
    text?: string;
    style?: TextStyle;
}
/**
 * XRText - 3D text rendering component
 *
 * Renders text in 3D space with customizable styling.
 *
 * @example
 * ```tsx
 * <XRText
 *   text="Hello World"
 *   style={{ fontSize: 0.05, color: [1, 1, 1, 1] }}
 *   position={[0, 1.5, -1]}
 * />
 * ```
 */
export declare function XRText({ position, rotation, scale, visible, opacity, onMount, onUnmount, children, text, style, }: XRTextProps): null;
export declare namespace XRText {
    var displayName: string;
}
/**
 * XRHeading - Large heading text
 */
export declare function XRHeading(props: XRTextProps): React.JSX.Element;
export declare namespace XRHeading {
    var displayName: string;
}
/**
 * XRLabel - Small label text
 */
export declare function XRLabel(props: XRTextProps): React.JSX.Element;
export declare namespace XRLabel {
    var displayName: string;
}
//# sourceMappingURL=XRText.d.ts.map
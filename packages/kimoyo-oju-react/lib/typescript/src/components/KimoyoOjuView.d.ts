import React from 'react';
import { type ViewStyle } from 'react-native';
import { type XRMode } from 'react-native-kimoyo-oju';
export interface KimoyoOjuViewProps {
    style?: ViewStyle;
    mode?: XRMode;
    children?: React.ReactNode;
    onError?: (code: string, message: string) => void;
}
/**
 * KimoyoOjuView - Main container for KimoyoOju 3D/XR content.
 *
 * This component:
 * 1. Creates the native OpenGL view that hosts the renderer
 * 2. Provides KimoyoOjuContext to all children
 * 3. Handles lifecycle events
 *
 * Usage:
 * ```tsx
 * <KimoyoOjuView mode="flat" style={{ flex: 1 }}>
 *   <KimoyoOjuScene>
 *     <KimoyoOjuBox position={[0, 0, -5]} />
 *   </KimoyoOjuScene>
 * </KimoyoOjuView>
 * ```
 */
export declare function KimoyoOjuView({ style, mode, children, onError, }: KimoyoOjuViewProps): React.JSX.Element;
//# sourceMappingURL=KimoyoOjuView.d.ts.map
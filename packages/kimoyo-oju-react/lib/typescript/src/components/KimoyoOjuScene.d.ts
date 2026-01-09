import React from 'react';
import type { Handle } from 'react-native-kimoyo-oju';
interface KimoyoOjuSceneContextValue {
    sceneHandle: Handle | null;
}
export declare function useKimoyoOjuSceneContext(): KimoyoOjuSceneContextValue;
export interface KimoyoOjuSceneProps {
    children?: React.ReactNode;
}
/**
 * KimoyoOjuScene - Root container for 3D scene content.
 *
 * All KimoyoOjuNode, KimoyoOjuBox, etc. components must be descendants of KimoyoOjuScene.
 *
 * Usage:
 * ```tsx
 * <KimoyoOjuView>
 *   <KimoyoOjuScene>
 *     <KimoyoOjuBox position={[0, 0, -5]} />
 *     <KimoyoOjuLight type="ambient" />
 *   </KimoyoOjuScene>
 * </KimoyoOjuView>
 * ```
 */
export declare function KimoyoOjuScene({ children }: KimoyoOjuSceneProps): React.JSX.Element;
export {};
//# sourceMappingURL=KimoyoOjuScene.d.ts.map
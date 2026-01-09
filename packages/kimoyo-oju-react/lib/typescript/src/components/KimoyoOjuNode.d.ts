import React from 'react';
import type { Handle, Vec3, Quat } from 'react-native-kimoyo-oju';
export declare function useKimoyoOjuParentHandle(): Handle | null;
export interface KimoyoOjuNodeProps {
    position?: Vec3;
    rotation?: Quat;
    scale?: Vec3;
    visible?: boolean;
    children?: React.ReactNode;
}
/**
 * KimoyoOjuNode - Generic container node in the scene graph.
 *
 * Used for grouping and transforming child nodes.
 *
 * Usage:
 * ```tsx
 * <KimoyoOjuNode position={[0, 1, 0]}>
 *   <KimoyoOjuBox />
 *   <KimoyoOjuSphere position={[1, 0, 0]} />
 * </KimoyoOjuNode>
 * ```
 */
export declare function KimoyoOjuNode({ position, rotation, scale, visible, children, }: KimoyoOjuNodeProps): React.JSX.Element;
//# sourceMappingURL=KimoyoOjuNode.d.ts.map
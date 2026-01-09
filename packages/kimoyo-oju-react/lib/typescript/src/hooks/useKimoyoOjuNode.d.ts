import type { Handle, NodeType, Vec3, Quat } from 'react-native-kimoyo-oju';
export interface UseKimoyoOjuNodeOptions {
    nodeType: NodeType;
    parentHandle?: Handle | null;
    position?: Vec3;
    rotation?: Quat;
    scale?: Vec3;
    visible?: boolean;
}
export interface UseKimoyoOjuNodeResult {
    handle: Handle | null;
    setTransform: (position?: Vec3, rotation?: Quat, scale?: Vec3) => void;
    setVisibility: (visible: boolean) => void;
    destroy: () => void;
}
/**
 * Hook to manage a KimoyoOju scene node's lifecycle.
 *
 * Automatically creates the node on mount, updates on prop changes,
 * and destroys on unmount.
 */
export declare function useKimoyoOjuNode(options: UseKimoyoOjuNodeOptions): UseKimoyoOjuNodeResult;
//# sourceMappingURL=useKimoyoOjuNode.d.ts.map
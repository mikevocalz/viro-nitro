import type { Handle, Vec3, Quat, CommandBuffer } from '../types';
/**
 * Hook for managing an XR node's lifecycle and transforms
 */
export declare function useXRNode(nodeType: 'group' | 'mesh' | 'light' | 'camera' | 'text' | 'model', options?: {
    position?: Vec3;
    rotation?: Quat;
    scale?: Vec3 | number;
    visible?: boolean;
    parentHandle?: Handle | null;
    onMount?: (handle: Handle) => void;
    onUnmount?: () => void;
}): {
    handle: Handle | null;
    getHandle: () => Handle | null;
    isReady: boolean;
};
/**
 * Hook for submitting commands
 */
export declare function useSubmitCommands(): (commands: CommandBuffer["commands"]) => import("..").VoidResult;
//# sourceMappingURL=useXRNode.d.ts.map
import React from 'react';
import { CommandBufferBuilder, type Handle, type XRMode, type RendererState } from 'react-native-kimoyo-oju';
interface KimoyoOjuContextValue {
    allocateHandle: () => Handle | null;
    getCommandBuffer: () => CommandBufferBuilder;
    flushCommands: () => void;
    getState: () => RendererState;
    getMode: () => XRMode;
    enterXR: (mode: XRMode) => boolean;
    exitXR: () => boolean;
    dispose: (handle: Handle) => void;
}
export interface KimoyoOjuProviderProps {
    children: React.ReactNode;
    onError?: (code: string, message: string) => void;
}
/**
 * KimoyoOjuProvider - provides access to the KimoyoOju engine throughout the component tree.
 *
 * Manages command buffer batching and automatic flushing on React commit.
 */
export declare function KimoyoOjuProvider({ children, onError }: KimoyoOjuProviderProps): React.JSX.Element;
/**
 * Hook to access the KimoyoOju context
 */
export declare function useKimoyoOjuContext(): KimoyoOjuContextValue;
export {};
//# sourceMappingURL=KimoyoOjuContext.d.ts.map
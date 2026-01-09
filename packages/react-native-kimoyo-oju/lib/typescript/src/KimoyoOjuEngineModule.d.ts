import { type HybridObject } from 'react-native-nitro-modules';
import type { Handle, HandleResult, VoidResult, XRMode, RendererState, MemoryCounters, CommandBuffer } from './types';
/**
 * Native KimoyoOjuEngine interface (matches C++ HybridObject)
 */
interface NativeKimoyoOjuEngine extends HybridObject<{
    ios: 'c++';
    android: 'c++';
}> {
    submitJson(bufferJson: string): VoidResult;
    allocateHandle(): HandleResult;
    getState(): RendererState;
    getMode(): XRMode;
    getMemoryCounters(): MemoryCounters;
    pause(): VoidResult;
    resume(): VoidResult;
    enterXR(mode: XRMode): VoidResult;
    exitXR(): VoidResult;
    destroyHandle(handle: Handle): VoidResult;
}
/**
 * Get or create the KimoyoOjuEngine singleton
 */
export declare function getKimoyoOjuEngine(): NativeKimoyoOjuEngine;
/**
 * KimoyoOjuEngine API wrapper with additional safety and convenience
 */
export declare const KimoyoOjuEngine: {
    /**
     * Submit a command buffer to the render thread
     */
    submit(buffer: CommandBuffer): VoidResult;
    /**
     * Allocate a new handle for a node
     */
    allocateHandle(): HandleResult;
    /**
     * Get current renderer state
     */
    getState(): RendererState;
    /**
     * Get current XR mode
     */
    getMode(): XRMode;
    /**
     * Get memory counters for leak detection
     */
    getMemoryCounters(): MemoryCounters;
    /**
     * Pause rendering (call on app background)
     */
    pause(): VoidResult;
    /**
     * Resume rendering (call on app foreground)
     */
    resume(): VoidResult;
    /**
     * Enter XR mode
     */
    enterXR(mode: XRMode): VoidResult;
    /**
     * Exit XR mode
     */
    exitXR(): VoidResult;
    /**
     * Destroy a native resource handle
     */
    destroyHandle(handle: Handle): VoidResult;
};
export {};
//# sourceMappingURL=KimoyoOjuEngineModule.d.ts.map
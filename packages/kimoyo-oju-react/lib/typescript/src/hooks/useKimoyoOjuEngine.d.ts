import { type XRMode, type RendererState, type MemoryCounters } from 'react-native-kimoyo-oju';
/**
 * Hook to access KimoyoOjuEngine state and controls
 */
export declare function useKimoyoOjuEngine(): {
    getState: () => RendererState;
    getMode: () => XRMode;
    getMemoryCounters: () => MemoryCounters;
    pause: () => boolean;
    resume: () => boolean;
    enterXR: (mode: XRMode) => boolean;
    exitXR: () => boolean;
};
//# sourceMappingURL=useKimoyoOjuEngine.d.ts.map
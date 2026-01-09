"use strict";

import { useCallback } from 'react';
import { KimoyoOjuEngine } from 'react-native-kimoyo-oju';

/**
 * Hook to access KimoyoOjuEngine state and controls
 */
export function useKimoyoOjuEngine() {
  const getState = useCallback(() => {
    return KimoyoOjuEngine.getState();
  }, []);
  const getMode = useCallback(() => {
    return KimoyoOjuEngine.getMode();
  }, []);
  const getMemoryCounters = useCallback(() => {
    return KimoyoOjuEngine.getMemoryCounters();
  }, []);
  const pause = useCallback(() => {
    const result = KimoyoOjuEngine.pause();
    return result.ok;
  }, []);
  const resume = useCallback(() => {
    const result = KimoyoOjuEngine.resume();
    return result.ok;
  }, []);
  const enterXR = useCallback(mode => {
    const result = KimoyoOjuEngine.enterXR(mode);
    return result.ok;
  }, []);
  const exitXR = useCallback(() => {
    const result = KimoyoOjuEngine.exitXR();
    return result.ok;
  }, []);
  return {
    getState,
    getMode,
    getMemoryCounters,
    pause,
    resume,
    enterXR,
    exitXR
  };
}
//# sourceMappingURL=useKimoyoOjuEngine.js.map
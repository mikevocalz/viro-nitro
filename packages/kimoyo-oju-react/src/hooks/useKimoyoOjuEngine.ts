import { useCallback } from 'react'
import { KimoyoOjuEngine, type XRMode, type RendererState, type MemoryCounters } from 'react-native-kimoyo-oju'

/**
 * Hook to access KimoyoOjuEngine state and controls
 */
export function useKimoyoOjuEngine() {
  const getState = useCallback((): RendererState => {
    return KimoyoOjuEngine.getState()
  }, [])

  const getMode = useCallback((): XRMode => {
    return KimoyoOjuEngine.getMode()
  }, [])

  const getMemoryCounters = useCallback((): MemoryCounters => {
    return KimoyoOjuEngine.getMemoryCounters()
  }, [])

  const pause = useCallback((): boolean => {
    const result = KimoyoOjuEngine.pause()
    return result.ok
  }, [])

  const resume = useCallback((): boolean => {
    const result = KimoyoOjuEngine.resume()
    return result.ok
  }, [])

  const enterXR = useCallback((mode: XRMode): boolean => {
    const result = KimoyoOjuEngine.enterXR(mode)
    return result.ok
  }, [])

  const exitXR = useCallback((): boolean => {
    const result = KimoyoOjuEngine.exitXR()
    return result.ok
  }, [])

  return {
    getState,
    getMode,
    getMemoryCounters,
    pause,
    resume,
    enterXR,
    exitXR,
  }
}

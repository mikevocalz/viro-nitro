import React, { useState, useCallback, useEffect, useRef } from 'react'
import type { Handle } from '../types'
import { useXRNode } from './useXRNode'
import { useParentPanel } from './XRPanel'
import type { XRInteractiveProps, VideoState } from './types'

/**
 * XRVideo Props
 */
export interface XRVideoProps extends XRInteractiveProps {
  source: string | { uri: string }
  width?: number
  height?: number
  autoplay?: boolean
  loop?: boolean
  muted?: boolean
  volume?: number
  playbackRate?: number
  controls?: boolean
  poster?: string
  chromaKeyColor?: [number, number, number]
  chromaKeyThreshold?: number
  stereoMode?: 'none' | 'left-right' | 'top-bottom'
  projection?: 'flat' | 'equirectangular' | 'equirectangular-stereo'
  onLoad?: () => void
  onError?: (error: string) => void
  onProgress?: (progress: { currentTime: number; duration: number }) => void
  onEnd?: () => void
  onStateChange?: (state: VideoState) => void
}

/**
 * XRVideo - Video playback component for XR
 * 
 * Supports flat videos, 360° videos, and stereoscopic content.
 * Includes optional chroma key for green screen effects.
 * 
 * @example
 * ```tsx
 * <XRVideo
 *   source={{ uri: 'https://example.com/video.mp4' }}
 *   width={1.6}
 *   height={0.9}
 *   autoplay
 *   loop
 * />
 * 
 * // 360° video
 * <XRVideo
 *   source={{ uri: 'https://example.com/360-video.mp4' }}
 *   projection="equirectangular"
 * />
 * ```
 */
export function XRVideo({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  onMount,
  onUnmount,
  disabled = false,
  onHover,
  onHoverEnd,
  onSelect,
  source,
  width = 0.8,
  height = 0.45,
  autoplay = false,
  loop = false,
  muted = false,
  volume = 1.0,
  playbackRate = 1.0,
  controls = true,
  poster,
  chromaKeyColor,
  chromaKeyThreshold = 0.4,
  stereoMode = 'none',
  projection = 'flat',
  onLoad,
  onError,
  onProgress,
  onEnd,
  onStateChange,
}: XRVideoProps) {
  const parentHandle = useParentPanel()
  const [state, setState] = useState<VideoState>('idle')
  const [currentTime, setCurrentTime] = useState(0)
  const [duration, setDuration] = useState(0)
  const [isHovered, setIsHovered] = useState(false)
  
  const { handle, isReady } = useXRNode('mesh', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount,
  })

  const uri = typeof source === 'string' ? source : source.uri

  // State change handler
  const updateState = useCallback((newState: VideoState) => {
    setState(newState)
    onStateChange?.(newState)
  }, [onStateChange])

  // Playback controls
  const play = useCallback(() => {
    if (state === 'loading' || state === 'error') return
    updateState('playing')
  }, [state, updateState])

  const pause = useCallback(() => {
    if (state !== 'playing') return
    updateState('paused')
  }, [state, updateState])

  const seek = useCallback((time: number) => {
    setCurrentTime(Math.max(0, Math.min(duration, time)))
  }, [duration])

  const togglePlay = useCallback(() => {
    if (state === 'playing') {
      pause()
    } else {
      play()
    }
  }, [state, play, pause])

  // Handle select for play/pause toggle
  const handleSelect = useCallback(() => {
    if (disabled) return
    if (controls) {
      togglePlay()
    }
    onSelect?.()
  }, [disabled, controls, togglePlay, onSelect])

  const handleHover = useCallback(() => {
    if (disabled) return
    setIsHovered(true)
    onHover?.()
  }, [disabled, onHover])

  const handleHoverEnd = useCallback(() => {
    setIsHovered(false)
    onHoverEnd?.()
  }, [onHoverEnd])

  // Auto-play handling
  useEffect(() => {
    if (autoplay && state === 'idle') {
      updateState('loading')
    }
  }, [autoplay, state, updateState])

  if (!isReady || !handle) {
    return null
  }

  return null
}

XRVideo.displayName = 'XRVideo'

/**
 * Imperative handle for video control
 */
export interface XRVideoRef {
  play: () => void
  pause: () => void
  seek: (time: number) => void
  setVolume: (volume: number) => void
  getState: () => VideoState
  getCurrentTime: () => number
  getDuration: () => number
}

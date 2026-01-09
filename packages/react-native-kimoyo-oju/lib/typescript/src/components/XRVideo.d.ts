import type { XRInteractiveProps, VideoState } from './types';
/**
 * XRVideo Props
 */
export interface XRVideoProps extends XRInteractiveProps {
    source: string | {
        uri: string;
    };
    width?: number;
    height?: number;
    autoplay?: boolean;
    loop?: boolean;
    muted?: boolean;
    volume?: number;
    playbackRate?: number;
    controls?: boolean;
    poster?: string;
    chromaKeyColor?: [number, number, number];
    chromaKeyThreshold?: number;
    stereoMode?: 'none' | 'left-right' | 'top-bottom';
    projection?: 'flat' | 'equirectangular' | 'equirectangular-stereo';
    onLoad?: () => void;
    onError?: (error: string) => void;
    onProgress?: (progress: {
        currentTime: number;
        duration: number;
    }) => void;
    onEnd?: () => void;
    onStateChange?: (state: VideoState) => void;
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
export declare function XRVideo({ position, rotation, scale, visible, opacity, onMount, onUnmount, disabled, onHover, onHoverEnd, onSelect, source, width, height, autoplay, loop, muted, volume, playbackRate, controls, poster, chromaKeyColor, chromaKeyThreshold, stereoMode, projection, onLoad, onError, onProgress, onEnd, onStateChange, }: XRVideoProps): null;
export declare namespace XRVideo {
    var displayName: string;
}
/**
 * Imperative handle for video control
 */
export interface XRVideoRef {
    play: () => void;
    pause: () => void;
    seek: (time: number) => void;
    setVolume: (volume: number) => void;
    getState: () => VideoState;
    getCurrentTime: () => number;
    getDuration: () => number;
}
//# sourceMappingURL=XRVideo.d.ts.map
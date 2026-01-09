import React, { useCallback, useEffect } from 'react'
import { View, StyleSheet, type ViewStyle } from 'react-native'
import { Camera, useCameraDevice, useCameraPermission } from 'react-native-vision-camera'
import { KimoyoOjuNativeView, type XRMode } from 'react-native-kimoyo-oju'
import { KimoyoOjuProvider } from '../context/KimoyoOjuContext'

export interface KimoyoOjuViewProps {
  style?: ViewStyle
  mode?: XRMode
  passthrough?: 'vision' | 'native' | 'none'
  children?: React.ReactNode
  onError?: (code: string, message: string) => void
}

/**
 * KimoyoOjuView - Main container for KimoyoOju 3D/XR content.
 * 
 * This component:
 * 1. Creates the native OpenGL view that hosts the renderer
 * 2. Provides KimoyoOjuContext to all children
 * 3. Handles lifecycle events
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuView mode="flat" style={{ flex: 1 }}>
 *   <KimoyoOjuScene>
 *     <KimoyoOjuBox position={[0, 0, -5]} />
 *   </KimoyoOjuScene>
 * </KimoyoOjuView>
 * ```
 */
export function KimoyoOjuView({
  style,
  mode = 'flat',
  passthrough = 'vision',
  children,
  onError,
}: KimoyoOjuViewProps) {
  const handleError = useCallback((code: string, message: string) => {
    onError?.(code, message)
  }, [onError])
  const device = useCameraDevice('back')
  const { hasPermission, requestPermission } = useCameraPermission()

  useEffect(() => {
    if (mode !== 'immersive-mr' || passthrough !== 'vision') {
      return
    }

    if (!hasPermission) {
      requestPermission()
    }
  }, [hasPermission, mode, passthrough, requestPermission])

  const showCamera = passthrough === 'vision' && mode === 'immersive-mr' && hasPermission && device

  return (
    <KimoyoOjuProvider onError={handleError}>
      <View style={[styles.container, style]}>
        {showCamera ? (
          <Camera
            style={styles.camera}
            device={device}
            isActive={mode === 'immersive-mr'}
          />
        ) : null}
        <KimoyoOjuNativeView style={styles.glView} mode={mode} passthrough={passthrough} />
        <View pointerEvents="box-none" style={styles.overlay}>
          {children}
        </View>
      </View>
    </KimoyoOjuProvider>
  )
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  glView: {
    ...StyleSheet.absoluteFillObject,
  },
  camera: {
    ...StyleSheet.absoluteFillObject,
  },
  overlay: {
    ...StyleSheet.absoluteFillObject,
  },
})

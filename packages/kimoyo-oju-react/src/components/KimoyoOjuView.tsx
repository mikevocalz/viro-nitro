import React, { useCallback } from 'react'
import { View, StyleSheet, type ViewStyle } from 'react-native'
import { KimoyoOjuNativeView, type XRMode } from 'react-native-kimoyo-oju'
import { KimoyoOjuProvider } from '../context/KimoyoOjuContext'

export interface KimoyoOjuViewProps {
  style?: ViewStyle
  mode?: XRMode
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
  children,
  onError,
}: KimoyoOjuViewProps) {
  const handleError = useCallback((code: string, message: string) => {
    onError?.(code, message)
  }, [onError])

  return (
    <KimoyoOjuProvider onError={handleError}>
      <View style={[styles.container, style]}>
        <KimoyoOjuNativeView style={styles.glView} mode={mode} />
        <View style={styles.overlay}>
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
  overlay: {
    ...StyleSheet.absoluteFillObject,
  },
})

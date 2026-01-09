import React from 'react'
import { requireNativeComponent, type ViewStyle } from 'react-native'

interface NativeProps {
  style?: ViewStyle
  mode?: string
  passthrough?: string
}

const NativeView = requireNativeComponent<NativeProps>('KimoyoOjuView')

export interface KimoyoOjuNativeViewProps {
  style?: ViewStyle
  mode?: 'flat' | 'immersive-vr' | 'immersive-mr'
  passthrough?: 'vision' | 'native' | 'none'
}

export function KimoyoOjuNativeView({ style, mode = 'flat', passthrough = 'native' }: KimoyoOjuNativeViewProps) {
  return <NativeView style={style} mode={mode} passthrough={passthrough} />
}

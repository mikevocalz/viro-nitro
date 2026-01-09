import React from 'react'
import { requireNativeComponent, type ViewStyle } from 'react-native'

interface NativeProps {
  style?: ViewStyle
  mode?: string
}

const NativeView = requireNativeComponent<NativeProps>('KimoyoOjuView')

export interface KimoyoOjuNativeViewProps {
  style?: ViewStyle
  mode?: 'flat' | 'immersive-vr' | 'immersive-mr'
}

export function KimoyoOjuNativeView({ style, mode = 'flat' }: KimoyoOjuNativeViewProps) {
  return <NativeView style={style} mode={mode} />
}

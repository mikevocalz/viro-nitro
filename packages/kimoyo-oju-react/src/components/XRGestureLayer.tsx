import React, { useMemo } from 'react'
import { Gesture, GestureDetector } from 'react-native-gesture-handler'
import { runOnJS } from 'react-native-reanimated'

export interface XRGestureLayerProps {
  children?: React.ReactNode
  enabled?: boolean
  onTap?: (x: number, y: number) => void
  onPanStart?: (x: number, y: number) => void
  onPanUpdate?: (translationX: number, translationY: number) => void
  onPanEnd?: () => void
}

export function XRGestureLayer({
  children,
  enabled = true,
  onTap,
  onPanStart,
  onPanUpdate,
  onPanEnd,
}: XRGestureLayerProps) {
  const panGesture = useMemo(() => {
    return Gesture.Pan()
      .enabled(enabled)
      .onBegin((event) => {
        if (onPanStart) {
          runOnJS(onPanStart)(event.x, event.y)
        }
      })
      .onUpdate((event) => {
        if (onPanUpdate) {
          runOnJS(onPanUpdate)(event.translationX, event.translationY)
        }
      })
      .onEnd(() => {
        if (onPanEnd) {
          runOnJS(onPanEnd)()
        }
      })
  }, [enabled, onPanEnd, onPanStart, onPanUpdate])

  const tapGesture = useMemo(() => {
    return Gesture.Tap()
      .enabled(enabled)
      .onEnd((event) => {
        if (onTap) {
          runOnJS(onTap)(event.x, event.y)
        }
      })
  }, [enabled, onTap])

  const composedGesture = useMemo(() => {
    return Gesture.Simultaneous(panGesture, tapGesture)
  }, [panGesture, tapGesture])

  return <GestureDetector gesture={composedGesture}>{children}</GestureDetector>
}

XRGestureLayer.displayName = 'XRGestureLayer'

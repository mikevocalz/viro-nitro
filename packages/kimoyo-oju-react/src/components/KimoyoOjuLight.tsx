import React, { useEffect } from 'react'
import type { Vec3, Quat, Color, LightType } from 'react-native-kimoyo-oju'
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode'
import { useKimoyoOjuParentHandle } from './KimoyoOjuNode'
import { useKimoyoOjuContext } from '../context/KimoyoOjuContext'

export interface KimoyoOjuLightProps {
  type: LightType
  position?: Vec3
  rotation?: Quat
  color?: Color
  intensity?: number
  range?: number
  innerConeAngle?: number
  outerConeAngle?: number
}

/**
 * KimoyoOjuLight - Light source in the scene.
 */
export function KimoyoOjuLight({
  type,
  position,
  rotation,
  color = [1, 1, 1, 1],
  intensity = 1,
  range = 10,
  innerConeAngle = 0,
  outerConeAngle = 45,
}: KimoyoOjuLightProps) {
  const parentHandle = useKimoyoOjuParentHandle()
  const { getCommandBuffer, flushCommands } = useKimoyoOjuContext()
  
  const { handle } = useKimoyoOjuNode({
    nodeType: 'light',
    parentHandle,
    position,
    rotation,
  })

  useEffect(() => {
    if (handle) {
      const buffer = getCommandBuffer()
      buffer.setLight(handle, type, color, intensity, range, innerConeAngle, outerConeAngle)
      flushCommands()
    }
  }, [handle, type, color[0], color[1], color[2], color[3], intensity, range, innerConeAngle, outerConeAngle])

  return null
}

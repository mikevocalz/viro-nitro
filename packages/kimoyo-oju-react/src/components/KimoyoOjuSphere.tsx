import React, { useEffect } from 'react'
import type { Vec3, Quat, Color } from 'react-native-kimoyo-oju'
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode'
import { useKimoyoOjuParentHandle } from './KimoyoOjuNode'
import { useKimoyoOjuContext } from '../context/KimoyoOjuContext'

export interface KimoyoOjuSphereProps {
  position?: Vec3
  rotation?: Quat
  scale?: Vec3
  visible?: boolean
  radius?: number
  color?: Color
}

/**
 * KimoyoOjuSphere - 3D sphere primitive.
 */
export function KimoyoOjuSphere({
  position,
  rotation,
  scale,
  visible = true,
  radius = 0.5,
  color = [1, 1, 1, 1],
}: KimoyoOjuSphereProps) {
  const parentHandle = useKimoyoOjuParentHandle()
  const { getCommandBuffer, flushCommands } = useKimoyoOjuContext()
  
  const { handle } = useKimoyoOjuNode({
    nodeType: 'mesh',
    parentHandle,
    position,
    rotation,
    scale,
    visible,
  })

  useEffect(() => {
    if (handle) {
      const buffer = getCommandBuffer()
      buffer.setGeometry(handle, 'sphere', [radius * 2, radius * 2, radius * 2])
      buffer.setMaterial(handle, color, [1, 1, 1, 1], 32, null, null)
      flushCommands()
    }
  }, [handle, radius, color[0], color[1], color[2], color[3]])

  return null
}

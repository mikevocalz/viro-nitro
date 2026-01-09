import React, { useEffect } from 'react'
import type { Vec3, Quat } from 'react-native-kimoyo-oju'
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode'
import { useKimoyoOjuParentHandle } from './KimoyoOjuNode'
import { useKimoyoOjuContext } from '../context/KimoyoOjuContext'

export interface KimoyoOjuCameraProps {
  position?: Vec3
  rotation?: Quat
  fov?: number
  nearClip?: number
  farClip?: number
}

/**
 * KimoyoOjuCamera - Camera in the scene (for flat mode).
 * In XR mode, the camera is controlled by the headset.
 */
export function KimoyoOjuCamera({
  position,
  rotation,
  fov = 60,
  nearClip = 0.1,
  farClip = 1000,
}: KimoyoOjuCameraProps) {
  const parentHandle = useKimoyoOjuParentHandle()
  const { getCommandBuffer, flushCommands } = useKimoyoOjuContext()
  
  const { handle } = useKimoyoOjuNode({
    nodeType: 'camera',
    parentHandle,
    position,
    rotation,
  })

  useEffect(() => {
    if (handle) {
      const buffer = getCommandBuffer()
      buffer.setCamera(handle, fov, nearClip, farClip)
      flushCommands()
    }
  }, [handle, fov, nearClip, farClip])

  return null
}

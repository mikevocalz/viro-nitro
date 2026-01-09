import React, { createContext, useContext } from 'react'
import type { Handle, Vec3, Quat } from 'react-native-kimoyo-oju'
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode'
import { useKimoyoOjuSceneContext } from './KimoyoOjuScene'

interface KimoyoOjuNodeContextValue {
  parentHandle: Handle | null
}

const KimoyoOjuNodeContext = createContext<KimoyoOjuNodeContextValue | null>(null)

export function useKimoyoOjuParentHandle(): Handle | null {
  const nodeContext = useContext(KimoyoOjuNodeContext)
  const sceneContext = useKimoyoOjuSceneContext()
  return nodeContext?.parentHandle ?? sceneContext.sceneHandle
}

export interface KimoyoOjuNodeProps {
  position?: Vec3
  rotation?: Quat
  scale?: Vec3
  visible?: boolean
  children?: React.ReactNode
}

/**
 * KimoyoOjuNode - Generic container node in the scene graph.
 * 
 * Used for grouping and transforming child nodes.
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuNode position={[0, 1, 0]}>
 *   <KimoyoOjuBox />
 *   <KimoyoOjuSphere position={[1, 0, 0]} />
 * </KimoyoOjuNode>
 * ```
 */
export function KimoyoOjuNode({
  position,
  rotation,
  scale,
  visible = true,
  children,
}: KimoyoOjuNodeProps) {
  const parentHandle = useKimoyoOjuParentHandle()
  
  const { handle } = useKimoyoOjuNode({
    nodeType: 'group',
    parentHandle,
    position,
    rotation,
    scale,
    visible,
  })

  return (
    <KimoyoOjuNodeContext.Provider value={{ parentHandle: handle }}>
      {children}
    </KimoyoOjuNodeContext.Provider>
  )
}

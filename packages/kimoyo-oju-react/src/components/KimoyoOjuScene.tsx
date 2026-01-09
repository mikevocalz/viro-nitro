import React, { createContext, useContext } from 'react'
import type { Handle } from 'react-native-kimoyo-oju'
import { useKimoyoOjuNode } from '../hooks/useKimoyoOjuNode'

interface KimoyoOjuSceneContextValue {
  sceneHandle: Handle | null
}

const KimoyoOjuSceneContext = createContext<KimoyoOjuSceneContextValue | null>(null)

export function useKimoyoOjuSceneContext(): KimoyoOjuSceneContextValue {
  const context = useContext(KimoyoOjuSceneContext)
  if (!context) {
    throw new Error('KimoyoOju components must be used within a KimoyoOjuScene')
  }
  return context
}

export interface KimoyoOjuSceneProps {
  children?: React.ReactNode
}

/**
 * KimoyoOjuScene - Root container for 3D scene content.
 * 
 * All KimoyoOjuNode, KimoyoOjuBox, etc. components must be descendants of KimoyoOjuScene.
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuView>
 *   <KimoyoOjuScene>
 *     <KimoyoOjuBox position={[0, 0, -5]} />
 *     <KimoyoOjuLight type="ambient" />
 *   </KimoyoOjuScene>
 * </KimoyoOjuView>
 * ```
 */
export function KimoyoOjuScene({ children }: KimoyoOjuSceneProps) {
  const { handle } = useKimoyoOjuNode({
    nodeType: 'group',
  })

  return (
    <KimoyoOjuSceneContext.Provider value={{ sceneHandle: handle }}>
      {children}
    </KimoyoOjuSceneContext.Provider>
  )
}

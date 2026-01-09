import { useRef, useEffect, useCallback } from 'react'
import type { Handle, Vec3, Quat, CommandBuffer } from '../types'
import { KimoyoOjuEngine } from '../KimoyoOjuEngineModule'
import { normalizeScale } from './types'

let txIdCounter = 0

function getNextTxId(): number {
  return ++txIdCounter
}

/**
 * Hook for managing an XR node's lifecycle and transforms
 */
export function useXRNode(
  nodeType: 'group' | 'mesh' | 'light' | 'camera' | 'text' | 'model',
  options: {
    position?: Vec3
    rotation?: Quat
    scale?: Vec3 | number
    visible?: boolean
    parentHandle?: Handle | null
    onMount?: (handle: Handle) => void
    onUnmount?: () => void
  } = {}
) {
  const handleRef = useRef<Handle | null>(null)
  const mountedRef = useRef(false)

  const {
    position = [0, 0, 0],
    rotation = [0, 0, 0, 1],
    scale = [1, 1, 1],
    visible = true,
    parentHandle = null,
    onMount,
    onUnmount,
  } = options

  const normalizedScale = normalizeScale(scale)

  // Allocate handle and create node on mount
  useEffect(() => {
    const result = KimoyoOjuEngine.allocateHandle()
    if (!result.ok) {
      console.error('Failed to allocate handle:', result.message)
      return
    }

    const handle = result.handle
    if (!handle) {
      console.error('Failed to get handle from result')
      return
    }
    
    handleRef.current = handle
    mountedRef.current = true

    const buffer: CommandBuffer = {
      version: 1,
      txId: getNextTxId(),
      timestamp: Date.now(),
      commands: [
        {
          type: 'CREATE_NODE',
          handle,
          nodeType,
          parentHandle,
        },
        {
          type: 'SET_TRANSFORM',
          handle,
          position,
          rotation,
          scale: normalizedScale,
        },
        {
          type: 'SET_VISIBILITY',
          handle,
          visible,
        },
      ],
    }

    KimoyoOjuEngine.submit(buffer)
    onMount?.(handle)

    return () => {
      if (handleRef.current && mountedRef.current) {
        const destroyBuffer: CommandBuffer = {
          version: 1,
          txId: getNextTxId(),
          timestamp: Date.now(),
          commands: [
            {
              type: 'DESTROY_NODE',
              handle: handleRef.current,
            },
          ],
        }
        KimoyoOjuEngine.submit(destroyBuffer)
        KimoyoOjuEngine.destroyHandle(handleRef.current)
        mountedRef.current = false
        onUnmount?.()
      }
    }
  }, []) // Only run on mount/unmount

  // Update transform when props change
  useEffect(() => {
    if (!handleRef.current || !mountedRef.current) return

    const buffer: CommandBuffer = {
      version: 1,
      txId: getNextTxId(),
      timestamp: Date.now(),
      commands: [
        {
          type: 'SET_TRANSFORM',
          handle: handleRef.current,
          position,
          rotation,
          scale: normalizedScale,
        },
      ],
    }

    KimoyoOjuEngine.submit(buffer)
  }, [position[0], position[1], position[2], rotation[0], rotation[1], rotation[2], rotation[3], normalizedScale[0], normalizedScale[1], normalizedScale[2]])

  // Update visibility when prop changes
  useEffect(() => {
    if (!handleRef.current || !mountedRef.current) return

    const buffer: CommandBuffer = {
      version: 1,
      txId: getNextTxId(),
      timestamp: Date.now(),
      commands: [
        {
          type: 'SET_VISIBILITY',
          handle: handleRef.current,
          visible,
        },
      ],
    }

    KimoyoOjuEngine.submit(buffer)
  }, [visible])

  const getHandle = useCallback(() => handleRef.current, [])

  return {
    handle: handleRef.current,
    getHandle,
    isReady: mountedRef.current && handleRef.current !== null,
  }
}

/**
 * Hook for submitting commands
 */
export function useSubmitCommands() {
  return useCallback((commands: CommandBuffer['commands']) => {
    const buffer: CommandBuffer = {
      version: 1,
      txId: getNextTxId(),
      timestamp: Date.now(),
      commands,
    }
    return KimoyoOjuEngine.submit(buffer)
  }, [])
}

/**
 * Opaque handle to a native resource with generation counter for safety
 */
export interface Handle {
  id: number
  gen: number
}

/**
 * Void result for operations that don't return a value
 */
export interface VoidResult {
  ok: boolean
  code: string
  message: string
}

/**
 * Result with handle value
 */
export interface HandleResult {
  ok: boolean
  code: string
  message: string
  handle: Handle | null
}

/**
 * XR rendering mode
 */
export type XRMode = 'flat' | 'immersive-vr' | 'immersive-mr'

/**
 * Renderer state
 */
export type RendererState = 'created' | 'running' | 'paused' | 'surface_lost' | 'destroyed'

/**
 * Node types in the scene graph
 */
export type NodeType = 'group' | 'mesh' | 'light' | 'camera' | 'text' | 'model'

/**
 * Light types
 */
export type LightType = 'ambient' | 'directional' | 'point' | 'spot'

/**
 * Engine events
 */
export type EngineEvent =
  | 'surface_created'
  | 'surface_destroyed'
  | 'xr_session_started'
  | 'xr_session_ended'
  | 'xr_session_lost'
  | 'error'

/**
 * Memory counter snapshot for leak detection
 */
export interface MemoryCounters {
  liveNodes: number
  liveTextures: number
  liveBuffers: number
  liveSwapchains: number
  liveShaders: number
  gpuMemoryBytes: number
  cpuMemoryBytes: number
}

/**
 * 3D Vector
 */
export type Vec3 = [number, number, number]

/**
 * Quaternion (x, y, z, w)
 */
export type Quat = [number, number, number, number]

/**
 * RGBA Color
 */
export type Color = [number, number, number, number]

/**
 * Command buffer containing batched scene updates
 */
export interface CommandBuffer {
  version: 1
  txId: number
  timestamp: number
  commands: Command[]
}

/**
 * All possible commands
 */
export type Command =
  | CreateNodeCommand
  | DestroyNodeCommand
  | SetTransformCommand
  | SetVisibilityCommand
  | ReparentCommand
  | SetMaterialCommand
  | SetLightCommand
  | SetCameraCommand
  | LoadAssetCommand
  | SetTextCommand
  | SetGeometryCommand

export interface CreateNodeCommand {
  type: 'CREATE_NODE'
  handle: Handle
  nodeType: NodeType
  parentHandle: Handle | null
}

export interface DestroyNodeCommand {
  type: 'DESTROY_NODE'
  handle: Handle
}

export interface SetTransformCommand {
  type: 'SET_TRANSFORM'
  handle: Handle
  position: Vec3
  rotation: Quat
  scale: Vec3
}

export interface SetVisibilityCommand {
  type: 'SET_VISIBILITY'
  handle: Handle
  visible: boolean
}

export interface ReparentCommand {
  type: 'REPARENT'
  handle: Handle
  newParentHandle: Handle | null
}

export interface SetMaterialCommand {
  type: 'SET_MATERIAL'
  handle: Handle
  diffuseColor: Color
  specularColor: Color
  shininess: number
  diffuseTexture: Handle | null
  normalTexture: Handle | null
}

export interface SetLightCommand {
  type: 'SET_LIGHT'
  handle: Handle
  lightType: LightType
  color: Color
  intensity: number
  range: number
  innerConeAngle: number
  outerConeAngle: number
}

export interface SetCameraCommand {
  type: 'SET_CAMERA'
  handle: Handle
  fov: number
  nearClip: number
  farClip: number
}

export interface LoadAssetCommand {
  type: 'LOAD_ASSET'
  handle: Handle
  uri: string
  assetType: 'model' | 'texture' | 'audio'
}

export interface SetTextCommand {
  type: 'SET_TEXT'
  handle: Handle
  text: string
  fontSize: number
  color: Color
  fontFamily: string
}

export interface SetGeometryCommand {
  type: 'SET_GEOMETRY'
  handle: Handle
  geometryType: 'box' | 'sphere' | 'plane' | 'cylinder'
  dimensions: Vec3
}

/**
 * Helper to create a successful void result
 */
export function successVoid(): VoidResult {
  return { ok: true, code: '', message: '' }
}

/**
 * Helper to create a failure void result
 */
export function failureVoid(code: string, message: string): VoidResult {
  return { ok: false, code, message }
}

/**
 * Helper to check if handle is valid
 */
export function isValidHandle(handle: Handle | null | undefined): handle is Handle {
  return handle != null && handle.id > 0 && handle.gen > 0
}

/**
 * Create an invalid/null handle
 */
export function nullHandle(): Handle {
  return { id: 0, gen: 0 }
}

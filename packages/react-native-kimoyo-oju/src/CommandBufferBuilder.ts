import type {
  Handle,
  Command,
  CommandBuffer,
  NodeType,
  LightType,
  Vec3,
  Quat,
  Color,
} from './types'

let txIdCounter = 0

/**
 * Builder for creating command buffers with batched scene updates.
 * 
 * Usage:
 * ```ts
 * const buffer = new CommandBufferBuilder()
 *   .createNode(handle, 'mesh', parentHandle)
 *   .setTransform(handle, [0, 1, 0], [0, 0, 0, 1], [1, 1, 1])
 *   .setGeometry(handle, 'box', [1, 1, 1])
 *   .build();
 * 
 * KimoyoOjuEngine.submit(buffer);
 * ```
 */
export class CommandBufferBuilder {
  private commands: Command[] = []
  private txId: number

  constructor() {
    this.txId = ++txIdCounter
  }

  /**
   * Create a new node in the scene graph
   */
  createNode(handle: Handle, nodeType: NodeType, parentHandle: Handle | null = null): this {
    this.commands.push({
      type: 'CREATE_NODE',
      handle,
      nodeType,
      parentHandle,
    })
    return this
  }

  /**
   * Destroy a node and remove it from the scene graph
   */
  destroyNode(handle: Handle): this {
    this.commands.push({
      type: 'DESTROY_NODE',
      handle,
    })
    return this
  }

  /**
   * Set the transform (position, rotation, scale) of a node
   */
  setTransform(
    handle: Handle,
    position: Vec3 = [0, 0, 0],
    rotation: Quat = [0, 0, 0, 1],
    scale: Vec3 = [1, 1, 1]
  ): this {
    this.commands.push({
      type: 'SET_TRANSFORM',
      handle,
      position,
      rotation,
      scale,
    })
    return this
  }

  /**
   * Set the visibility of a node
   */
  setVisibility(handle: Handle, visible: boolean): this {
    this.commands.push({
      type: 'SET_VISIBILITY',
      handle,
      visible,
    })
    return this
  }

  /**
   * Reparent a node to a new parent
   */
  reparent(handle: Handle, newParentHandle: Handle | null): this {
    this.commands.push({
      type: 'REPARENT',
      handle,
      newParentHandle,
    })
    return this
  }

  /**
   * Set material properties for a mesh node
   */
  setMaterial(
    handle: Handle,
    diffuseColor: Color = [1, 1, 1, 1],
    specularColor: Color = [1, 1, 1, 1],
    shininess: number = 32,
    diffuseTexture: Handle | null = null,
    normalTexture: Handle | null = null
  ): this {
    this.commands.push({
      type: 'SET_MATERIAL',
      handle,
      diffuseColor,
      specularColor,
      shininess,
      diffuseTexture,
      normalTexture,
    })
    return this
  }

  /**
   * Set light properties for a light node
   */
  setLight(
    handle: Handle,
    lightType: LightType,
    color: Color = [1, 1, 1, 1],
    intensity: number = 1,
    range: number = 10,
    innerConeAngle: number = 0,
    outerConeAngle: number = 45
  ): this {
    this.commands.push({
      type: 'SET_LIGHT',
      handle,
      lightType,
      color,
      intensity,
      range,
      innerConeAngle,
      outerConeAngle,
    })
    return this
  }

  /**
   * Set camera properties for a camera node
   */
  setCamera(
    handle: Handle,
    fov: number = 60,
    nearClip: number = 0.1,
    farClip: number = 1000
  ): this {
    this.commands.push({
      type: 'SET_CAMERA',
      handle,
      fov,
      nearClip,
      farClip,
    })
    return this
  }

  /**
   * Load an asset (model, texture, audio)
   */
  loadAsset(
    handle: Handle,
    uri: string,
    assetType: 'model' | 'texture' | 'audio'
  ): this {
    this.commands.push({
      type: 'LOAD_ASSET',
      handle,
      uri,
      assetType,
    })
    return this
  }

  /**
   * Set text properties for a text node
   */
  setText(
    handle: Handle,
    text: string,
    fontSize: number = 16,
    color: Color = [1, 1, 1, 1],
    fontFamily: string = 'system'
  ): this {
    this.commands.push({
      type: 'SET_TEXT',
      handle,
      text,
      fontSize,
      color,
      fontFamily,
    })
    return this
  }

  /**
   * Set geometry for a mesh node
   */
  setGeometry(
    handle: Handle,
    geometryType: 'box' | 'sphere' | 'plane' | 'cylinder',
    dimensions: Vec3 = [1, 1, 1]
  ): this {
    this.commands.push({
      type: 'SET_GEOMETRY',
      handle,
      geometryType,
      dimensions,
    })
    return this
  }

  /**
   * Get the number of commands in the buffer
   */
  size(): number {
    return this.commands.length
  }

  /**
   * Check if the buffer is empty
   */
  isEmpty(): boolean {
    return this.commands.length === 0
  }

  /**
   * Clear all commands from the buffer
   */
  clear(): this {
    this.commands = []
    return this
  }

  /**
   * Build the final command buffer
   */
  build(): CommandBuffer {
    return {
      version: 1,
      txId: this.txId,
      timestamp: Date.now(),
      commands: [...this.commands],
    }
  }

  /**
   * Build and clear the buffer (for reuse)
   */
  flush(): CommandBuffer {
    const buffer = this.build()
    this.clear()
    this.txId = ++txIdCounter
    return buffer
  }
}

/**
 * Create a new command buffer builder
 */
export function createCommandBuffer(): CommandBufferBuilder {
  return new CommandBufferBuilder()
}

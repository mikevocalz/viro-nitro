"use strict";

let txIdCounter = 0;

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
  commands = [];
  constructor() {
    this.txId = ++txIdCounter;
  }

  /**
   * Create a new node in the scene graph
   */
  createNode(handle, nodeType, parentHandle = null) {
    this.commands.push({
      type: 'CREATE_NODE',
      handle,
      nodeType,
      parentHandle
    });
    return this;
  }

  /**
   * Destroy a node and remove it from the scene graph
   */
  destroyNode(handle) {
    this.commands.push({
      type: 'DESTROY_NODE',
      handle
    });
    return this;
  }

  /**
   * Set the transform (position, rotation, scale) of a node
   */
  setTransform(handle, position = [0, 0, 0], rotation = [0, 0, 0, 1], scale = [1, 1, 1]) {
    this.commands.push({
      type: 'SET_TRANSFORM',
      handle,
      position,
      rotation,
      scale
    });
    return this;
  }

  /**
   * Set the visibility of a node
   */
  setVisibility(handle, visible) {
    this.commands.push({
      type: 'SET_VISIBILITY',
      handle,
      visible
    });
    return this;
  }

  /**
   * Reparent a node to a new parent
   */
  reparent(handle, newParentHandle) {
    this.commands.push({
      type: 'REPARENT',
      handle,
      newParentHandle
    });
    return this;
  }

  /**
   * Set material properties for a mesh node
   */
  setMaterial(handle, diffuseColor = [1, 1, 1, 1], specularColor = [1, 1, 1, 1], shininess = 32, diffuseTexture = null, normalTexture = null) {
    this.commands.push({
      type: 'SET_MATERIAL',
      handle,
      diffuseColor,
      specularColor,
      shininess,
      diffuseTexture,
      normalTexture
    });
    return this;
  }

  /**
   * Set light properties for a light node
   */
  setLight(handle, lightType, color = [1, 1, 1, 1], intensity = 1, range = 10, innerConeAngle = 0, outerConeAngle = 45) {
    this.commands.push({
      type: 'SET_LIGHT',
      handle,
      lightType,
      color,
      intensity,
      range,
      innerConeAngle,
      outerConeAngle
    });
    return this;
  }

  /**
   * Set camera properties for a camera node
   */
  setCamera(handle, fov = 60, nearClip = 0.1, farClip = 1000) {
    this.commands.push({
      type: 'SET_CAMERA',
      handle,
      fov,
      nearClip,
      farClip
    });
    return this;
  }

  /**
   * Load an asset (model, texture, audio)
   */
  loadAsset(handle, uri, assetType) {
    this.commands.push({
      type: 'LOAD_ASSET',
      handle,
      uri,
      assetType
    });
    return this;
  }

  /**
   * Set text properties for a text node
   */
  setText(handle, text, fontSize = 16, color = [1, 1, 1, 1], fontFamily = 'system') {
    this.commands.push({
      type: 'SET_TEXT',
      handle,
      text,
      fontSize,
      color,
      fontFamily
    });
    return this;
  }

  /**
   * Set geometry for a mesh node
   */
  setGeometry(handle, geometryType, dimensions = [1, 1, 1]) {
    this.commands.push({
      type: 'SET_GEOMETRY',
      handle,
      geometryType,
      dimensions
    });
    return this;
  }

  /**
   * Get the number of commands in the buffer
   */
  size() {
    return this.commands.length;
  }

  /**
   * Check if the buffer is empty
   */
  isEmpty() {
    return this.commands.length === 0;
  }

  /**
   * Clear all commands from the buffer
   */
  clear() {
    this.commands = [];
    return this;
  }

  /**
   * Build the final command buffer
   */
  build() {
    return {
      version: 1,
      txId: this.txId,
      timestamp: Date.now(),
      commands: [...this.commands]
    };
  }

  /**
   * Build and clear the buffer (for reuse)
   */
  flush() {
    const buffer = this.build();
    this.clear();
    this.txId = ++txIdCounter;
    return buffer;
  }
}

/**
 * Create a new command buffer builder
 */
export function createCommandBuffer() {
  return new CommandBufferBuilder();
}
//# sourceMappingURL=CommandBufferBuilder.js.map
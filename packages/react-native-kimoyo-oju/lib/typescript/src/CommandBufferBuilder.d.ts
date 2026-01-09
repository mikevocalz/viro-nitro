import type { Handle, CommandBuffer, NodeType, LightType, Vec3, Quat, Color } from './types';
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
export declare class CommandBufferBuilder {
    private commands;
    private txId;
    constructor();
    /**
     * Create a new node in the scene graph
     */
    createNode(handle: Handle, nodeType: NodeType, parentHandle?: Handle | null): this;
    /**
     * Destroy a node and remove it from the scene graph
     */
    destroyNode(handle: Handle): this;
    /**
     * Set the transform (position, rotation, scale) of a node
     */
    setTransform(handle: Handle, position?: Vec3, rotation?: Quat, scale?: Vec3): this;
    /**
     * Set the visibility of a node
     */
    setVisibility(handle: Handle, visible: boolean): this;
    /**
     * Reparent a node to a new parent
     */
    reparent(handle: Handle, newParentHandle: Handle | null): this;
    /**
     * Set material properties for a mesh node
     */
    setMaterial(handle: Handle, diffuseColor?: Color, specularColor?: Color, shininess?: number, diffuseTexture?: Handle | null, normalTexture?: Handle | null): this;
    /**
     * Set light properties for a light node
     */
    setLight(handle: Handle, lightType: LightType, color?: Color, intensity?: number, range?: number, innerConeAngle?: number, outerConeAngle?: number): this;
    /**
     * Set camera properties for a camera node
     */
    setCamera(handle: Handle, fov?: number, nearClip?: number, farClip?: number): this;
    /**
     * Load an asset (model, texture, audio)
     */
    loadAsset(handle: Handle, uri: string, assetType: 'model' | 'texture' | 'audio'): this;
    /**
     * Set text properties for a text node
     */
    setText(handle: Handle, text: string, fontSize?: number, color?: Color, fontFamily?: string): this;
    /**
     * Set geometry for a mesh node
     */
    setGeometry(handle: Handle, geometryType: 'box' | 'sphere' | 'plane' | 'cylinder', dimensions?: Vec3): this;
    /**
     * Get the number of commands in the buffer
     */
    size(): number;
    /**
     * Check if the buffer is empty
     */
    isEmpty(): boolean;
    /**
     * Clear all commands from the buffer
     */
    clear(): this;
    /**
     * Build the final command buffer
     */
    build(): CommandBuffer;
    /**
     * Build and clear the buffer (for reuse)
     */
    flush(): CommandBuffer;
}
/**
 * Create a new command buffer builder
 */
export declare function createCommandBuffer(): CommandBufferBuilder;
//# sourceMappingURL=CommandBufferBuilder.d.ts.map
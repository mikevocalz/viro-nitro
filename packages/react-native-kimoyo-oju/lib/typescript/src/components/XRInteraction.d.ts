import React from 'react';
import type { Vec3 } from '../types';
import type { XRBaseProps, InteractionType } from './types';
/**
 * Interaction context for nested components
 */
export interface InteractionState {
    isHovered: boolean;
    isSelected: boolean;
    interactionType: InteractionType | null;
    hitPoint: Vec3 | null;
    hitNormal: Vec3 | null;
}
export declare function useInteraction(): InteractionState;
/**
 * XRNearInteraction Props
 */
export interface XRNearInteractionProps extends XRBaseProps {
    enabled?: boolean;
    hoverDistance?: number;
    selectDistance?: number;
    onNearEnter?: (hand: 'left' | 'right') => void;
    onNearExit?: (hand: 'left' | 'right') => void;
    onNearSelect?: (hand: 'left' | 'right', point: Vec3) => void;
    onNearSelectEnd?: (hand: 'left' | 'right') => void;
    hapticOnHover?: boolean;
    hapticOnSelect?: boolean;
}
/**
 * XRNearInteraction - Near-field hand interaction component
 *
 * Enables direct touch/poke interactions with hands.
 * Wrap interactive elements to enable near interaction.
 *
 * @example
 * ```tsx
 * <XRNearInteraction onNearSelect={(hand) => console.log(`${hand} hand selected`)}>
 *   <XRButton>Touch Me</XRButton>
 * </XRNearInteraction>
 * ```
 */
export declare function XRNearInteraction({ position, rotation, scale, visible, children, onMount, onUnmount, enabled, hoverDistance, selectDistance, onNearEnter, onNearExit, onNearSelect, onNearSelectEnd, hapticOnHover, hapticOnSelect, }: XRNearInteractionProps): React.JSX.Element | null;
export declare namespace XRNearInteraction {
    var displayName: string;
}
/**
 * XRRayInteraction Props
 */
export interface XRRayInteractionProps extends XRBaseProps {
    enabled?: boolean;
    maxDistance?: number;
    showRay?: boolean;
    rayColor?: [number, number, number, number];
    rayWidth?: number;
    cursorSize?: number;
    cursorColor?: [number, number, number, number];
    onRayEnter?: (hand: 'left' | 'right') => void;
    onRayExit?: (hand: 'left' | 'right') => void;
    onRaySelect?: (hand: 'left' | 'right', point: Vec3, normal: Vec3) => void;
    onRaySelectEnd?: (hand: 'left' | 'right') => void;
    onRayMove?: (hand: 'left' | 'right', point: Vec3) => void;
}
/**
 * XRRayInteraction - Ray-based pointing interaction
 *
 * Enables controller/hand ray interactions for distant objects.
 *
 * @example
 * ```tsx
 * <XRRayInteraction
 *   showRay
 *   onRaySelect={(hand, point) => console.log(`Selected at ${point}`)}
 * >
 *   <XRPanel position={[0, 1.5, -2]}>
 *     <XRButton>Click Me</XRButton>
 *   </XRPanel>
 * </XRRayInteraction>
 * ```
 */
export declare function XRRayInteraction({ position, rotation, scale, visible, children, onMount, onUnmount, enabled, maxDistance, showRay, rayColor, rayWidth, cursorSize, cursorColor, onRayEnter, onRayExit, onRaySelect, onRaySelectEnd, onRayMove, }: XRRayInteractionProps): React.JSX.Element | null;
export declare namespace XRRayInteraction {
    var displayName: string;
}
//# sourceMappingURL=XRInteraction.d.ts.map
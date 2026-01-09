import React from 'react';
import type { XRBaseProps, Alignment, JustifyContent } from './types';
/**
 * XRGrid Props
 */
export interface XRGridProps extends XRBaseProps {
    columns?: number;
    rows?: number;
    cellWidth?: number;
    cellHeight?: number;
    spacingX?: number;
    spacingY?: number;
    alignItems?: Alignment;
    justifyItems?: JustifyContent;
}
/**
 * XRGrid - Layout children in a 2D grid
 *
 * Perfect for icon grids, image galleries, or any grid-based UI.
 *
 * @example
 * ```tsx
 * <XRGrid columns={3} cellWidth={0.1} cellHeight={0.1} spacingX={0.02} spacingY={0.02}>
 *   <XRButton>1</XRButton>
 *   <XRButton>2</XRButton>
 *   <XRButton>3</XRButton>
 *   <XRButton>4</XRButton>
 *   <XRButton>5</XRButton>
 *   <XRButton>6</XRButton>
 * </XRGrid>
 * ```
 */
export declare function XRGrid({ position, rotation, scale, visible, opacity, children, onMount, onUnmount, columns, rows, cellWidth, cellHeight, spacingX, spacingY, alignItems, justifyItems, }: XRGridProps): React.JSX.Element | null;
export declare namespace XRGrid {
    var displayName: string;
}
//# sourceMappingURL=XRGrid.d.ts.map
import type { Vec3, Quat, Color } from 'react-native-kimoyo-oju';
export interface KimoyoOjuBoxProps {
    position?: Vec3;
    rotation?: Quat;
    scale?: Vec3;
    visible?: boolean;
    width?: number;
    height?: number;
    length?: number;
    color?: Color;
}
/**
 * KimoyoOjuBox - 3D box/cube primitive.
 *
 * Usage:
 * ```tsx
 * <KimoyoOjuBox
 *   position={[0, 0, -5]}
 *   width={1}
 *   height={1}
 *   length={1}
 *   color={[1, 0, 0, 1]}
 * />
 * ```
 */
export declare function KimoyoOjuBox({ position, rotation, scale, visible, width, height, length, color, }: KimoyoOjuBoxProps): null;
//# sourceMappingURL=KimoyoOjuBox.d.ts.map
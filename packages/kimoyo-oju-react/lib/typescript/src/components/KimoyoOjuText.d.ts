import type { Vec3, Quat, Color } from 'react-native-kimoyo-oju';
export interface KimoyoOjuTextProps {
    text: string;
    position?: Vec3;
    rotation?: Quat;
    scale?: Vec3;
    visible?: boolean;
    fontSize?: number;
    color?: Color;
    fontFamily?: string;
}
/**
 * KimoyoOjuText - 3D text rendered in the scene.
 */
export declare function KimoyoOjuText({ text, position, rotation, scale, visible, fontSize, color, fontFamily, }: KimoyoOjuTextProps): null;
//# sourceMappingURL=KimoyoOjuText.d.ts.map
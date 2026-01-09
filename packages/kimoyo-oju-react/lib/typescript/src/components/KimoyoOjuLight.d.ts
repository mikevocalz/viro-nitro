import type { Vec3, Quat, Color, LightType } from 'react-native-kimoyo-oju';
export interface KimoyoOjuLightProps {
    type: LightType;
    position?: Vec3;
    rotation?: Quat;
    color?: Color;
    intensity?: number;
    range?: number;
    innerConeAngle?: number;
    outerConeAngle?: number;
}
/**
 * KimoyoOjuLight - Light source in the scene.
 */
export declare function KimoyoOjuLight({ type, position, rotation, color, intensity, range, innerConeAngle, outerConeAngle, }: KimoyoOjuLightProps): null;
//# sourceMappingURL=KimoyoOjuLight.d.ts.map
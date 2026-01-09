import type { Vec3, Quat } from 'react-native-kimoyo-oju';
export interface KimoyoOjuCameraProps {
    position?: Vec3;
    rotation?: Quat;
    fov?: number;
    nearClip?: number;
    farClip?: number;
}
/**
 * KimoyoOjuCamera - Camera in the scene (for flat mode).
 * In XR mode, the camera is controlled by the headset.
 */
export declare function KimoyoOjuCamera({ position, rotation, fov, nearClip, farClip, }: KimoyoOjuCameraProps): null;
//# sourceMappingURL=KimoyoOjuCamera.d.ts.map
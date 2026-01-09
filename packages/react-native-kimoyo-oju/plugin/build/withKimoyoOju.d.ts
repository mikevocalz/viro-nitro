import { ConfigPlugin } from '@expo/config-plugins';
export type XRMode = 'flat' | 'immersive-vr' | 'immersive-mr';
export interface KimoyoOjuPluginProps {
    /**
     * iOS-specific configuration
     */
    ios?: {
        /** Camera usage description for Info.plist */
        cameraUsagePermission?: string;
        /** Microphone usage description for Info.plist */
        microphoneUsagePermission?: string;
        /** Photo library usage description */
        photoLibraryUsagePermission?: string;
        /** Location usage description (for AR geolocation features) */
        locationUsagePermission?: string;
        /** Enable ARKit support for iOS AR experiences */
        enableARKit?: boolean;
    };
    /**
     * Android-specific configuration
     */
    android?: {
        /** XR modes to enable: 'flat', 'immersive-vr', 'immersive-mr' */
        xrModes?: XRMode[];
        /** Enable Meta Quest support (adds OpenXR dependencies) */
        enableQuest?: boolean;
        /** Enable ARCore support for Android phones/tablets */
        enableARCore?: boolean;
        /** ARCore requirement: 'required' | 'optional' */
        arCoreRequirement?: 'required' | 'optional';
    };
}
/**
 * Default configuration options
 */
export declare const DEFAULTS: KimoyoOjuPluginProps;
declare const _default: ConfigPlugin<void | KimoyoOjuPluginProps>;
export default _default;

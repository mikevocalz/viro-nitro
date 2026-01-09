import {
  ConfigPlugin,
  withPlugins,
  createRunOncePlugin,
  WarningAggregator,
} from '@expo/config-plugins';
import { withKimoyoOjuAndroid } from './withKimoyoOjuAndroid';
import { withKimoyoOjuIOS } from './withKimoyoOjuIOS';

const pkg = require('../../package.json');

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
export const DEFAULTS: KimoyoOjuPluginProps = {
  ios: {
    cameraUsagePermission: 'Allow $(PRODUCT_NAME) to use your camera for AR experiences',
    microphoneUsagePermission: 'Allow $(PRODUCT_NAME) to use your microphone',
    photoLibraryUsagePermission: 'Allow $(PRODUCT_NAME) to access your photos',
    locationUsagePermission: 'Allow $(PRODUCT_NAME) to use your location for AR experiences',
    enableARKit: true,
  },
  android: {
    xrModes: ['flat', 'immersive-vr'],
    enableQuest: true,
    enableARCore: true,
    arCoreRequirement: 'optional',
  },
};

/**
 * Expo Config Plugin for Kimoyo Oju
 * 
 * Configures the native projects for 3D/XR rendering with:
 * - OpenXR support for Meta Quest
 * - Nitro Modules native linking
 * - Required permissions
 * 
 * @example
 * // app.json or app.config.js
 * {
 *   "expo": {
 *     "plugins": [
 *       ["react-native-kimoyo-oju", {
 *         "android": {
 *           "xrModes": ["flat", "immersive-vr"],
 *           "enableQuest": true
 *         }
 *       }]
 *     ]
 *   }
 * }
 */
const withKimoyoOju: ConfigPlugin<KimoyoOjuPluginProps | void> = (
  config,
  props = {}
) => {
  const options: KimoyoOjuPluginProps = {
    ios: { ...DEFAULTS.ios, ...props?.ios },
    android: { ...DEFAULTS.android, ...props?.android },
  };

  // Validate New Architecture is enabled (required for Nitro)
  const newArchEnabled =
    config.expo?.newArchEnabled === true ||
    (config as any).newArchEnabled === true;

  if (!newArchEnabled) {
    WarningAggregator.addWarningAndroid(
      'withKimoyoOju',
      'Kimoyo Oju requires React Native New Architecture to be enabled. ' +
        'Add "newArchEnabled": true to your app.json/app.config.js.'
    );
    WarningAggregator.addWarningIOS(
      'withKimoyoOju',
      'Kimoyo Oju requires React Native New Architecture to be enabled. ' +
        'Add "newArchEnabled": true to your app.json/app.config.js.'
    );
  }

  return withPlugins(config, [
    [withKimoyoOjuAndroid, options],
    [withKimoyoOjuIOS, options],
  ]);
};

export default createRunOncePlugin(withKimoyoOju, pkg.name, pkg.version);

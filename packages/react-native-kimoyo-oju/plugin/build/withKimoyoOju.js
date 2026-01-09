"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.DEFAULTS = void 0;
const config_plugins_1 = require("@expo/config-plugins");
const withKimoyoOjuAndroid_1 = require("./withKimoyoOjuAndroid");
const withKimoyoOjuIOS_1 = require("./withKimoyoOjuIOS");
const pkg = require('../../package.json');
/**
 * Default configuration options
 */
exports.DEFAULTS = {
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
const withKimoyoOju = (config, props = {}) => {
    const options = {
        ios: { ...exports.DEFAULTS.ios, ...props?.ios },
        android: { ...exports.DEFAULTS.android, ...props?.android },
    };
    // Validate New Architecture is enabled (required for Nitro)
    const newArchEnabled = config.expo?.newArchEnabled === true ||
        config.newArchEnabled === true;
    if (!newArchEnabled) {
        config_plugins_1.WarningAggregator.addWarningAndroid('withKimoyoOju', 'Kimoyo Oju requires React Native New Architecture to be enabled. ' +
            'Add "newArchEnabled": true to your app.json/app.config.js.');
        config_plugins_1.WarningAggregator.addWarningIOS('withKimoyoOju', 'Kimoyo Oju requires React Native New Architecture to be enabled. ' +
            'Add "newArchEnabled": true to your app.json/app.config.js.');
    }
    return (0, config_plugins_1.withPlugins)(config, [
        [withKimoyoOjuAndroid_1.withKimoyoOjuAndroid, options],
        [withKimoyoOjuIOS_1.withKimoyoOjuIOS, options],
    ]);
};
exports.default = (0, config_plugins_1.createRunOncePlugin)(withKimoyoOju, pkg.name, pkg.version);

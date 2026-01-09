import {
  ConfigPlugin,
  withProjectBuildGradle,
  withAppBuildGradle,
  withSettingsGradle,
  withAndroidManifest,
  AndroidConfig,
} from '@expo/config-plugins';
import type { KimoyoOjuPluginProps } from './withKimoyoOju';

/**
 * Configure Android project for Kimoyo Oju
 */
export const withKimoyoOjuAndroid: ConfigPlugin<KimoyoOjuPluginProps> = (
  config,
  props
) => {
  // Add Android Manifest permissions and features
  config = withAndroidManifestMods(config, props);

  // Modify project-level build.gradle
  config = withProjectGradle(config, props);

  // Modify app-level build.gradle  
  config = withAppGradle(config, props);

  // Modify settings.gradle for native module includes
  config = withSettingsGradleMods(config, props);

  return config;
};

/**
 * Add required permissions and features to AndroidManifest.xml
 */
const withAndroidManifestMods: ConfigPlugin<KimoyoOjuPluginProps> = (
  config,
  props
) => {
  return withAndroidManifest(config, async (config) => {
    const manifest = config.modResults.manifest;

    // Ensure uses-permission array exists
    if (!manifest['uses-permission']) {
      manifest['uses-permission'] = [];
    }

    // Ensure uses-feature array exists
    if (!manifest['uses-feature']) {
      manifest['uses-feature'] = [];
    }

    const permissions = manifest['uses-permission'];
    const features = manifest['uses-feature'];

    // Add OpenGL ES 3.0 requirement
    if (!features.some((f: any) => f.$?.['android:glEsVersion'] === '0x00030000')) {
      features.push({
        $: {
          'android:glEsVersion': '0x00030000',
          'android:required': 'true',
        },
      });
    }

    // Quest/OpenXR specific
    if (props?.android?.enableQuest) {
      // Add Quest category for Oculus Store
      const application = manifest.application?.[0];
      if (application) {
        if (!application['meta-data']) {
          application['meta-data'] = [];
        }

        // Mark as VR app for Quest
        if (!application['meta-data'].some((m: any) => 
          m.$?.['android:name'] === 'com.oculus.vr.focusaware'
        )) {
          application['meta-data'].push({
            $: {
              'android:name': 'com.oculus.vr.focusaware',
              'android:value': 'true',
            },
          });
        }

        // Supported devices
        if (!application['meta-data'].some((m: any) => 
          m.$?.['android:name'] === 'com.oculus.supportedDevices'
        )) {
          application['meta-data'].push({
            $: {
              'android:name': 'com.oculus.supportedDevices',
              'android:value': 'quest|quest2|questpro|quest3',
            },
          });
        }
      }

      // Add VR feature
      if (!features.some((f: any) => f.$?.['android:name'] === 'android.hardware.vr.headtracking')) {
        features.push({
          $: {
            'android:name': 'android.hardware.vr.headtracking',
            'android:required': 'false',
            'android:version': '1',
          },
        });
      }
    }

    // ARCore specific (for Android phones/tablets)
    if (props?.android?.enableARCore) {
      const arCoreRequired = props?.android?.arCoreRequirement === 'required';
      
      // Camera permission for AR
      if (!permissions.some((p: any) => p.$?.['android:name'] === 'android.permission.CAMERA')) {
        permissions.push({
          $: { 'android:name': 'android.permission.CAMERA' },
        });
      }

      // Internet permission for AR cloud anchors
      if (!permissions.some((p: any) => p.$?.['android:name'] === 'android.permission.INTERNET')) {
        permissions.push({
          $: { 'android:name': 'android.permission.INTERNET' },
        });
      }

      // ARCore camera feature
      if (!features.some((f: any) => f.$?.['android:name'] === 'android.hardware.camera.ar')) {
        features.push({
          $: {
            'android:name': 'android.hardware.camera.ar',
            'android:required': arCoreRequired ? 'true' : 'false',
          },
        });
      }

      // Camera autofocus feature
      if (!features.some((f: any) => f.$?.['android:name'] === 'android.hardware.camera.autofocus')) {
        features.push({
          $: {
            'android:name': 'android.hardware.camera.autofocus',
            'android:required': 'false',
          },
        });
      }

      // ARCore meta-data
      const application = manifest.application?.[0];
      if (application) {
        if (!application['meta-data']) {
          application['meta-data'] = [];
        }

        // ARCore requirement level
        if (!application['meta-data'].some((m: any) => 
          m.$?.['android:name'] === 'com.google.ar.core'
        )) {
          application['meta-data'].push({
            $: {
              'android:name': 'com.google.ar.core',
              'android:value': arCoreRequired ? 'required' : 'optional',
            },
          });
        }

        // Depth API support (for occlusion)
        if (!application['meta-data'].some((m: any) => 
          m.$?.['android:name'] === 'com.google.ar.core.depth'
        )) {
          application['meta-data'].push({
            $: {
              'android:name': 'com.google.ar.core.depth',
              'android:value': 'optional',
            },
          });
        }
      }
    }

    return config;
  });
};

/**
 * Modify project-level build.gradle
 */
const withProjectGradle: ConfigPlugin<KimoyoOjuPluginProps> = (config, props) => {
  return withProjectBuildGradle(config, async (config) => {
    let contents = config.modResults.contents;

    // Ensure minSdkVersion is at least 24 (required for OpenXR)
    contents = contents.replace(
      /minSdkVersion\s*=\s*\d+/,
      'minSdkVersion = 24'
    );

    config.modResults.contents = contents;
    return config;
  });
};

/**
 * Modify app-level build.gradle
 */
const withAppGradle: ConfigPlugin<KimoyoOjuPluginProps> = (config, props) => {
  return withAppBuildGradle(config, async (config) => {
    let contents = config.modResults.contents;

    // Add Kimoyo Oju dependencies
    const kimoyoOjuDependencies = `
    // ========================================================================
    // Kimoyo Oju Dependencies
    // ========================================================================
    implementation project(':kimoyo-oju-engine')
    
    ${props?.android?.enableQuest ? `// OpenXR for Meta Quest VR support
    implementation 'org.khronos.openxr:openxr_loader_for_android:1.0.34'` : ''}
    
    ${props?.android?.enableARCore ? `// ARCore for Android AR support
    implementation 'com.google.ar:core:1.40.0'
    // ARCore Sceneform for 3D rendering helpers (optional)
    // implementation 'com.google.ar.sceneform:core:1.17.1'
    // Play Services for ARCore availability check
    implementation 'com.google.android.gms:play-services-location:21.0.1'` : ''}
    // ========================================================================`;

    // Insert after react-android dependency
    if (contents.includes('implementation("com.facebook.react:react-android")')) {
      contents = contents.replace(
        'implementation("com.facebook.react:react-android")',
        `implementation("com.facebook.react:react-android")${kimoyoOjuDependencies}`
      );
    }

    // Add CMake configuration for native build
    if (!contents.includes('externalNativeBuild')) {
      const cmakeConfig = `
    externalNativeBuild {
        cmake {
            path file("../../../node_modules/react-native-kimoyo-oju/cpp/CMakeLists.txt")
            version "3.22.1"
        }
    }`;

      // Insert in android block
      contents = contents.replace(
        /android\s*\{/,
        `android {${cmakeConfig}`
      );
    }

    config.modResults.contents = contents;
    return config;
  });
};

/**
 * Modify settings.gradle for native module includes
 */
const withSettingsGradleMods: ConfigPlugin<KimoyoOjuPluginProps> = (config, props) => {
  return withSettingsGradle(config, async (config) => {
    let contents = config.modResults.contents;

    // Add kimoyo-oju-engine project include
    const kimoyoOjuIncludes = `
// Kimoyo Oju Engine
include ':kimoyo-oju-engine'
project(':kimoyo-oju-engine').projectDir = new File(rootProject.projectDir, '../node_modules/react-native-kimoyo-oju/android/kimoyo-oju-engine')
`;

    if (!contents.includes("include ':kimoyo-oju-engine'")) {
      contents += kimoyoOjuIncludes;
    }

    config.modResults.contents = contents;
    return config;
  });
};

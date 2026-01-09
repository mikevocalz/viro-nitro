import {
  ConfigPlugin,
  withInfoPlist,
  withXcodeProject,
  withPodfile,
  withEntitlementsPlist,
} from '@expo/config-plugins';
import type { KimoyoOjuPluginProps } from './withKimoyoOju';

/**
 * Configure iOS project for Kimoyo Oju
 */
export const withKimoyoOjuIOS: ConfigPlugin<KimoyoOjuPluginProps> = (
  config,
  props
) => {
  // Add Info.plist permissions
  config = withInfoPlistMods(config, props);

  // Modify Podfile for native dependencies
  config = withPodfileMods(config, props);

  return config;
};

/**
 * Add required permissions to Info.plist
 */
const withInfoPlistMods: ConfigPlugin<KimoyoOjuPluginProps> = (config, props) => {
  return withInfoPlist(config, (config) => {
    const plist = config.modResults;

    // Camera permission (required for ARKit)
    if (props?.ios?.cameraUsagePermission) {
      plist.NSCameraUsageDescription = props.ios.cameraUsagePermission;
    }

    // Microphone permission (for audio/video recording in AR)
    if (props?.ios?.microphoneUsagePermission) {
      plist.NSMicrophoneUsageDescription = props.ios.microphoneUsagePermission;
    }

    // Photo library permission (for saving AR screenshots/recordings)
    if (props?.ios?.photoLibraryUsagePermission) {
      plist.NSPhotoLibraryUsageDescription = props.ios.photoLibraryUsagePermission;
      plist.NSPhotoLibraryAddUsageDescription = props.ios.photoLibraryUsagePermission;
    }

    // Location permission (for ARKit geo-anchors and World Maps)
    if (props?.ios?.locationUsagePermission) {
      plist.NSLocationWhenInUseUsageDescription = props.ios.locationUsagePermission;
      plist.NSLocationAlwaysAndWhenInUseUsageDescription = props.ios.locationUsagePermission;
    }

    // Required device capabilities
    plist.UIRequiredDeviceCapabilities = plist.UIRequiredDeviceCapabilities || [];
    if (Array.isArray(plist.UIRequiredDeviceCapabilities)) {
      // Metal is required for rendering
      if (!plist.UIRequiredDeviceCapabilities.includes('metal')) {
        plist.UIRequiredDeviceCapabilities.push('metal');
      }
      
      // ARKit capability (if enabled)
      if (props?.ios?.enableARKit) {
        if (!plist.UIRequiredDeviceCapabilities.includes('arkit')) {
          plist.UIRequiredDeviceCapabilities.push('arkit');
        }
      }
    }

    // ARKit-specific settings
    if (props?.ios?.enableARKit) {
      // Allow arbitrary loads for AR asset downloads
      if (!plist.NSAppTransportSecurity) {
        plist.NSAppTransportSecurity = {};
      }
      
      // World alignment (for ARKit)
      plist.UIFileSharingEnabled = true;
      plist.LSSupportsOpeningDocumentsInPlace = true;
    }

    return config;
  });
};

/**
 * Modify Podfile for native dependencies
 */
const withPodfileMods: ConfigPlugin<KimoyoOjuPluginProps> = (config, props) => {
  return withPodfile(config, (config) => {
    let contents = config.modResults.contents;

    // Minimum iOS version for ARKit (12.0) and Metal support
    const minIOSVersion = props?.ios?.enableARKit ? '12.0' : '13.0';

    // Add Kimoyo Oju pod configuration
    const kimoyoOjuPodConfig = `
  # ============================================================================
  # Kimoyo Oju Configuration
  # ============================================================================
  # Ensure minimum iOS version for ARKit/Metal support
  platform :ios, '${minIOSVersion}'
  
  # Kimoyo Oju native module
  pod 'react-native-kimoyo-oju', :path => '../node_modules/react-native-kimoyo-oju'
  
  ${props?.ios?.enableARKit ? `# ARKit is enabled - requires iOS 12.0+
  # ARKit framework is automatically linked by Xcode for iOS 12+` : ''}
  # ============================================================================
`;

    // Insert before use_react_native if it exists
    if (contents.includes('use_react_native!') && !contents.includes('Kimoyo Oju Configuration')) {
      contents = contents.replace(
        'use_react_native!',
        `${kimoyoOjuPodConfig}\n  use_react_native!`
      );
    }

    // Ensure minimum deployment target
    contents = contents.replace(
      /platform :ios, ['"](\d+\.\d+)['"]/,
      (match: string, version: string) => {
        const currentVersion = parseFloat(version);
        const requiredVersion = parseFloat(minIOSVersion);
        return currentVersion < requiredVersion ? `platform :ios, '${minIOSVersion}'` : match;
      }
    );

    // Add post_install hook for ARKit build settings
    if (props?.ios?.enableARKit && !contents.includes('CLANG_ENABLE_MODULES')) {
      const postInstallARKit = `
  post_install do |installer|
    installer.pods_project.targets.each do |target|
      target.build_configurations.each do |config|
        # Enable modules for ARKit
        config.build_settings['CLANG_ENABLE_MODULES'] = 'YES'
        # Ensure minimum iOS version
        config.build_settings['IPHONEOS_DEPLOYMENT_TARGET'] = '${minIOSVersion}'
      end
    end
  end
`;
      // Add before the final 'end' of the Podfile
      if (!contents.includes('post_install do |installer|')) {
        contents = contents.replace(/end\s*$/, `${postInstallARKit}\nend`);
      }
    }

    config.modResults.contents = contents;
    return config;
  });
};

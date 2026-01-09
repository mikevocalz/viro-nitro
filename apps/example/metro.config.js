const path = require('path');
const { getDefaultConfig, mergeConfig } = require('@react-native/metro-config');

const projectRoot = __dirname;
const monorepoRoot = path.resolve(projectRoot, '../..');

/**
 * Metro configuration
 * https://reactnative.dev/docs/metro
 *
 * @type {import('@react-native/metro-config').MetroConfig}
 */
const config = {
  watchFolders: [monorepoRoot],
  resolver: {
    nodeModulesPaths: [
      path.resolve(projectRoot, 'node_modules'),
      path.resolve(monorepoRoot, 'node_modules'),
    ],
    // Deduplicate React to prevent "Invalid hook call" errors in monorepo
    extraNodeModules: {
      'react': path.resolve(projectRoot, 'node_modules/react'),
      'react-native': path.resolve(projectRoot, 'node_modules/react-native'),
    },
    assetExts: [
      // 3D Models
      'gltf', 'glb', 'obj', 'mtl', 'fbx',
      // Textures
      'png', 'jpg', 'jpeg', 'ktx', 'ktx2', 'basis',
      // Audio
      'mp3', 'wav', 'ogg',
      // Video
      'mp4', 'webm',
    ],
  },
};

module.exports = mergeConfig(getDefaultConfig(__dirname), config);

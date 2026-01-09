# Kimoyo Oju

A modern 3D/XR rendering engine for React Native using Nitro Modules (JSI), targeting Meta Quest, Android, and iOS.

## Features

- **Nitro/JSI Native Bindings** - No legacy bridge, pure C++ with JSI
- **OpenXR Support** - Meta Quest 2/3/Pro via OpenXR
- **Cross-Platform** - Android, iOS, visionOS, Meta Quest, Android XR, Windows PC VR
- **XR GUI Components** - Full suite of spatial UI components (panels, buttons, sliders, etc.)
- **Memory Safe** - RAII everywhere, generation counters prevent use-after-free
- **Crash Free** - Strict lifecycle state machine, idempotent teardown
- **Batched Updates** - Command buffer system for efficient prop updates

## Packages

| Package | Description |
|---------|-------------|
| `packages/kimoyo-oju-engine` | C++ rendering engine with OpenXR backend |
| `packages/kimoyo-oju` | Nitro module with HybridObjects + XR GUI |
| `packages/kimoyo-oju-react` | React components (KimoyoOjuView, KimoyoOjuScene, etc.) |
| `apps/example` | Example React Native app |

## Quick Start

```bash
# Clone and install
git clone https://github.com/user/kimoyo-oju.git
cd kimoyo-oju
yarn install

# Run example app
cd apps/example
yarn ios     # iOS Simulator
yarn android # Android device/emulator
yarn quest   # Meta Quest
```

## Usage

```tsx
import React from 'react';
import { KimoyoOjuView, KimoyoOjuScene, KimoyoOjuBox, KimoyoOjuLight } from '@kimoyo-oju/react';

export default function App() {
  return (
    <KimoyoOjuView mode="flat">
      <KimoyoOjuScene>
        <KimoyoOjuLight type="ambient" />
        <KimoyoOjuBox 
          position={[0, 0, -5]} 
          color={[1, 0, 0, 1]} 
        />
      </KimoyoOjuScene>
    </KimoyoOjuView>
  );
}
```

## Architecture

```
┌──────────────────────────────────────────────────────────────────────────┐
│                           React Components                                │
│    KimoyoOjuView  │  KimoyoOjuScene  │  KimoyoOjuNode  │  KimoyoOjuBox   │
└──────────────────────────────────────────────────────────────────────────┘
                                      │
                            Nitro HybridObjects (JSI)
                                      │
┌──────────────────────────────────────────────────────────────────────────┐
│                      Platform Abstraction Layer                           │
│                                                                           │
│    IRenderBackend                              IXRBackend                 │
│    ├── RealityKitBackend (Swift)               ├── ARKitBackend           │
│    ├── VulkanBackend (C++)                     ├── ARCoreBackend          │
│    ├── MetalBackend (ObjC++)                   ├── OpenXRBackend          │
│    └── GLESBackend (C++)                       └── AndroidXRBackend       │
└──────────────────────────────────────────────────────────────────────────┘
                                      │
         ┌────────────────────────────┼────────────────────────────┐
         │                            │                            │
         ▼                            ▼                            ▼
    ┌──────────┐               ┌──────────┐                ┌──────────────┐
    │  Apple   │               │ Android  │                │   XR HMDs    │
    │          │               │  Phones  │                │              │
    │ iOS      │               │          │                │ Meta Quest   │
    │ iPadOS   │               │ Vulkan   │                │ (OpenXR+GLES)│
    │ visionOS │               │ ARCore   │                │              │
    │          │               │          │                │ Android XR   │
    │RealityKit│               │          │                │ (OpenXR+Vk)  │
    │ ARKit    │               │          │                │              │
    └──────────┘               └──────────┘                │ Vision Pro   │
                                                           │ (RealityKit) │
                                                           └──────────────┘
```

## Platform Support

| Platform | Render Backend | XR Backend | Status |
|----------|---------------|------------|--------|
| **iOS/iPadOS** | RealityKit | ARKit | 🔄 Planned |
| **visionOS** | RealityKit | ARKit (Spatial) | 🔄 Planned |
| **Android Phones** | Vulkan | ARCore | 🔄 Planned |
| **Meta Quest** | OpenGL ES | OpenXR | ✅ Supported |
| **Android XR** | Vulkan | OpenXR | 🔄 Planned |
| **Windows PC VR** | D3D11 | OpenXR | 🔄 Planned |

### Windows PC VR (OpenXR Only)

Supports **all OpenXR-compliant runtimes** via a single renderer:

- **Windows Mixed Reality** (WMR OpenXR)
- **SteamVR** (SteamVR OpenXR runtime)
- **PICO** (PC streaming via PICO OpenXR)

**Design principles:**

- ✅ OpenXR is the **single source of truth** for XR behavior
- ✅ D3D11 rendering via `XR_KHR_D3D11_enable`
- ✅ Action-based input via interaction profiles
- ❌ No OpenVR, no vendor SDKs, no per-device forks

## XR GUI Components

A complete set of spatial UI components for building XR interfaces:

### Layout

| Component | Description |
|-----------|-------------|
| `XRPanel` | Container panel for spatial UI |
| `XRStack` | Horizontal/vertical/depth stack layout |
| `XRGrid` | 2D grid layout |

### Input

| Component | Description |
|-----------|-------------|
| `XRButton` | Interactive button with variants |
| `XRSlider` | Value slider (horizontal/vertical) |
| `XRCheckbox` | Toggle checkbox, switch, radio |

### Content

| Component | Description |
|-----------|-------------|
| `XRText` | 3D text rendering |
| `XRVideo` | Video playback (flat/360°/stereo) |
| `XRCylinder` | Curved geometry for displays |

### Interaction

| Component | Description |
|-----------|-------------|
| `XRNearInteraction` | Hand/touch interactions |
| `XRRayInteraction` | Controller ray interactions |
| `XRHUD` | Head-locked UI overlay |
| `XRScrollView` | Scrollable container |

### XR GUI Example

```tsx
import { XRPanel, XRStack, XRButton, XRText, XRSlider } from 'kimoyo-oju';

function SettingsPanel() {
  const [volume, setVolume] = useState(0.5);
  
  return (
    <XRPanel position={[0, 1.5, -1]} width={0.4} height={0.3}>
      <XRStack direction="vertical" spacing={0.02}>
        <XRText style={{ fontSize: 0.03 }}>Settings</XRText>
        <XRSlider value={volume} onChange={setVolume} label="Volume" />
        <XRButton variant="primary" onSelect={() => console.log('saved')}>
          Save
        </XRButton>
      </XRStack>
    </XRPanel>
  );
}
```

## Documentation

- [XR GUI Components](docs/XR_GUI_COMPONENTS.md) - Spatial UI component reference
- [Platform Backends](docs/PLATFORM_BACKENDS.md) - Multi-backend architecture details
- [Technical Design](docs/TECHNICAL_DESIGN.md) - Threading, ownership, lifecycle
- [Test Plan](docs/TEST_PLAN.md) - Soak tests, CI pipeline
- [Milestones](docs/MILESTONES.md) - M1-M5 development roadmap

## Requirements

- React Native >= 0.83
- React >= 19.0
- react-native-nitro-modules >= 0.32.0
- Android: API 24+ (Android 7.0)
- iOS: iOS 15+
- Quest: Quest 2/3/Pro with OpenXR

## 3D Model Formats

| Format | Extension | Description |
|--------|-----------|-------------|
| **glTF 2.0** | `.gltf`, `.glb` | Recommended format, full PBR support |
| **OBJ** | `.obj` | Legacy support, basic materials |
| **FBX** | `.fbx` | Animation support (converted at build time) |

### Metro Configuration

Add 3D asset extensions to your `metro.config.js`:

```js
const { getDefaultConfig, mergeConfig } = require('@react-native/metro-config');

const config = {
  resolver: {
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
```

### Loading Models

```tsx
import { KimoyoOjuModel } from 'kimoyo-oju';

<KimoyoOjuModel
  source={require('./assets/robot.glb')}
  position={[0, 0, -2]}
  scale={0.5}
/>
```

## Permissions

Kimoyo Oju uses an Expo config plugin to automatically configure permissions.

### Android Permissions

```xml
<!-- Camera (AR) -->
<uses-permission android:name="android.permission.CAMERA" />
<uses-feature android:name="android.hardware.camera.ar" android:required="false" />

<!-- OpenXR (Quest/Android XR) -->
<uses-feature android:name="android.hardware.vr.headtracking" android:required="false" />
<uses-permission android:name="com.oculus.permission.HAND_TRACKING" />

<!-- Microphone (voice input) -->
<uses-permission android:name="android.permission.RECORD_AUDIO" />
```

### iOS Permissions

Add to `Info.plist`:

```xml
<key>NSCameraUsageDescription</key>
<string>Required for AR experiences</string>
<key>NSMicrophoneUsageDescription</key>
<string>Required for voice input</string>
```

### Expo Config Plugin

For managed Expo projects, add to `app.json`:

```json
{
  "expo": {
    "plugins": [
      ["kimoyo-oju", {
        "android": {
          "enableAR": true,
          "enableVR": true,
          "enableHandTracking": true
        },
        "ios": {
          "enableAR": true,
          "cameraPermission": "Required for AR experiences"
        }
      }]
    ]
  }
}
```

### Runtime Permission Requests

```tsx
import { KimoyoOjuPermissions } from 'kimoyo-oju';

// Check permissions
const status = await KimoyoOjuPermissions.check('camera');

// Request permissions
const result = await KimoyoOjuPermissions.request('camera');
if (result === 'granted') {
  // Start AR session
}

// Available permissions: 'camera', 'microphone', 'handTracking'
```

## Building

### C++ Engine

```bash
cd packages/kimoyo-oju-engine/cpp
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### With Sanitizers (Development)

```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DKIMOYOOJU_ENABLE_ASAN=ON \
  -DKIMOYOOJU_ENABLE_UBSAN=ON
cmake --build build
```

## License

MIT

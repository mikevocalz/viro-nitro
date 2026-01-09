# Kimoyo Oju: TypeScript API Specification

## Core Types

```typescript
// Platform Detection
type XRPlatform = 'quest' | 'horizon' | 'arcore' | 'arkit' | 'unknown';
type XRSessionMode = 'immersive-ar' | 'immersive-vr' | 'immersive-mr' | 'inline';

interface XRFeatureSupport {
  depthSensing: boolean;
  sceneMesh: boolean;
  sceneSemantics: boolean;
  persistentAnchors: boolean;
  sharedAnchors: boolean;
  handTracking: boolean;
  controllerTracking: boolean;
  passthrough: boolean;
  planeDetection: boolean;
  hitTest: boolean;
}

// Transform Types
interface XRPose {
  position: [number, number, number];
  orientation: [number, number, number, number]; // quaternion xyzw
  timestamp: number;
}

// Anchor Types
type AnchorState = 'pending' | 'locating' | 'located' | 'paused' | 'lost' | 'saved' | 'failed';

interface XRAnchor {
  id: string;
  uuid: string;
  state: AnchorState;
  pose: XRPose;
  confidence: number;
  isPersistent: boolean;
  isShared: boolean;
}

// Depth Types
type DepthQuality = 'low' | 'medium' | 'high';
type OcclusionMode = 'disabled' | 'depth' | 'people' | 'mesh';

interface DepthConfig {
  enabled: boolean;
  quality: DepthQuality;
  occlusionMode: OcclusionMode;
  nearClip: number;
  farClip: number;
}

interface DepthFrame {
  timestamp: number;
  width: number;
  height: number;
  textureId: number;
  intrinsics: { fx: number; fy: number; cx: number; cy: number };
}

// Scene Mesh Types
type MeshClassification = 'floor' | 'ceiling' | 'wall' | 'table' | 'seat' | 'door' | 'window' | 'unknown';

interface MeshBlock {
  id: string;
  classification: MeshClassification;
  pose: XRPose;
  positions: Float32Array;
  normals: Float32Array;
  indices: Uint32Array;
  lastUpdatedAt: number;
}

interface SceneMeshUpdate {
  added: MeshBlock[];
  updated: MeshBlock[];
  removed: string[];
  timestamp: number;
}

// Hand Tracking Types
type Handedness = 'left' | 'right';
type HandJoint = 'wrist' | 'thumb_tip' | 'index_tip' | 'middle_tip' | 'ring_tip' | 'pinky_tip'; // simplified

interface XRHand {
  handedness: Handedness;
  isTracking: boolean;
  joints: Map<HandJoint, XRPose>;
  pinchStrength: number;
  gripStrength: number;
}

// Interaction Types
type InteractionType = 'select' | 'squeeze' | 'pinch' | 'grab' | 'poke' | 'hover';

interface XRInteractionEvent {
  type: InteractionType;
  source: 'hand_left' | 'hand_right' | 'controller_left' | 'controller_right';
  state: 'start' | 'hold' | 'end';
  pose: XRPose;
  targetId?: string;
  hitPoint?: [number, number, number];
}

// Hit Test Types
interface XRHitTestResult {
  pose: XRPose;
  distance: number;
  planeId?: string;
  meshBlockId?: string;
  classification?: MeshClassification;
}
```

## KimoyoOjuXR Module API

```typescript
interface KimoyoOjuXRModule {
  // Platform Detection
  getPlatform(): Promise<XRPlatform>;
  getFeatureSupport(): Promise<XRFeatureSupport>;
  isFeatureAvailable(feature: keyof XRFeatureSupport): Promise<boolean>;

  // Session Management
  requestSession(mode: XRSessionMode): Promise<void>;
  endSession(): Promise<void>;

  // Depth & Occlusion
  configureDepth(config: DepthConfig): Promise<boolean>;
  setOcclusionEnabled(enabled: boolean): void;

  // Scene Mesh
  enableSceneMesh(options?: { updateThrottleMs?: number }): Promise<boolean>;
  disableSceneMesh(): void;
  getSceneMeshBlocks(): Promise<MeshBlock[]>;

  // Anchors
  createAnchor(pose: XRPose): Promise<XRAnchor>;
  createAnchorFromHitTest(hitResult: XRHitTestResult): Promise<XRAnchor>;
  saveAnchor(anchorId: string): Promise<string>;
  loadAnchors(uuids: string[]): Promise<XRAnchor[]>;
  getSavedAnchorUuids(): Promise<string[]>;
  deleteAnchor(uuid: string): Promise<void>;
  detachAnchor(anchorId: string): void;

  // Hand Tracking
  enableHandTracking(options?: { includeMesh?: boolean }): Promise<boolean>;
  disableHandTracking(): void;

  // Hit Testing
  hitTest(screenX: number, screenY: number): Promise<XRHitTestResult[]>;

  // Events
  addEventListener<K extends keyof XREventMap>(event: K, callback: (e: XREventMap[K]) => void): () => void;
  removeEventListener<K extends keyof XREventMap>(event: K, callback: (e: XREventMap[K]) => void): void;
}
```

## React Components

```typescript
// XRScene - Root component for XR experiences
interface XRSceneProps {
  mode: XRSessionMode;
  occlusion?: { mode: OcclusionMode; quality?: DepthQuality };
  sceneMesh?: { enabled: boolean; visible?: boolean; collisions?: boolean };
  handTracking?: { enabled: boolean; showMesh?: boolean };
  onSessionStarted?: () => void;
  onSessionEnded?: (reason: string) => void;
  onError?: (error: Error) => void;
  children: React.ReactNode;
}

// XRAnchor - Spatial anchor component
interface XRAnchorProps {
  pose?: XRPose;           // Create new anchor
  uuid?: string;           // Load existing anchor
  persistent?: boolean;    // Auto-save
  onStateChanged?: (state: AnchorState) => void;
  onLocated?: (anchor: XRAnchor) => void;
  onLost?: () => void;
  children?: React.ReactNode;
}

// XRSceneMesh - Scene mesh visualization
interface XRSceneMeshProps {
  visible?: boolean;
  wireframe?: boolean;
  material?: { color?: string; opacity?: number };
  classifications?: MeshClassification[];
  onMeshUpdated?: (update: SceneMeshUpdate) => void;
}

// XROcclusion - Depth occlusion configuration
interface XROcclusionProps {
  mode: OcclusionMode;
  quality?: DepthQuality;
  onStateChanged?: (active: boolean) => void;
}

// XRHands - Hand tracking visualization
interface XRHandsProps {
  showMesh?: boolean;
  showJoints?: boolean;
  onHandsUpdated?: (left?: XRHand, right?: XRHand) => void;
  onPinch?: (event: XRInteractionEvent) => void;
  onGrab?: (event: XRInteractionEvent) => void;
}
```

## Hooks

```typescript
// useXRFeatures - Query platform capabilities
function useXRFeatures(): {
  platform: XRPlatform;
  features: XRFeatureSupport;
  isLoading: boolean;
  isFeatureAvailable: (feature: keyof XRFeatureSupport) => boolean;
};

// useXRAnchor - Anchor management
function useXRAnchor(options: { pose?: XRPose; uuid?: string; persistent?: boolean }): {
  anchor: XRAnchor | null;
  isLocated: boolean;
  save: () => Promise<string>;
  remove: () => void;
};

// useXRHitTest - Hit testing
function useXRHitTest(): {
  results: XRHitTestResult[];
  hitTest: (screenX: number, screenY: number) => Promise<XRHitTestResult[]>;
};

// useXRHands - Hand tracking
function useXRHands(): {
  leftHand: XRHand | null;
  rightHand: XRHand | null;
  isTracking: boolean;
};
```

## Example Usage

```tsx
import { XRScene, XRAnchor, XRSceneMesh, XROcclusion, XRHands } from '@kimoyo-oju/react';
import { useXRFeatures, useXRHitTest } from '@kimoyo-oju/react/hooks';

function MRExperience() {
  const { features } = useXRFeatures();
  const { hitTest } = useXRHitTest();
  const [anchorPose, setAnchorPose] = useState<XRPose | null>(null);

  const handleTap = async (x: number, y: number) => {
    const results = await hitTest(x, y);
    if (results.length > 0) {
      setAnchorPose(results[0].pose);
    }
  };

  return (
    <XRScene 
      mode="immersive-mr"
      occlusion={{ mode: 'depth', quality: 'high' }}
      sceneMesh={{ enabled: true, visible: false, collisions: true }}
      handTracking={{ enabled: true }}
    >
      {/* Depth occlusion */}
      {features.depthSensing && <XROcclusion mode="depth" />}
      
      {/* Scene mesh for collisions */}
      <XRSceneMesh visible={false} />
      
      {/* Hand tracking */}
      <XRHands 
        showMesh={false}
        onPinch={(e) => console.log('Pinch!', e)}
      />
      
      {/* Place object on anchor */}
      {anchorPose && (
        <XRAnchor pose={anchorPose} persistent>
          <KimoyoOjuBox width={0.1} height={0.1} length={0.1} />
        </XRAnchor>
      )}
    </XRScene>
  );
}
```

## Backwards Compatibility

Existing KimoyoOju APIs continue to work:

```tsx
// Old API (still works)
<KimoyoOjuARScene onTrackingUpdated={...}>
  <KimoyoOjuARPlane ... />
  <KimoyoOjuARImageMarker ... />
</KimoyoOjuARScene>

// New API (recommended)
<XRScene mode="immersive-ar">
  <XRSceneMesh />
  <XROcclusion mode="depth" />
</XRScene>
```

Migration strategy:
1. `KimoyoOjuARScene` wraps `XRScene` internally
2. Old props map to new config
3. New features available via new components
4. Gradual adoption path

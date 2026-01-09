# XR GUI Components

Complete reference for Kimoyo Oju's spatial UI component library.

## Overview

XR GUI components provide a React-like API for building spatial user interfaces in XR applications. All components support 3D positioning, rotation, and scale, and integrate with hand tracking and controller input.

```tsx
import {
  XRPanel,
  XRStack,
  XRButton,
  XRText,
  XRSlider,
} from 'kimoyo-oju';
```

## Common Props

All XR components extend `XRBaseProps`:

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `position` | `[x, y, z]` | `[0, 0, 0]` | 3D position in meters |
| `rotation` | `[x, y, z, w]` | `[0, 0, 0, 1]` | Quaternion rotation |
| `scale` | `number \| [x, y, z]` | `1` | Uniform or per-axis scale |
| `visible` | `boolean` | `true` | Visibility toggle |
| `opacity` | `number` | `1` | Opacity (0-1) |
| `onMount` | `(handle) => void` | - | Called when mounted |
| `onUnmount` | `() => void` | - | Called before unmount |

Interactive components also include:

| Prop | Type | Description |
|------|------|-------------|
| `disabled` | `boolean` | Disable interactions |
| `onHover` | `() => void` | Called on hover start |
| `onHoverEnd` | `() => void` | Called on hover end |
| `onSelect` | `() => void` | Called on selection (click/pinch) |
| `onSelectStart` | `() => void` | Called when selection begins |
| `onSelectEnd` | `() => void` | Called when selection ends |

---

## Layout Components

### XRPanel

Container for spatial UI elements. Acts as a root context for child components.

```tsx
<XRPanel
  position={[0, 1.5, -1]}
  width={0.4}
  height={0.3}
  style={{
    backgroundColor: [0.1, 0.1, 0.1, 0.9],
    cornerRadius: 0.02,
    padding: 0.02,
  }}
>
  {children}
</XRPanel>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `width` | `number` | `0.4` | Panel width in meters |
| `height` | `number` | `0.3` | Panel height in meters |
| `depth` | `number` | `0.01` | Panel depth in meters |
| `style` | `PanelStyle` | - | Visual styling |
| `curved` | `boolean` | `false` | Enable curved panel |
| `curveRadius` | `number` | `1` | Curve radius when curved |
| `followGaze` | `boolean` | `false` | Panel follows user gaze |
| `billboardMode` | `'none' \| 'all' \| 'y-only'` | `'none'` | Billboard behavior |

**PanelStyle:**

```tsx
{
  backgroundColor: [r, g, b, a],  // RGBA color
  backgroundOpacity: number,      // 0-1
  cornerRadius: number,           // In meters
  padding: number | [t, r, b, l], // Uniform or per-side
  borderColor: [r, g, b, a],
  borderWidth: number,
}
```

---

### XRStack

Arrange children in a line (horizontal, vertical, or depth).

```tsx
<XRStack direction="vertical" spacing={0.02} alignment="center">
  <XRButton>Option 1</XRButton>
  <XRButton>Option 2</XRButton>
  <XRButton>Option 3</XRButton>
</XRStack>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `direction` | `'horizontal' \| 'vertical' \| 'depth'` | `'horizontal'` | Stack direction |
| `spacing` | `number` | `0.02` | Space between items (meters) |
| `alignment` | `'start' \| 'center' \| 'end'` | `'center'` | Cross-axis alignment |
| `justifyContent` | `'start' \| 'center' \| 'end' \| 'space-between'` | `'center'` | Main-axis distribution |
| `wrap` | `boolean` | `false` | Wrap to next line |

**Convenience Components:**

```tsx
<XRHStack spacing={0.02}>{/* Horizontal */}</XRHStack>
<XRVStack spacing={0.02}>{/* Vertical */}</XRVStack>
<XRZStack spacing={0.02}>{/* Depth */}</XRZStack>
```

---

### XRGrid

Arrange children in a 2D grid.

```tsx
<XRGrid columns={3} cellWidth={0.08} cellHeight={0.08} spacingX={0.01} spacingY={0.01}>
  {items.map(item => (
    <XRButton key={item.id}>{item.label}</XRButton>
  ))}
</XRGrid>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `columns` | `number` | `3` | Number of columns |
| `rows` | `number` | - | Optional row limit |
| `cellWidth` | `number` | `0.1` | Cell width (meters) |
| `cellHeight` | `number` | `0.1` | Cell height (meters) |
| `spacingX` | `number` | `0.01` | Horizontal gap |
| `spacingY` | `number` | `0.01` | Vertical gap |

---

## Input Components

### XRButton

Interactive button with multiple variants.

```tsx
<XRButton
  variant="primary"
  size="md"
  onSelect={() => handleClick()}
  disabled={isLoading}
>
  Submit
</XRButton>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `label` | `string` | - | Button text (or use children) |
| `variant` | `'primary' \| 'secondary' \| 'ghost' \| 'danger'` | `'primary'` | Visual variant |
| `size` | `'sm' \| 'md' \| 'lg'` | `'md'` | Button size |
| `icon` | `string` | - | Icon name |
| `iconPosition` | `'left' \| 'right'` | `'left'` | Icon placement |
| `fullWidth` | `boolean` | `false` | Stretch to container |
| `loading` | `boolean` | `false` | Show loading state |

**Button Sizes:**

| Size | Height | Font Size |
|------|--------|-----------|
| `sm` | 0.03m | 0.015m |
| `md` | 0.04m | 0.02m |
| `lg` | 0.05m | 0.025m |

**XRIconButton:**

```tsx
<XRIconButton icon="settings" aria-label="Settings" onSelect={openSettings} />
```

---

### XRSlider

Value selection slider.

```tsx
const [volume, setVolume] = useState(0.5);

<XRSlider
  value={volume}
  min={0}
  max={1}
  step={0.01}
  onChange={setVolume}
  showValue
  formatValue={(v) => `${Math.round(v * 100)}%`}
/>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `value` | `number` | - | Controlled value |
| `defaultValue` | `number` | `0` | Uncontrolled default |
| `min` | `number` | `0` | Minimum value |
| `max` | `number` | `1` | Maximum value |
| `step` | `number` | `0.01` | Step increment |
| `orientation` | `'horizontal' \| 'vertical'` | `'horizontal'` | Slider direction |
| `width` | `number` | `0.2` | Track width (meters) |
| `showValue` | `boolean` | `false` | Display current value |
| `formatValue` | `(value) => string` | - | Value formatter |
| `onChange` | `(value) => void` | - | Called during drag |
| `onChangeEnd` | `(value) => void` | - | Called on drag end |

**XRRangeSlider:**

```tsx
<XRRangeSlider
  value={[0.2, 0.8]}
  onChange={setRange}
  minDistance={0.1}
/>
```

---

### XRCheckbox

Toggle checkbox with optional label.

```tsx
const [enabled, setEnabled] = useState(false);

<XRCheckbox
  checked={enabled}
  onChange={setEnabled}
  label="Enable notifications"
/>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `checked` | `boolean` | - | Controlled state |
| `defaultChecked` | `boolean` | `false` | Uncontrolled default |
| `label` | `string` | - | Label text |
| `size` | `'sm' \| 'md' \| 'lg'` | `'md'` | Checkbox size |
| `onChange` | `(checked) => void` | - | Called on toggle |

**XRSwitch:**

Toggle switch variant:

```tsx
<XRSwitch checked={darkMode} onChange={setDarkMode} label="Dark Mode" />
```

**XRRadio:**

Radio button for single selection:

```tsx
<XRStack direction="vertical">
  <XRRadio value="low" selectedValue={quality} label="Low" onSelect={() => setQuality('low')} />
  <XRRadio value="medium" selectedValue={quality} label="Medium" onSelect={() => setQuality('medium')} />
  <XRRadio value="high" selectedValue={quality} label="High" onSelect={() => setQuality('high')} />
</XRStack>
```

---

## Content Components

### XRText

3D text rendering.

```tsx
<XRText
  style={{
    fontSize: 0.03,
    color: [1, 1, 1, 1],
    fontWeight: 'bold',
    textAlign: 'center',
  }}
>
  Hello World
</XRText>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `text` | `string` | - | Text content (or use children) |
| `style` | `TextStyle` | - | Text styling |

**TextStyle:**

```tsx
{
  fontSize: number,           // In meters (default: 0.02)
  fontFamily: string,         // Font name (default: 'system')
  fontWeight: 'normal' | 'bold',
  color: [r, g, b, a],        // RGBA
  textAlign: 'left' | 'center' | 'right',
  lineHeight: number,         // Multiplier
  letterSpacing: number,      // In meters
}
```

**Convenience Components:**

```tsx
<XRHeading>Large Title</XRHeading>  {/* fontSize: 0.04, bold */}
<XRLabel>Small label</XRLabel>       {/* fontSize: 0.015, secondary color */}
```

---

### XRVideo

Video playback for flat, 360°, and stereoscopic content.

```tsx
<XRVideo
  source={{ uri: 'https://example.com/video.mp4' }}
  width={1.6}
  height={0.9}
  autoplay
  loop
  controls
/>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `source` | `string \| { uri: string }` | - | Video URL |
| `width` | `number` | `0.8` | Video width (meters) |
| `height` | `number` | `0.45` | Video height (meters) |
| `autoplay` | `boolean` | `false` | Auto-start playback |
| `loop` | `boolean` | `false` | Loop video |
| `muted` | `boolean` | `false` | Mute audio |
| `volume` | `number` | `1.0` | Volume (0-1) |
| `controls` | `boolean` | `true` | Show play/pause on tap |
| `poster` | `string` | - | Poster image URL |
| `projection` | `'flat' \| 'equirectangular' \| 'equirectangular-stereo'` | `'flat'` | Video projection |
| `stereoMode` | `'none' \| 'left-right' \| 'top-bottom'` | `'none'` | Stereo layout |

**360° Video:**

```tsx
<XRVideo
  source={{ uri: 'https://example.com/360-video.mp4' }}
  projection="equirectangular"
/>
```

**Chroma Key (Green Screen):**

```tsx
<XRVideo
  source={{ uri: 'presenter.mp4' }}
  chromaKeyColor={[0, 1, 0]}  // Green
  chromaKeyThreshold={0.4}
/>
```

**Callbacks:**

| Callback | Parameters | Description |
|----------|------------|-------------|
| `onLoad` | - | Video loaded |
| `onError` | `(error: string)` | Load/playback error |
| `onProgress` | `({ currentTime, duration })` | Time update |
| `onEnd` | - | Video finished |
| `onStateChange` | `(state: VideoState)` | State changed |

---

### XRCylinder

Cylindrical geometry for curved displays.

```tsx
<XRCylinder
  radius={2}
  height={1}
  thetaLength={Math.PI / 2}  // 90° arc
  mapping="inside"
  texture="panorama.jpg"
/>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `radius` | `number` | `0.5` | Cylinder radius |
| `height` | `number` | `1.0` | Cylinder height |
| `radialSegments` | `number` | `32` | Geometry detail |
| `openEnded` | `boolean` | `false` | Remove caps |
| `thetaStart` | `number` | `0` | Arc start angle (radians) |
| `thetaLength` | `number` | `2π` | Arc length (radians) |
| `color` | `[r, g, b, a]` | - | Surface color |
| `texture` | `string` | - | Texture URL |
| `mapping` | `'inside' \| 'outside'` | `'outside'` | Texture mapping |

**XRCurvedPanel:**

Convenience for curved UI panels:

```tsx
<XRCurvedPanel
  width={1}
  height={0.5}
  curveRadius={2}
  curveAngle={Math.PI / 3}  // 60° arc
>
  {children}
</XRCurvedPanel>
```

---

## Scroll Components

### XRScrollView

Scrollable container for content.

```tsx
<XRScrollView
  width={0.4}
  height={0.3}
  contentHeight={1.0}
  direction="vertical"
  showScrollbar
>
  <XRStack direction="vertical" spacing={0.02}>
    {items.map(item => <XRText key={item.id}>{item.name}</XRText>)}
  </XRStack>
</XRScrollView>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `width` | `number` | `0.4` | Viewport width |
| `height` | `number` | `0.3` | Viewport height |
| `contentWidth` | `number` | - | Content width (if > viewport, enables horizontal scroll) |
| `contentHeight` | `number` | - | Content height (if > viewport, enables vertical scroll) |
| `direction` | `'horizontal' \| 'vertical' \| 'both'` | `'vertical'` | Scroll direction |
| `showScrollbar` | `boolean` | `true` | Show scrollbar |
| `bounces` | `boolean` | `true` | Bounce at edges |
| `pagingEnabled` | `boolean` | `false` | Snap to pages |
| `snapToInterval` | `number` | - | Snap interval |
| `onScroll` | `({ x, y }) => void` | - | Scroll position change |
| `onScrollEnd` | `({ x, y }) => void` | - | Scroll finished |

**ScrollViewer:**

Alias with desktop-style defaults (no bounce):

```tsx
<ScrollViewer width={0.4} height={0.3}>
  {content}
</ScrollViewer>
```

---

## Interaction Components

### XRNearInteraction

Enable near-field hand interactions (touch/poke).

```tsx
<XRNearInteraction
  onNearSelect={(hand, point) => console.log(`${hand} touched at`, point)}
  hapticOnSelect
>
  <XRButton>Touch Me</XRButton>
</XRNearInteraction>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `enabled` | `boolean` | `true` | Enable interactions |
| `hoverDistance` | `number` | `0.05` | Hover trigger distance |
| `selectDistance` | `number` | `0.02` | Select trigger distance |
| `hapticOnHover` | `boolean` | `true` | Haptic on hover |
| `hapticOnSelect` | `boolean` | `true` | Haptic on select |

**Callbacks:**

| Callback | Parameters | Description |
|----------|------------|-------------|
| `onNearEnter` | `(hand: 'left' \| 'right')` | Hand entered range |
| `onNearExit` | `(hand: 'left' \| 'right')` | Hand exited range |
| `onNearSelect` | `(hand, point: Vec3)` | Touch/poke occurred |
| `onNearSelectEnd` | `(hand)` | Touch released |

---

### XRRayInteraction

Enable ray-based pointing interactions.

```tsx
<XRRayInteraction
  showRay
  rayColor={[0.2, 0.5, 1.0, 0.8]}
  onRaySelect={(hand, point, normal) => console.log('Selected at', point)}
>
  <XRPanel position={[0, 1.5, -2]}>
    <XRButton>Click Me</XRButton>
  </XRPanel>
</XRRayInteraction>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `enabled` | `boolean` | `true` | Enable interactions |
| `maxDistance` | `number` | `10` | Maximum ray distance |
| `showRay` | `boolean` | `true` | Render ray visual |
| `rayColor` | `[r, g, b, a]` | `[0.2, 0.5, 1, 0.8]` | Ray color |
| `rayWidth` | `number` | `0.002` | Ray width |
| `cursorSize` | `number` | `0.01` | Cursor radius |
| `cursorColor` | `[r, g, b, a]` | `[1, 1, 1, 1]` | Cursor color |

**Callbacks:**

| Callback | Parameters | Description |
|----------|------------|-------------|
| `onRayEnter` | `(hand)` | Ray hit object |
| `onRayExit` | `(hand)` | Ray left object |
| `onRaySelect` | `(hand, point, normal)` | Trigger pressed |
| `onRaySelectEnd` | `(hand)` | Trigger released |
| `onRayMove` | `(hand, point)` | Ray moved while hovering |

---

### useInteraction Hook

Access interaction state from child components:

```tsx
function InteractiveContent() {
  const { isHovered, isSelected, hitPoint } = useInteraction();
  
  return (
    <XRText style={{ color: isHovered ? [1, 1, 0, 1] : [1, 1, 1, 1] }}>
      {isSelected ? 'Selected!' : 'Hover me'}
    </XRText>
  );
}
```

---

## HUD Components

### XRHUD

Head-locked UI overlay.

```tsx
<XRHUD anchor="bottom-center" distance={1.5}>
  <XRPanel>
    <XRText>Health: 100</XRText>
  </XRPanel>
</XRHUD>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `anchor` | `HUDAnchor` | `'center'` | Screen position |
| `distance` | `number` | `1.5` | Distance from head |
| `offsetX` | `number` | `0` | Horizontal offset |
| `offsetY` | `number` | `0` | Vertical offset |
| `followHead` | `boolean` | `true` | Track head movement |
| `followSpeed` | `number` | `5` | Tracking smoothness |
| `fadeAtEdge` | `boolean` | `true` | Fade at peripheral |
| `alwaysVisible` | `boolean` | `true` | Ignore occlusion |

**Anchor Positions:**

```
top-left      top-center      top-right
center-left   center          center-right
bottom-left   bottom-center   bottom-right
```

---

### XRTooltip

Contextual tooltip.

```tsx
<XRTooltip text="Save your changes" show={isHovered} placement="top">
  <XRButton onHover={() => setIsHovered(true)} onHoverEnd={() => setIsHovered(false)}>
    Save
  </XRButton>
</XRTooltip>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `text` | `string` | - | Tooltip content |
| `show` | `boolean` | `false` | Visibility |
| `delay` | `number` | `500` | Show delay (ms) |
| `placement` | `'top' \| 'bottom' \| 'left' \| 'right'` | `'top'` | Position |
| `maxWidth` | `number` | `0.2` | Max width |

---

### XRNotification

Toast notification.

```tsx
<XRNotification
  message="Settings saved!"
  type="success"
  duration={3000}
  onDismiss={() => setShowNotification(false)}
/>
```

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `message` | `string` | - | Notification text |
| `type` | `'info' \| 'success' \| 'warning' \| 'error'` | `'info'` | Visual type |
| `duration` | `number` | `3000` | Auto-dismiss (ms, 0 = never) |
| `onDismiss` | `() => void` | - | Called on dismiss |

---

## Design Tokens

### Colors

```tsx
import { XRColors } from 'kimoyo-oju';

XRColors.primary       // [0.2, 0.4, 0.9, 1.0] - Primary accent
XRColors.secondary     // [0.25, 0.25, 0.25, 1.0] - Secondary
XRColors.surface       // [0.12, 0.12, 0.12, 1.0] - Panel backgrounds
XRColors.text          // [1.0, 1.0, 1.0, 1.0] - Primary text
XRColors.textSecondary // [0.7, 0.7, 0.7, 1.0] - Secondary text
XRColors.border        // [0.3, 0.3, 0.3, 1.0] - Borders
XRColors.hover         // [0.3, 0.5, 1.0, 1.0] - Hover state
XRColors.active        // [0.15, 0.35, 0.8, 1.0] - Active state
XRColors.danger        // [0.9, 0.2, 0.2, 1.0] - Danger/error
XRColors.success       // [0.2, 0.8, 0.4, 1.0] - Success
XRColors.warning       // [0.9, 0.7, 0.1, 1.0] - Warning
```

### Dimensions

```tsx
import { XRDimensions } from 'kimoyo-oju';

XRDimensions.spacing.xs  // 0.005m
XRDimensions.spacing.sm  // 0.01m
XRDimensions.spacing.md  // 0.02m
XRDimensions.spacing.lg  // 0.03m
XRDimensions.spacing.xl  // 0.05m

XRDimensions.fontSize.xs // 0.012m
XRDimensions.fontSize.sm // 0.015m
XRDimensions.fontSize.md // 0.02m
XRDimensions.fontSize.lg // 0.025m
XRDimensions.fontSize.xl // 0.04m

XRDimensions.buttonHeight.sm // 0.03m
XRDimensions.buttonHeight.md // 0.04m
XRDimensions.buttonHeight.lg // 0.05m
```

---

## Complete Example

```tsx
import React, { useState } from 'react';
import {
  XRPanel,
  XRStack,
  XRText,
  XRButton,
  XRSlider,
  XRCheckbox,
  XRScrollView,
  XRHUD,
  XRNearInteraction,
  XRRayInteraction,
} from 'kimoyo-oju';

export function SettingsMenu() {
  const [volume, setVolume] = useState(0.7);
  const [musicEnabled, setMusicEnabled] = useState(true);
  const [sfxEnabled, setSfxEnabled] = useState(true);

  return (
    <XRRayInteraction showRay>
      <XRNearInteraction>
        <XRPanel
          position={[0, 1.5, -1]}
          width={0.5}
          height={0.4}
          style={{ cornerRadius: 0.02, padding: 0.03 }}
        >
          <XRStack direction="vertical" spacing={0.025}>
            <XRText style={{ fontSize: 0.035, fontWeight: 'bold' }}>
              Audio Settings
            </XRText>
            
            <XRStack direction="horizontal" spacing={0.02}>
              <XRText>Volume</XRText>
              <XRSlider
                value={volume}
                onChange={setVolume}
                width={0.25}
                showValue
                formatValue={(v) => `${Math.round(v * 100)}%`}
              />
            </XRStack>
            
            <XRCheckbox
              checked={musicEnabled}
              onChange={setMusicEnabled}
              label="Background Music"
            />
            
            <XRCheckbox
              checked={sfxEnabled}
              onChange={setSfxEnabled}
              label="Sound Effects"
            />
            
            <XRStack direction="horizontal" spacing={0.02}>
              <XRButton variant="secondary" onSelect={() => console.log('Cancel')}>
                Cancel
              </XRButton>
              <XRButton variant="primary" onSelect={() => console.log('Save')}>
                Save
              </XRButton>
            </XRStack>
          </XRStack>
        </XRPanel>
      </XRNearInteraction>
    </XRRayInteraction>
  );
}
```

---

## Best Practices

### Positioning

- Place panels at **1-2 meters** distance for comfortable viewing
- Eye level is approximately **1.5m** from floor
- Avoid placing UI directly at 0,0,0 (inside user's head)

### Sizing

- **Font sizes:** 0.02-0.04m for readable text at 1m distance
- **Buttons:** Minimum 0.03m height for touch targets
- **Panels:** Keep under 60° of field of view

### Performance

- Use `visible={false}` instead of conditional rendering for toggleable UI
- Batch updates with command buffers when possible
- Avoid deep component nesting

### Accessibility

- Provide sufficient contrast (4.5:1 minimum)
- Support both near and ray interactions
- Include haptic feedback for selections
- Use `aria-label` props for icon buttons

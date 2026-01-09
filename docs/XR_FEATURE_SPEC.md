# Kimoyo Oju: Modern XR/MR Feature Specification

## Executive Summary

This document specifies modern AR/MR features for Kimoyo Oju, prioritizing **Meta Quest / Horizon OS** passthrough MR, then Android ARCore, with iOS ARKit optional.

**References:**
- [KimoyoOjuCore](https://github.com/ReactVision/kimoyoojucore)
- [Meta Horizon OS](https://developers.meta.com/horizon/develop/)
- [Babylon.js WebXR](https://doc.babylonjs.com/features/featuresDeepDive/webXR/webXRFeatures)

---

# 1. Prioritized Roadmap

## P0 - Critical (Quest MR MVP) - Weeks 1-5

| Feature | Quest | ARCore | ARKit |
|---------|-------|--------|-------|
| **Depth Occlusion** | Quest Depth API | Depth API | LiDAR |
| **Scene Mesh** | Scene Model API | - | Scene Reconstruction |
| **Persistent Anchors** | Spatial Anchors | Cloud Anchors | World Map |
| **Hand Tracking** | Hand Tracking API | - | Hand Tracking |
| **MR Passthrough** | Passthrough API | - | - |

## P1 - Important - Weeks 6-8

| Feature | Quest | ARCore | ARKit |
|---------|-------|--------|-------|
| Scene Semantics | Scene Understanding | Scene Semantics | - |
| Shared Anchors | Shared Spatial Anchors | Cloud Anchors | - |
| Controller Input | Controller API | - | - |
| People Occlusion | - | - | People Occlusion |

## P2 - Future

| Feature | Quest | ARCore | ARKit |
|---------|-------|--------|-------|
| Streetscape/Geo | - | Geospatial API | ARGeoAnchor |
| Body Tracking | Body Tracking API | - | Body Detection |
| Eye Tracking | Eye Tracking API | - | - |

---

# 2. Feature Gap Analysis

## Current KimoyoOju Capabilities

| Feature | Status | Notes |
|---------|--------|-------|
| Plane Detection | ✅ | Horizontal/vertical |
| Image Tracking | ✅ | AR markers |
| Hit Testing | ✅ | Ray-based |
| Light Estimation | ✅ | Ambient intensity |
| Basic Anchors | ✅ | Session-scoped only |
| 3D Object Placement | ✅ | Via anchors |

## Gap: Missing Modern Features

| Feature | Gap | Solution |
|---------|-----|----------|
| Depth Occlusion | Not implemented | Quest Depth API / ARCore Depth |
| Scene Mesh | Not implemented | Quest Scene Model / ARKit |
| Persistent Anchors | Session-only | Platform spatial anchors |
| Hand Tracking | Not implemented | Quest Hand Tracking API |
| Semantics | Not implemented | Scene labels/classification |
| MR Composition | Basic | Proper layer compositing |

---

See `XR_API_SPEC.md` for TypeScript API definitions.
See `XR_NATIVE_SPEC.md` for native implementation architecture.
See `XR_IMPL_SPEC.md` for implementation pseudocode.
See `XR_TEST_SPEC.md` for acceptance criteria and test matrix.

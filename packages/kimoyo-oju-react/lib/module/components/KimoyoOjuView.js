"use strict";

import React, { useCallback } from 'react';
import { View, StyleSheet } from 'react-native';
import { KimoyoOjuNativeView } from 'react-native-kimoyo-oju';
import { KimoyoOjuProvider } from '../context/KimoyoOjuContext';
import { jsx as _jsx, jsxs as _jsxs } from "react/jsx-runtime";
/**
 * KimoyoOjuView - Main container for KimoyoOju 3D/XR content.
 * 
 * This component:
 * 1. Creates the native OpenGL view that hosts the renderer
 * 2. Provides KimoyoOjuContext to all children
 * 3. Handles lifecycle events
 * 
 * Usage:
 * ```tsx
 * <KimoyoOjuView mode="flat" style={{ flex: 1 }}>
 *   <KimoyoOjuScene>
 *     <KimoyoOjuBox position={[0, 0, -5]} />
 *   </KimoyoOjuScene>
 * </KimoyoOjuView>
 * ```
 */
export function KimoyoOjuView({
  style,
  mode = 'flat',
  children,
  onError
}) {
  const handleError = useCallback((code, message) => {
    onError?.(code, message);
  }, [onError]);
  return /*#__PURE__*/_jsx(KimoyoOjuProvider, {
    onError: handleError,
    children: /*#__PURE__*/_jsxs(View, {
      style: [styles.container, style],
      children: [/*#__PURE__*/_jsx(KimoyoOjuNativeView, {
        style: styles.glView,
        mode: mode
      }), /*#__PURE__*/_jsx(View, {
        style: styles.overlay,
        children: children
      })]
    })
  });
}
const styles = StyleSheet.create({
  container: {
    flex: 1
  },
  glView: {
    ...StyleSheet.absoluteFillObject
  },
  overlay: {
    ...StyleSheet.absoluteFillObject
  }
});
//# sourceMappingURL=KimoyoOjuView.js.map
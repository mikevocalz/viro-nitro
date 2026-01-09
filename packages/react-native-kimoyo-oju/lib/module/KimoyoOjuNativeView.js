"use strict";

import React from 'react';
import { requireNativeComponent } from 'react-native';
import { jsx as _jsx } from "react/jsx-runtime";
const NativeView = requireNativeComponent('KimoyoOjuView');
export function KimoyoOjuNativeView({
  style,
  mode = 'flat'
}) {
  return /*#__PURE__*/_jsx(NativeView, {
    style: style,
    mode: mode
  });
}
//# sourceMappingURL=KimoyoOjuNativeView.js.map
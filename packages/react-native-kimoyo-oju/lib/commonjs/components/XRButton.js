"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRButton = XRButton;
exports.XRIconButton = XRIconButton;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _types = require("./types");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * XRButton Props
 */

/**
 * Get button colors based on variant and state
 */
function getButtonColors(variant, isHovered, isPressed, disabled) {
  if (disabled) {
    return {
      bg: [0.3, 0.3, 0.3, 0.5],
      text: [0.5, 0.5, 0.5, 1.0],
      border: [0.3, 0.3, 0.3, 0.5]
    };
  }
  const colors = {
    primary: {
      bg: isPressed ? _types.XRColors.active : isHovered ? _types.XRColors.hover : _types.XRColors.primary,
      text: _types.XRColors.text,
      border: _types.XRColors.primary
    },
    secondary: {
      bg: isPressed ? [0.35, 0.35, 0.35, 1.0] : isHovered ? [0.3, 0.3, 0.3, 1.0] : _types.XRColors.secondary,
      text: _types.XRColors.text,
      border: _types.XRColors.border
    },
    ghost: {
      bg: isPressed ? [0.2, 0.2, 0.2, 0.8] : isHovered ? [0.15, 0.15, 0.15, 0.6] : [0, 0, 0, 0],
      text: _types.XRColors.text,
      border: [0, 0, 0, 0]
    },
    danger: {
      bg: isPressed ? [0.7, 0.1, 0.1, 1.0] : isHovered ? [0.8, 0.15, 0.15, 1.0] : _types.XRColors.danger,
      text: _types.XRColors.text,
      border: _types.XRColors.danger
    }
  };
  return colors[variant];
}

/**
 * Get button dimensions based on size
 */
function getButtonDimensions(size) {
  const dimensions = {
    sm: {
      height: _types.XRDimensions.buttonHeight.sm,
      paddingX: 0.015,
      fontSize: _types.XRDimensions.fontSize.sm
    },
    md: {
      height: _types.XRDimensions.buttonHeight.md,
      paddingX: 0.02,
      fontSize: _types.XRDimensions.fontSize.md
    },
    lg: {
      height: _types.XRDimensions.buttonHeight.lg,
      paddingX: 0.025,
      fontSize: _types.XRDimensions.fontSize.lg
    }
  };
  return dimensions[size];
}

/**
 * XRButton - Interactive 3D button component
 * 
 * Supports multiple variants, sizes, and interaction states.
 * 
 * @example
 * ```tsx
 * <XRButton 
 *   variant="primary" 
 *   size="md"
 *   onSelect={() => console.log('Clicked!')}
 * >
 *   Click Me
 * </XRButton>
 * ```
 */
function XRButton({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  onMount,
  onUnmount,
  disabled = false,
  onHover,
  onHoverEnd,
  onSelect,
  onSelectStart,
  onSelectEnd,
  label,
  children,
  variant = 'primary',
  size = 'md',
  icon,
  iconPosition = 'left',
  fullWidth = false,
  loading = false,
  textStyle
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const [isHovered, setIsHovered] = (0, _react.useState)(false);
  const [isPressed, setIsPressed] = (0, _react.useState)(false);
  const {
    handle,
    isReady
  } = (0, _useXRNode.useXRNode)('group', {
    position,
    rotation,
    scale,
    visible,
    parentHandle,
    onMount,
    onUnmount
  });
  const displayText = label ?? (typeof children === 'string' ? children : '');
  const dimensions = getButtonDimensions(size);
  const colors = getButtonColors(variant, isHovered, isPressed, disabled);
  const handleHover = (0, _react.useCallback)(() => {
    if (disabled) return;
    setIsHovered(true);
    onHover?.();
  }, [disabled, onHover]);
  const handleHoverEnd = (0, _react.useCallback)(() => {
    setIsHovered(false);
    onHoverEnd?.();
  }, [onHoverEnd]);
  const handleSelectStart = (0, _react.useCallback)(() => {
    if (disabled) return;
    setIsPressed(true);
    onSelectStart?.();
  }, [disabled, onSelectStart]);
  const handleSelectEnd = (0, _react.useCallback)(() => {
    setIsPressed(false);
    onSelectEnd?.();
  }, [onSelectEnd]);
  const handleSelect = (0, _react.useCallback)(() => {
    if (disabled || loading) return;
    onSelect?.();
  }, [disabled, loading, onSelect]);
  if (!isReady || !handle) {
    return null;
  }
  return null;
}
XRButton.displayName = 'XRButton';

/**
 * XRIconButton - Button with only an icon
 */

function XRIconButton({
  icon,
  size = 'md',
  ...props
}) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRButton, {
    ...props,
    icon: icon,
    size: size,
    variant: props.variant ?? 'ghost'
  });
}
XRIconButton.displayName = 'XRIconButton';
//# sourceMappingURL=XRButton.js.map
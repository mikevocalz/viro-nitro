"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRCheckbox = XRCheckbox;
exports.XRRadio = XRRadio;
exports.XRSwitch = XRSwitch;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * XRCheckbox Props
 */

/**
 * XRCheckbox - Toggle checkbox component
 * 
 * @example
 * ```tsx
 * <XRCheckbox
 *   checked={enabled}
 *   onChange={setEnabled}
 *   label="Enable feature"
 * />
 * ```
 */
function XRCheckbox({
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
  checked: controlledChecked,
  defaultChecked = false,
  label,
  size = 'md',
  onChange
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const [internalChecked, setInternalChecked] = (0, _react.useState)(defaultChecked);
  const [isHovered, setIsHovered] = (0, _react.useState)(false);
  const checked = controlledChecked !== undefined ? controlledChecked : internalChecked;
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
  const boxSize = {
    sm: 0.02,
    md: 0.025,
    lg: 0.03
  }[size];
  const handleToggle = (0, _react.useCallback)(() => {
    if (disabled) return;
    const newValue = !checked;
    if (controlledChecked === undefined) {
      setInternalChecked(newValue);
    }
    onChange?.(newValue);
    onSelect?.();
  }, [disabled, checked, controlledChecked, onChange, onSelect]);
  const handleHover = (0, _react.useCallback)(() => {
    if (disabled) return;
    setIsHovered(true);
    onHover?.();
  }, [disabled, onHover]);
  const handleHoverEnd = (0, _react.useCallback)(() => {
    setIsHovered(false);
    onHoverEnd?.();
  }, [onHoverEnd]);
  if (!isReady || !handle) {
    return null;
  }
  return null;
}
XRCheckbox.displayName = 'XRCheckbox';

/**
 * XRSwitch - Toggle switch variant
 */

function XRSwitch(props) {
  // Switch is visually different but same behavior
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRCheckbox, {
    ...props
  });
}
XRSwitch.displayName = 'XRSwitch';

/**
 * XRRadio - Radio button component
 */

function XRRadio({
  position,
  rotation,
  scale,
  visible = true,
  disabled = false,
  value,
  selectedValue,
  label,
  size = 'md',
  onSelect,
  onMount,
  onUnmount
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const [isHovered, setIsHovered] = (0, _react.useState)(false);
  const isSelected = value === selectedValue;
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
  if (!isReady || !handle) {
    return null;
  }
  return null;
}
XRRadio.displayName = 'XRRadio';
//# sourceMappingURL=XRCheckbox.js.map
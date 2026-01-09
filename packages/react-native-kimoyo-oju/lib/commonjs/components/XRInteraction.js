"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.XRNearInteraction = XRNearInteraction;
exports.XRRayInteraction = XRRayInteraction;
exports.useInteraction = useInteraction;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * Interaction context for nested components
 */

const InteractionContext = /*#__PURE__*/(0, _react.createContext)({
  isHovered: false,
  isSelected: false,
  interactionType: null,
  hitPoint: null,
  hitNormal: null
});
function useInteraction() {
  return (0, _react.useContext)(InteractionContext);
}

/**
 * XRNearInteraction Props
 */

/**
 * XRNearInteraction - Near-field hand interaction component
 * 
 * Enables direct touch/poke interactions with hands.
 * Wrap interactive elements to enable near interaction.
 * 
 * @example
 * ```tsx
 * <XRNearInteraction onNearSelect={(hand) => console.log(`${hand} hand selected`)}>
 *   <XRButton>Touch Me</XRButton>
 * </XRNearInteraction>
 * ```
 */
function XRNearInteraction({
  position,
  rotation,
  scale,
  visible = true,
  children,
  onMount,
  onUnmount,
  enabled = true,
  hoverDistance = 0.05,
  selectDistance = 0.02,
  onNearEnter,
  onNearExit,
  onNearSelect,
  onNearSelectEnd,
  hapticOnHover = true,
  hapticOnSelect = true
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const [state, setState] = (0, _react.useState)({
    isHovered: false,
    isSelected: false,
    interactionType: null,
    hitPoint: null,
    hitNormal: null
  });
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
  const handleNearEnter = (0, _react.useCallback)(hand => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      isHovered: true,
      interactionType: 'near'
    }));
    onNearEnter?.(hand);
  }, [enabled, onNearEnter]);
  const handleNearExit = (0, _react.useCallback)(hand => {
    setState(prev => ({
      ...prev,
      isHovered: false,
      interactionType: null
    }));
    onNearExit?.(hand);
  }, [onNearExit]);
  const handleNearSelect = (0, _react.useCallback)((hand, point) => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      isSelected: true,
      hitPoint: point
    }));
    onNearSelect?.(hand, point);
  }, [enabled, onNearSelect]);
  const handleNearSelectEnd = (0, _react.useCallback)(hand => {
    setState(prev => ({
      ...prev,
      isSelected: false
    }));
    onNearSelectEnd?.(hand);
  }, [onNearSelectEnd]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(InteractionContext.Provider, {
    value: state,
    children: children
  });
}
XRNearInteraction.displayName = 'XRNearInteraction';

/**
 * XRRayInteraction Props
 */

/**
 * XRRayInteraction - Ray-based pointing interaction
 * 
 * Enables controller/hand ray interactions for distant objects.
 * 
 * @example
 * ```tsx
 * <XRRayInteraction 
 *   showRay
 *   onRaySelect={(hand, point) => console.log(`Selected at ${point}`)}
 * >
 *   <XRPanel position={[0, 1.5, -2]}>
 *     <XRButton>Click Me</XRButton>
 *   </XRPanel>
 * </XRRayInteraction>
 * ```
 */
function XRRayInteraction({
  position,
  rotation,
  scale,
  visible = true,
  children,
  onMount,
  onUnmount,
  enabled = true,
  maxDistance = 10,
  showRay = true,
  rayColor = [0.2, 0.5, 1.0, 0.8],
  rayWidth = 0.002,
  cursorSize = 0.01,
  cursorColor = [1, 1, 1, 1],
  onRayEnter,
  onRayExit,
  onRaySelect,
  onRaySelectEnd,
  onRayMove
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const [state, setState] = (0, _react.useState)({
    isHovered: false,
    isSelected: false,
    interactionType: null,
    hitPoint: null,
    hitNormal: null
  });
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
  const handleRayEnter = (0, _react.useCallback)(hand => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      isHovered: true,
      interactionType: 'ray'
    }));
    onRayEnter?.(hand);
  }, [enabled, onRayEnter]);
  const handleRayExit = (0, _react.useCallback)(hand => {
    setState(prev => ({
      ...prev,
      isHovered: false,
      interactionType: null,
      hitPoint: null
    }));
    onRayExit?.(hand);
  }, [onRayExit]);
  const handleRaySelect = (0, _react.useCallback)((hand, point, normal) => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      isSelected: true,
      hitPoint: point,
      hitNormal: normal
    }));
    onRaySelect?.(hand, point, normal);
  }, [enabled, onRaySelect]);
  const handleRaySelectEnd = (0, _react.useCallback)(hand => {
    setState(prev => ({
      ...prev,
      isSelected: false
    }));
    onRaySelectEnd?.(hand);
  }, [onRaySelectEnd]);
  const handleRayMove = (0, _react.useCallback)((hand, point) => {
    if (!enabled) return;
    setState(prev => ({
      ...prev,
      hitPoint: point
    }));
    onRayMove?.(hand, point);
  }, [enabled, onRayMove]);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(InteractionContext.Provider, {
    value: state,
    children: children
  });
}
XRRayInteraction.displayName = 'XRRayInteraction';
//# sourceMappingURL=XRInteraction.js.map
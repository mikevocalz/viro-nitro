"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.ScrollViewer = ScrollViewer;
exports.XRScrollView = XRScrollView;
var _react = _interopRequireWildcard(require("react"));
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _types = require("./types");
var _jsxRuntime = require("react/jsx-runtime");
function _interopRequireWildcard(e, t) { if ("function" == typeof WeakMap) var r = new WeakMap(), n = new WeakMap(); return (_interopRequireWildcard = function (e, t) { if (!t && e && e.__esModule) return e; var o, i, f = { __proto__: null, default: e }; if (null === e || "object" != typeof e && "function" != typeof e) return f; if (o = t ? n : r) { if (o.has(e)) return o.get(e); o.set(e, f); } for (const t in e) "default" !== t && {}.hasOwnProperty.call(e, t) && ((i = (o = Object.defineProperty) && Object.getOwnPropertyDescriptor(e, t)) && (i.get || i.set) ? o(f, t, i) : f[t] = e[t]); return f; })(e, t); }
/**
 * XRScrollView Props
 */

/**
 * XRScrollView - Scrollable container for content
 * 
 * Supports horizontal, vertical, or both directions.
 * Works with near interaction (drag) and controllers (thumbstick).
 * 
 * @example
 * ```tsx
 * <XRScrollView width={0.4} height={0.3} contentHeight={1.0}>
 *   <XRStack direction="vertical" spacing={0.02}>
 *     {items.map(item => <XRButton key={item.id}>{item.label}</XRButton>)}
 *   </XRStack>
 * </XRScrollView>
 * ```
 */
function XRScrollView({
  position,
  rotation,
  scale,
  visible = true,
  opacity = 1,
  children,
  onMount,
  onUnmount,
  disabled = false,
  width = 0.4,
  height = 0.3,
  contentWidth,
  contentHeight,
  direction = 'vertical',
  showScrollbar = true,
  scrollbarWidth = 0.005,
  bounces = true,
  pagingEnabled = false,
  snapToInterval,
  onScroll,
  onScrollEnd
}) {
  const parentHandle = (0, _XRPanel.useParentPanel)();
  const [scrollOffset, setScrollOffset] = (0, _react.useState)({
    x: 0,
    y: 0
  });
  const [isDragging, setIsDragging] = (0, _react.useState)(false);
  const velocityRef = (0, _react.useRef)({
    x: 0,
    y: 0
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
  const effectiveContentWidth = contentWidth ?? width;
  const effectiveContentHeight = contentHeight ?? height;
  const maxScrollX = Math.max(0, effectiveContentWidth - width);
  const maxScrollY = Math.max(0, effectiveContentHeight - height);
  const canScrollX = direction === 'horizontal' || direction === 'both';
  const canScrollY = direction === 'vertical' || direction === 'both';
  const handleScroll = (0, _react.useCallback)((deltaX, deltaY) => {
    if (disabled) return;
    setScrollOffset(prev => {
      let newX = prev.x;
      let newY = prev.y;
      if (canScrollX) {
        newX = Math.max(0, Math.min(maxScrollX, prev.x + deltaX));
      }
      if (canScrollY) {
        newY = Math.max(0, Math.min(maxScrollY, prev.y + deltaY));
      }
      const newOffset = {
        x: newX,
        y: newY
      };
      onScroll?.(newOffset);
      return newOffset;
    });
  }, [disabled, canScrollX, canScrollY, maxScrollX, maxScrollY, onScroll]);
  const handleDragStart = (0, _react.useCallback)(() => {
    if (disabled) return;
    setIsDragging(true);
  }, [disabled]);
  const handleDragEnd = (0, _react.useCallback)(() => {
    setIsDragging(false);
    onScrollEnd?.(scrollOffset);

    // Apply snapping if enabled
    if (snapToInterval) {
      setScrollOffset(prev => ({
        x: Math.round(prev.x / snapToInterval) * snapToInterval,
        y: Math.round(prev.y / snapToInterval) * snapToInterval
      }));
    }
  }, [scrollOffset, onScrollEnd, snapToInterval]);

  // Calculate content position based on scroll
  const contentPosition = [canScrollX ? scrollOffset.x : 0, canScrollY ? -scrollOffset.y : 0, 0];

  // Scrollbar calculations
  const scrollbarTrackColor = _types.XRColors.border;
  const scrollbarThumbColor = _types.XRColors.secondary;
  const verticalScrollbarHeight = height * (height / effectiveContentHeight);
  const verticalScrollbarOffset = scrollOffset.y / maxScrollY * (height - verticalScrollbarHeight);
  const horizontalScrollbarWidth = width * (width / effectiveContentWidth);
  const horizontalScrollbarOffset = scrollOffset.x / maxScrollX * (width - horizontalScrollbarWidth);
  if (!isReady || !handle) {
    return null;
  }
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(_jsxRuntime.Fragment, {
    children: _react.Children.map(children, (child, index) => {
      if (! /*#__PURE__*/(0, _react.isValidElement)(child)) return child;
      return /*#__PURE__*/(0, _react.cloneElement)(child, {
        key: index,
        position: contentPosition
      });
    })
  });
}
XRScrollView.displayName = 'XRScrollView';

/**
 * ScrollViewer - Alias for XRScrollView with more desktop-like defaults
 */
function ScrollViewer(props) {
  return /*#__PURE__*/(0, _jsxRuntime.jsx)(XRScrollView, {
    ...props,
    showScrollbar: props.showScrollbar ?? true,
    bounces: props.bounces ?? false
  });
}
ScrollViewer.displayName = 'ScrollViewer';
//# sourceMappingURL=XRScrollView.js.map
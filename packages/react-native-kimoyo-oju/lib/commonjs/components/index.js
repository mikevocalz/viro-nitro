"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
var _exportNames = {
  useXRNode: true,
  useSubmitCommands: true,
  XRPanel: true,
  XRPanelContext: true,
  useParentPanel: true,
  XRStack: true,
  XRHStack: true,
  XRVStack: true,
  XRZStack: true,
  XRGrid: true,
  XRText: true,
  XRHeading: true,
  XRLabel: true,
  XRCylinder: true,
  XRCurvedPanel: true,
  XRVideo: true,
  XRButton: true,
  XRIconButton: true,
  XRSlider: true,
  XRRangeSlider: true,
  XRCheckbox: true,
  XRSwitch: true,
  XRRadio: true,
  XRScrollView: true,
  ScrollViewer: true,
  XRNearInteraction: true,
  XRRayInteraction: true,
  useInteraction: true,
  XRHUD: true,
  XRTooltip: true,
  XRNotification: true
};
Object.defineProperty(exports, "ScrollViewer", {
  enumerable: true,
  get: function () {
    return _XRScrollView.ScrollViewer;
  }
});
Object.defineProperty(exports, "XRButton", {
  enumerable: true,
  get: function () {
    return _XRButton.XRButton;
  }
});
Object.defineProperty(exports, "XRCheckbox", {
  enumerable: true,
  get: function () {
    return _XRCheckbox.XRCheckbox;
  }
});
Object.defineProperty(exports, "XRCurvedPanel", {
  enumerable: true,
  get: function () {
    return _XRCylinder.XRCurvedPanel;
  }
});
Object.defineProperty(exports, "XRCylinder", {
  enumerable: true,
  get: function () {
    return _XRCylinder.XRCylinder;
  }
});
Object.defineProperty(exports, "XRGrid", {
  enumerable: true,
  get: function () {
    return _XRGrid.XRGrid;
  }
});
Object.defineProperty(exports, "XRHStack", {
  enumerable: true,
  get: function () {
    return _XRStack.XRHStack;
  }
});
Object.defineProperty(exports, "XRHUD", {
  enumerable: true,
  get: function () {
    return _XRHUD.XRHUD;
  }
});
Object.defineProperty(exports, "XRHeading", {
  enumerable: true,
  get: function () {
    return _XRText.XRHeading;
  }
});
Object.defineProperty(exports, "XRIconButton", {
  enumerable: true,
  get: function () {
    return _XRButton.XRIconButton;
  }
});
Object.defineProperty(exports, "XRLabel", {
  enumerable: true,
  get: function () {
    return _XRText.XRLabel;
  }
});
Object.defineProperty(exports, "XRNearInteraction", {
  enumerable: true,
  get: function () {
    return _XRInteraction.XRNearInteraction;
  }
});
Object.defineProperty(exports, "XRNotification", {
  enumerable: true,
  get: function () {
    return _XRHUD.XRNotification;
  }
});
Object.defineProperty(exports, "XRPanel", {
  enumerable: true,
  get: function () {
    return _XRPanel.XRPanel;
  }
});
Object.defineProperty(exports, "XRPanelContext", {
  enumerable: true,
  get: function () {
    return _XRPanel.XRPanelContext;
  }
});
Object.defineProperty(exports, "XRRadio", {
  enumerable: true,
  get: function () {
    return _XRCheckbox.XRRadio;
  }
});
Object.defineProperty(exports, "XRRangeSlider", {
  enumerable: true,
  get: function () {
    return _XRSlider.XRRangeSlider;
  }
});
Object.defineProperty(exports, "XRRayInteraction", {
  enumerable: true,
  get: function () {
    return _XRInteraction.XRRayInteraction;
  }
});
Object.defineProperty(exports, "XRScrollView", {
  enumerable: true,
  get: function () {
    return _XRScrollView.XRScrollView;
  }
});
Object.defineProperty(exports, "XRSlider", {
  enumerable: true,
  get: function () {
    return _XRSlider.XRSlider;
  }
});
Object.defineProperty(exports, "XRStack", {
  enumerable: true,
  get: function () {
    return _XRStack.XRStack;
  }
});
Object.defineProperty(exports, "XRSwitch", {
  enumerable: true,
  get: function () {
    return _XRCheckbox.XRSwitch;
  }
});
Object.defineProperty(exports, "XRText", {
  enumerable: true,
  get: function () {
    return _XRText.XRText;
  }
});
Object.defineProperty(exports, "XRTooltip", {
  enumerable: true,
  get: function () {
    return _XRHUD.XRTooltip;
  }
});
Object.defineProperty(exports, "XRVStack", {
  enumerable: true,
  get: function () {
    return _XRStack.XRVStack;
  }
});
Object.defineProperty(exports, "XRVideo", {
  enumerable: true,
  get: function () {
    return _XRVideo.XRVideo;
  }
});
Object.defineProperty(exports, "XRZStack", {
  enumerable: true,
  get: function () {
    return _XRStack.XRZStack;
  }
});
Object.defineProperty(exports, "useInteraction", {
  enumerable: true,
  get: function () {
    return _XRInteraction.useInteraction;
  }
});
Object.defineProperty(exports, "useParentPanel", {
  enumerable: true,
  get: function () {
    return _XRPanel.useParentPanel;
  }
});
Object.defineProperty(exports, "useSubmitCommands", {
  enumerable: true,
  get: function () {
    return _useXRNode.useSubmitCommands;
  }
});
Object.defineProperty(exports, "useXRNode", {
  enumerable: true,
  get: function () {
    return _useXRNode.useXRNode;
  }
});
var _types = require("./types");
Object.keys(_types).forEach(function (key) {
  if (key === "default" || key === "__esModule") return;
  if (Object.prototype.hasOwnProperty.call(_exportNames, key)) return;
  if (key in exports && exports[key] === _types[key]) return;
  Object.defineProperty(exports, key, {
    enumerable: true,
    get: function () {
      return _types[key];
    }
  });
});
var _useXRNode = require("./useXRNode");
var _XRPanel = require("./XRPanel");
var _XRStack = require("./XRStack");
var _XRGrid = require("./XRGrid");
var _XRText = require("./XRText");
var _XRCylinder = require("./XRCylinder");
var _XRVideo = require("./XRVideo");
var _XRButton = require("./XRButton");
var _XRSlider = require("./XRSlider");
var _XRCheckbox = require("./XRCheckbox");
var _XRScrollView = require("./XRScrollView");
var _XRInteraction = require("./XRInteraction");
var _XRHUD = require("./XRHUD");
//# sourceMappingURL=index.js.map
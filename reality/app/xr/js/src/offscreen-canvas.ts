// Copyright (c) 2023 Niantic Labs, Inc.
// Original Author: Nathan Waters (nathanwaters@nianticlabs.com)

// TODO(nathan): remove once we've upgraded typescript.
declare global {
  interface Window {
    OffscreenCanvas?: any
  }
}
declare const OffscreenCanvas: any

enum OffscreenCanvasAvailable {
  No,
  Webgl1,
  Webgl2,
}

// https://caniuse.com/offscreencanvas
// OffscreenCanvas with WebGl context is not available on Safari until version 17 and iOS Safari 17.
// Determine if this browser can get an OffscreenCanvas with access to Webgl / Webgl2 context
// TODO(dat): We create then destroy the WebGlContext on detection here. Can we do better?
const hasValidOffscreenCanvas = (): OffscreenCanvasAvailable => {
  const hasOffscreenCanvas = window.OffscreenCanvas !== undefined
  if (!hasOffscreenCanvas) {
    return OffscreenCanvasAvailable.No
  }

  try {
    const offscreenCanvas = new OffscreenCanvas(0, 0)
    if (offscreenCanvas.getContext('webgl2')) {
      return OffscreenCanvasAvailable.Webgl2
    }

    if (offscreenCanvas.getContext('webgl')) {
      return OffscreenCanvasAvailable.Webgl1
    }
  } catch {
    // do nothing
  }

  return OffscreenCanvasAvailable.No
}

export {
  hasValidOffscreenCanvas,
  OffscreenCanvasAvailable,
}

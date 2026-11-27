/* globals AFRAME */

import {isXrScene} from './xr-scene'

// Delays the start of an A-Frame scene until the 8th Wall engine (XR8) has loaded.
//
// xr.js is usually loaded asynchronously. When it loads, it registers its A-Frame components
// (xrweb, xrface, xrlayers, xrconfig, ...) on window.AFRAME by itself. A-Frame initializes the
// components of the scene when the scene loads, and the scene only loads after all its child nodes
// have loaded. The <xrextras-wait-for-xr8> node emits `loaded` only once XR8 is available, so as a
// child of <a-scene> it guarantees that the XR components are registered before the scene
// initializes its components and starts rendering.
//
// The xrextras-wait-for-xr8 system adds this node automatically to a scene that uses xrweb,
// xrface, xrlayers or xrconfig, so a scene works with stock A-Frame without changes. The node can
// also be added explicitly in the HTML:
//
//   <a-scene xrweb>
//     <xrextras-wait-for-xr8></xrextras-wait-for-xr8>
//     ...
//   </a-scene>

const WAIT_FOR_XR8_TAG = 'xrextras-wait-for-xr8'

const defineWaitForXr8Element = () => {
  if (customElements.get(WAIT_FOR_XR8_TAG)) {
    return true
  }

  // A-Frame nodes are classes that can be extended (requires A-Frame 1.8 or later).
  const {ANode} = window.AFRAME
  if (typeof ANode !== 'function' || !(ANode.prototype instanceof HTMLElement)) {
    return false
  }

  class WaitForXr8 extends ANode {
    doConnectedCallback() {
      super.doConnectedCallback()
      const onxrloaded = () => this.load()
      // eslint-disable-next-line @typescript-eslint/no-unused-expressions
      window.XR8 ? onxrloaded() : window.addEventListener('xrloaded', onxrloaded, {once: true})
    }
  }

  customElements.define(WAIT_FOR_XR8_TAG, WaitForXr8)
  return true
}

const waitForXr8System = () => ({
  init() {
    const scene = this.el
    if (window.XR8) {
      return
    }

    if (!isXrScene(scene)) {
      return
    }

    const alreadyAdded = Array.from(scene.children)
      .some(child => child.tagName.toLowerCase() === WAIT_FOR_XR8_TAG)
    if (alreadyAdded) {
      return
    }

    scene.appendChild(document.createElement(WAIT_FOR_XR8_TAG))
  },
})

const registerWaitForXr8 = () => {
  if (!defineWaitForXr8Element()) {
    return
  }
  AFRAME.registerSystem(WAIT_FOR_XR8_TAG, waitForXr8System())
}

export {
  registerWaitForXr8,
}

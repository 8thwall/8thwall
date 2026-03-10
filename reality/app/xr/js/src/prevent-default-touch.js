// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

// Utility methods for calling `preventDefault` on touch events to stop the iOS 15 Safari floating
// address bar from popping up & down on tap.

/* global XR8:readonly */

const isiOS15OrLater = () => XR8.XrDevice.deviceEstimate().os === 'iOS' &&
    parseInt(XR8.XrDevice.deviceEstimate().osVersion, 10) >= 15

const dispatchClickFromTouchEvent = (evt) => {
  const mouseEvt = new MouseEvent('click', {
    bubbles: evt.bubbles,
    cancelable: evt.cancelable,
    clientX: evt.clientX ? evt.changedTouches[0].clientX : evt.pageX,
    clientY: evt.clientY ? evt.changedTouches[0].clientY : evt.pageY,
    ctrlKey: evt.ctrlKey,
    layerX: evt.layerX,
    layerY: evt.layerY,
    offsetX: evt.offsetX,
    offsetY: evt.offsetY,
    pageX: evt.changedTouches.length ? evt.changedTouches[0].pageX : evt.pageX,
    pageY: evt.changedTouches.length ? evt.changedTouches[0].pageY : evt.pageY,
    screenX: evt.screenX ? evt.changedTouches[0].screenX : evt.pageX,
    screenY: evt.screenY ? evt.changedTouches[0].screenY : evt.pageY,
    shiftKey: evt.shiftKey,
  })
  evt.target.dispatchEvent(mouseEvt)
}

const preventDefault = (evt) => {
  evt.preventDefault()
  // preventDefault() stops the normal click event after touchend. Fire it manually.
  if (evt.type === 'touchend' && evt.defaultPrevented) {
    dispatchClickFromTouchEvent(evt)
  }
}

const attachPreventDefaultListener = (evt) => {
  // evt.currentTarget will be the canvas.
  evt.currentTarget.removeEventListener('touchend', attachPreventDefaultListener)

  evt.currentTarget.addEventListener('touchmove', preventDefault)
  evt.currentTarget.addEventListener('touchend', preventDefault)
  evt.currentTarget.addEventListener('touchstart', preventDefault)
}

// If we're running iOS 15 and in Safari, call preventDefault() on touch events to prevent the
// floating address bar from popping up.
const maybeAddPreventDefaultTouchListeners = (canvas) => {
  if (!isiOS15OrLater() || XR8.XrDevice.deviceEstimate().browser.name !== 'Mobile Safari') {
    return
  }

  // Add `preventDefault()` calls to touch events to prevent the floating address bar from expanding
  // on tap. We first check if the bar is expanded with safe-area-inset-bottom. If so we allow the
  // first tap to minimize it, then prevent it from being brought up subsequently.
  const body = document.getElementsByTagName('body')[0]
  body.style.setProperty('--safe-area-inset-bottom', 'env(safe-area-inset-bottom)')
  const safeAreaInsetBottom = getComputedStyle(body).getPropertyValue('--safe-area-inset-bottom')

  // Sometimes the address bar is moving and returns something small (i.e. 2px or 7px). When the bar
  // is expanded it's ~112px, so assume if <10px then we're currently moving but will end up
  // minimized.
  const isUrlBarExpanded = safeAreaInsetBottom && parseInt(safeAreaInsetBottom, 10) > 10
  if (isUrlBarExpanded) {
    canvas.addEventListener('touchend', attachPreventDefaultListener)
  } else {
    canvas.addEventListener('touchmove', preventDefault, false)
    canvas.addEventListener('touchend', preventDefault, false)
    canvas.addEventListener('touchstart', preventDefault, false)
  }
  // Always prevent mousedown to stop double taps from popping address bar back up again.
  canvas.addEventListener('mousedown', preventDefault, false)
}

// Cleans up event listeners added by maybeAddPreventDefaultTouchListeners().
const cleanupPreventDefaultTouchListeners = (canvas) => {
  canvas.removeEventListener('touchend', attachPreventDefaultListener)

  canvas.removeEventListener('touchmove', preventDefault, false)
  canvas.removeEventListener('touchend', preventDefault, false)
  canvas.removeEventListener('touchstart', preventDefault, false)
  canvas.removeEventListener('mousedown', preventDefault, false)
}

export {
  maybeAddPreventDefaultTouchListeners,
  cleanupPreventDefaultTouchListeners,
}

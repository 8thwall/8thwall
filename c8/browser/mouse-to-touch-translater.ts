import {createInteractionProvider} from './interaction-provider'

const createMouseToTouchTranslator = () => {
  const interactionProvider_ = createInteractionProvider()

  let attached_ = false
  let canvas_ = null

  const dragState_ = {
    mouseMoved: false,    // Whether or not we have moved from the first pointer position.
    topPointer: null,     // The main pointer, true location of the touch / move.
    bottomPointer: null,  // If this is a two-finger gesture, bottomPointer is 1px below top.
  }

  const scrollState_ = {
    scrolling: false,              // Whether or not we're currently scrolling.
    lastEvent: performance.now(),  // Time of the last scroll wheel event.
    deltaSum: 0,                   // Accumulated scroll delta since the start of scrolling.
    topPointer: null,              // A pointer representing a top touch.
    bottomPointer: null,           // A pointer representing a bottom touch.
    ctrlDown: false,               // Whether or not the control key is currently pressed.
  }

  const clip = (v, min, max) => Math.min(Math.max(v, min), max)

  const mouseDown = (event) => {
    // Bona-fide browser events vs CustomEvent dispatched by aframe.
    if (!(event instanceof MouseEvent)) {
      return
    }
    if (event.altKey) {
      // Ignore alt click events.
      return
    }
    // One pointer at a time.
    // NOTE(pawel) Perhaps cancel the previous pointer and start anew?
    if (dragState_.topPointer) {
      return
    }
    dragState_.topPointer = interactionProvider_.startPointer({
      x: event.clientX,
      y: event.clientY,
    })
    // If the option key is pressed when the mouse drag starts, add a second pointer just below the
    // first one. This will persist for the remainder of the touch, regardless of the option key
    // press state.
    if (event.ctrlKey || event.button === 2) {
      dragState_.bottomPointer = interactionProvider_.startPointer({
        x: event.clientX,
        y: event.clientY + 1,
      })
    }
  }

  const preventNextClick = () => {
    const handler = (event) => {
      event.stopPropagation()
      window.removeEventListener('click', handler, {capture: true})
    }
    // Capture is top bottom, so prevent the click from reaching the canvas.
    window.addEventListener('click', handler, {capture: true})
  }

  const mouseUp = (event) => {
    // Bona-fide browser events vs CustomEvent dispatched by aframe.
    if (!(event instanceof MouseEvent)) {
      return
    }
    if (!dragState_.topPointer) {
      // If a drag action was started with a key modifier (like right click), we may not have a
      // matching event here. Just ignore.
      return
    }
    interactionProvider_.endPointer(dragState_.topPointer, {
      preventClick: true,
    })
    // If we're doing a two finger drag, end the second pointer.
    if (dragState_.bottomPointer) {
      interactionProvider_.endPointer(dragState_.bottomPointer, {
        preventClick: true,
      })
    }
    dragState_.topPointer = null
    dragState_.bottomPointer = null
    if (dragState_.mouseMoved) {
      preventNextClick()
    }
    dragState_.mouseMoved = false
  }

  const mouseMove = (event) => {
    // Bona-fide browser events vs CustomEvent dispatched by aframe.
    if (!(event instanceof MouseEvent)) {
      return
    }
    if (!dragState_.topPointer) {
      // If a drag action was started with a key modifier (like right click), we may not have a
      // matching event here. Just ignore.
      return
    }
    // If no button is pressed, we missed an earlier mouse up event (maybe the mouse went up off the
    // screen). So, we're done.
    if (!event.buttons) {
      mouseUp(event)
      return
    }
    // Only emit touchmove events if the movement is above a certain threshold.
    const isMove = Math.abs(event.movementX) >= 1 || Math.abs(event.movementY) >= 1
    if (!isMove) {
      return
    }
    interactionProvider_.updatePointerPosition(dragState_.topPointer, {
      x: event.clientX,
      y: event.clientY,
    })
    // If we're doing a two finger drag, keep the second pointer 1px below the first.
    if (dragState_.bottomPointer) {
      interactionProvider_.updatePointerPosition(dragState_.bottomPointer, {
        x: event.clientX,
        y: event.clientY + 1,
      })
    }
    interactionProvider_.flushMoveEvents()
    dragState_.mouseMoved = true
  }

  const MAX_ZOOM = 10  // The most we can zoom in a single wheel scroll.
  const ZOOM_SPEED = 1  // How much continuous scrolling is required to get to max zoom.

  const startWheelZoom = (event: WheelEvent) => {
    const {x: mouseX, y: mouseY} = event

    // Start two pointers centered at the x mouse position and above/below the y mouse position
    const baseDistance = canvas_.clientHeight / MAX_ZOOM

    // Clip the pointers to fit in the canvas
    const y1 = clip(
      mouseY - 0.5 * baseDistance,
      0,
      canvas_.clientHeight - baseDistance
    )
    const y2 = clip(
      mouseY + 0.5 * baseDistance,
      baseDistance,  // enforce minimum 1px distance.
      canvas_.clientHeight
    )

    // Store pointers on the state.
    scrollState_.topPointer =
      interactionProvider_.startPointer({x: mouseX, y: canvas_.clientTop + y1})
    scrollState_.bottomPointer =
      interactionProvider_.startPointer({x: mouseX, y: canvas_.clientTop + y2})
  }

  const moveWheelZoom = (event: WheelEvent) => {
    const {clientX: mouseX, clientY: mouseY} = event

    // Update pointers to move in the direction of desired zoom
    const scrollDelta = scrollState_.deltaSum / canvas_.clientHeight
    const scrollRange = 2 / (1 + Math.exp(-scrollDelta * ZOOM_SPEED)) - 1  // in [-1, 1]
    const desiredZoom = Math.exp(scrollRange * Math.log(MAX_ZOOM))  // in [1 / MAX_ZOOM, MAX_ZOOM]

    const baseDistance = canvas_.clientHeight / MAX_ZOOM
    const pointerDistance = baseDistance * desiredZoom

    // Clip the pointers to fit in the canvas
    // The max they can be apart is the full canvas height.
    // The min they can be apart is 1 pixel at the center of the image.
    const y1 = clip(
      mouseY - 0.5 * pointerDistance,
      0,
      canvas_.clientHeight - pointerDistance
    )
    const y2 = clip(
      mouseY + 0.5 * pointerDistance,
      pointerDistance,  // enforce minimum 1px distance.
      canvas_.clientHeight
    )
    interactionProvider_.updatePointerPosition(
      scrollState_.topPointer, {x: mouseX, y: canvas_.clientTop + y1}
    )
    interactionProvider_.updatePointerPosition(
      scrollState_.bottomPointer, {x: mouseX, y: canvas_.clientTop + y2}
    )
    interactionProvider_.flushMoveEvents()
  }

  const endWheelZoom = () => {
    // Stop this scroll interaction.
    interactionProvider_.endPointer(scrollState_.topPointer)
    interactionProvider_.endPointer(scrollState_.bottomPointer)
    // Clear pointers on the state.
    scrollState_.topPointer = null
    scrollState_.bottomPointer = null
  }

  const pollWheelUpdates = (event: WheelEvent) => {
    // If it's been <100ms since our last wheel event, keep moving.
    if (performance.now() - scrollState_.lastEvent < 100) {
      moveWheelZoom(event)  // Emit move events.
      requestAnimationFrame(() => pollWheelUpdates(event))  // Keep checking for moves.
      return
    }
    // No updates in a little while, time to end. Don't keep checking for moves.
    endWheelZoom()
    scrollState_.scrolling = false
    scrollState_.deltaSum = 0
  }

  const wheel = (event) => {
    if (!scrollState_.scrolling) {
      startWheelZoom(event)
      requestAnimationFrame(() => pollWheelUpdates(event))
    }
    // Update scroll state. This is the same for start and move. moveWheelZoom will be called
    // in pollWheelUpdates.
    scrollState_.scrolling = true
    scrollState_.lastEvent = performance.now()

    // Pinch to zoom gesture is emulated as a wheel event with ctrl key pressed.
    // There doesn't seem to be a reliable way to differentiate from a bona fide ctrl key pressed.
    // How this came to be: https://bugzilla.mozilla.org/show_bug.cgi?id=1052253
    // Listen for an explicit ctrl key press while scrolling to determine if the scroll
    // should be reversed to prevent pinch to zoom gesture from being reversed.
    if (scrollState_.ctrlDown) {
      scrollState_.deltaSum += event.deltaY
    } else {
      scrollState_.deltaSum -= event.deltaY
    }

    event.preventDefault()
  }

  const preventDefault = (e => e.preventDefault())

  const keydown = (event: KeyboardEvent) => {
    if (event.key === 'Control') {
      scrollState_.ctrlDown = true
    }
  }

  const keyup = (event: KeyboardEvent) => {
    if (event.key === 'Control') {
      scrollState_.ctrlDown = false
    }
  }

  const attach = ({canvas}) => {
    if (attached_) {
      throw new Error('[xr][mouse-to-touch-translator] Already attached.')
    }
    canvas_ = canvas
    // TODO: `window` here eats click events on buttons rendering them buggy. Fix this.
    window.addEventListener('mousedown', mouseDown)
    window.addEventListener('mouseup', mouseUp)
    window.addEventListener('mousemove', mouseMove)
    window.addEventListener('contextmenu', preventDefault)
    window.addEventListener('keydown', keydown)
    window.addEventListener('keyup', keyup)
    canvas_.addEventListener('wheel', wheel)
    attached_ = true
  }

  const detach = () => {
    if (!attached_) {
      throw new Error('[xr][mouse-to-touch-translator] Not attached.')
    }
    // TODO: `window` here eats click events on buttons rendering them buggy. Fix this.
    window.removeEventListener('mousedown', mouseDown)
    window.removeEventListener('mouseup', mouseUp)
    window.removeEventListener('mousemove', mouseMove)
    window.removeEventListener('contextmenu', preventDefault)
    window.removeEventListener('keydown', keydown)
    window.removeEventListener('keyup', keyup)
    canvas_.removeEventListener('wheel', wheel)
    attached_ = false
  }

  return {
    attach,
    detach,
  }
}

export {
  createMouseToTouchTranslator,
}

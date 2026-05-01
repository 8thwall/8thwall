// NOTE(christoph): Duplicated from:
//  reality/app/xr/js/src/mouse-to-touch-translator.ts
import {createCanvasPinch} from './canvas-pinch'
import {createInteractionProvider} from './interaction-provider'

type DragState = {
  mouseMoved: boolean
  topPointer: null | number
  bottomPointer: null | number
}

const createMouseToTouchTranslator = () => {
  const interactionProvider_ = createInteractionProvider()

  const pinchScroll = createCanvasPinch(interactionProvider_)

  let attached_ = false

  const dragState_: DragState = {
    mouseMoved: false,    // Whether or not we have moved from the first pointer position.
    topPointer: null,     // The main pointer, true location of the touch / move.
    bottomPointer: null,  // If this is a two-finger gesture, bottomPointer is 1px below top.
  }

  const mouseDown = (event) => {
    // Bona-fide browser events vs CustomEvent dispatched by aframe.
    if (!(event instanceof MouseEvent)) {
      return
    }
    // Ignore right-click events.
    if (event.ctrlKey || event.button === 2) {
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
    if (event.altKey) {
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
    const isMove = Math.abs(event.movementX) > 1 || Math.abs(event.movementY) > 1
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

  const attach = () => {
    if (attached_) {
      return
    }
    pinchScroll.attach()
    // TODO: `window` here eats click events on buttons rendering them buggy. Fix this.
    window.addEventListener('mousedown', mouseDown)
    window.addEventListener('mouseup', mouseUp)
    window.addEventListener('mousemove', mouseMove)
    attached_ = true
  }

  const detach = () => {
    if (!attached_) {
      return
    }
    pinchScroll.detach()
    // TODO: `window` here eats click events on buttons rendering them buggy. Fix this.
    window.removeEventListener('mousedown', mouseDown)
    window.removeEventListener('mouseup', mouseUp)
    window.removeEventListener('mousemove', mouseMove)
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

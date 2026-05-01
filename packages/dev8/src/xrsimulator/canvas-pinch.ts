// NOTE(christoph): Adapted from:
//  reality/app/xr/js/src/mouse-to-touch-translator.ts

import type {InteractionProvider} from './interaction-provider'

const PINCH_END_DELAY_MS = 100

const createCanvasPinch = (interactionProvider: InteractionProvider) => {
  let endPinchTimeout_: ReturnType<typeof setTimeout> | null
  let activeCanvas_: HTMLCanvasElement | null
  let topPointer_: number | null
  let bottomPointer_: number | null
  let deltaSum_: number = 0

  const endPinch = () => {
    clearTimeout(endPinchTimeout_!)
    if (topPointer_) {
      interactionProvider.endPointer(topPointer_, {preventClick: true})
      topPointer_ = null
    }

    if (bottomPointer_) {
      interactionProvider.endPointer(bottomPointer_, {preventClick: true})
      bottomPointer_ = null
    }
    activeCanvas_ = null
    deltaSum_ = 0
  }

  const delayPinchEnd = () => {
    clearTimeout(endPinchTimeout_!)
    endPinchTimeout_ = setTimeout(endPinch, PINCH_END_DELAY_MS)
  }

  const updatePinch = (canvas: Element) => {
    const centerX = canvas.clientLeft + canvas.clientWidth * 0.5
    const centerY = canvas.clientTop + canvas.clientHeight * 0.5

    const normalizedDelta = Math.max(-1, Math.min(1, (deltaSum_ / canvas.clientHeight)))

    const distance = (0.25 + normalizedDelta / 4) * canvas.clientHeight

    const x = centerX
    const y1 = centerY - distance
    const y2 = centerY + distance + 1

    interactionProvider.updatePointerPosition(topPointer_, {x, y: y1})
    interactionProvider.updatePointerPosition(bottomPointer_, {x, y: y2})
    interactionProvider.flushMoveEvents()

    delayPinchEnd()
  }

  const startPinch = (target: HTMLCanvasElement) => {
    const x = target.clientLeft + target.clientWidth * 0.5
    const y1 = target.clientTop + target.clientHeight * 0.25
    const y2 = target.clientTop + target.clientHeight * 0.75

    activeCanvas_ = target
    deltaSum_ = 0
    topPointer_ = interactionProvider.startPointer({x, y: y1}, target)
    bottomPointer_ = interactionProvider.startPointer({x, y: y2}, target)
    delayPinchEnd()
  }

  const handleWheelEvent = (event: WheelEvent) => {
    if (!(event.target instanceof HTMLCanvasElement)) {
      return
    }

    event.preventDefault()

    if (activeCanvas_) {
      // Pinch to zoom gesture is emulated as a wheel event with ctrl key pressed.
      // There doesn't seem to be a reliable way to differentiate from a bona fide ctrl key pressed.
      // How this came to be: https://bugzilla.mozilla.org/show_bug.cgi?id=1052253
      if (event.ctrlKey) {
        deltaSum_ -= event.deltaY
      } else {
        deltaSum_ += event.deltaY
      }
      updatePinch(activeCanvas_)
    } else {
      startPinch(event.target)
    }
  }

  const attach = () => {
    window.addEventListener('wheel', handleWheelEvent, {passive: false})
  }

  const detach = () => {
    window.removeEventListener('wheel', handleWheelEvent)
    endPinch()
  }

  return {
    attach,
    detach,
  }
}

export {
  createCanvasPinch,
}

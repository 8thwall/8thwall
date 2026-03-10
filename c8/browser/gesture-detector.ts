import BROWSERWASM from '@repo/c8/browser/browser-wasm'

import {writeStringToEmscriptenHeap} from '@repo/c8/ems/ems'
import {createMouseToTouchTranslator} from './mouse-to-touch-translater'

interface GestureState {
  touchCount: number
  positionRaw: {x: number, y: number}
  position: {x: number, y: number}
  velocity: {x: number, y: number}
  positionClip: {x: number, y: number}
  normalizedViewSize: {x: number, y: number}
  spread: number
  touchMillis: number
  startMillis: number
  startPosition: {x: number, y: number}
  startSpread: number
  positionChange: {x: number, y: number}
  spreadChange: number
  startRotationRadian: number
  rotationRadianChange: number
}

type GestureEvent = 'onefinterstart'
  | 'onefingermove'
  | 'onefingerend'
  | 'onefingertap'
  | 'onefingerdoubletap'
  | 'twofingerstart'
  | 'twofingermove'
  | 'twofingerend'
  | 'twofingertap'
  | 'twofingerdoubletap'
  | 'threefingerstart'
  | 'threefingermove'
  | 'threefingerend'
  | 'threefingertap'
  | 'threefingerdoubletap'
  | 'manyfingerstart'
  | 'manyfingermove'
  | 'manyfingerend'
  | 'manyfingertap'
  | 'manyfingerdoubletap'

interface GestureResponse {
  event: GestureEvent
  id: string
  state: GestureState
}

type GestureListener = (event: GestureEvent, state: GestureState) => void

let browserWasm: any = null
const wasmPromise = BROWSERWASM({}).then((module) => {
  browserWasm = module
})

const preventDefault = (e) => {
  e.preventDefault()
  e.stopPropagation()
}

const gestureDetector = ({canvas}) => {
  let detector_: number = 0

  // Map from listener to {event: id}
  const listenerIds_ = new Map<GestureListener, Partial<Record<GestureEvent, string>>>()
  const idListeners_ = new Map<string, GestureListener>()  // Map from id to listener
  const mtt_ = createMouseToTouchTranslator()

  const emitGestureEvent = (e) => {
    preventDefault(e)
    const touches = e.touches || e.detail.touches
    // TODO: Android has something like:
    //     int countMod = a == MotionEvent.ACTION_UP || a == MotionEvent.ACTION_POINTER_UP ? -1 : 0;
    // Do we need that here?
    const touchPosition = t => ({
      x: t.clientX,
      y: t.clientY,
      rawX: t.clientX,
      rawY: t.clientY,
    })
    const touchEvent = {
      timeMillis: Date.now(),
      count: touches.length,
      pos: [...touches].map(touchPosition),
      screenWidth: window.innerWidth,
      screenHeight: window.innerHeight,
      viewWidth: canvas.clientWidth,
      viewHeight: canvas.clientHeight,
    }

    const touchJson = writeStringToEmscriptenHeap(browserWasm, JSON.stringify(touchEvent))
    browserWasm._c8EmAsm_observe(detector_, touchJson)
    browserWasm._free(touchJson)

    const result = JSON.parse((window as any)._browserWasm.events) as {events: GestureResponse[]}
    delete (window as any)._browserWasm

    result.events.forEach(({id, event, state}) => {
      const listener = idListeners_.get(id)
      if (listener) {
        listener(event, state)
      }
    })
  }

  let initialized = false
  const init = wasmPromise.then(() => {
    detector_ = browserWasm._c8EmAsm_createGestureDetector()
    mtt_.attach({canvas})
    canvas.addEventListener('touchstart', emitGestureEvent)
    canvas.addEventListener('touchend', emitGestureEvent)
    canvas.addEventListener('touchmove', emitGestureEvent)
    initialized = true
  })

  const destroy = () => {
    if (!initialized) {
      init.then(() => destroy())
      return
    }
    browserWasm._c8EmAsm_destroyGestureDetector(detector_)
    mtt_.detach()
  }

  const addListener = (event: GestureEvent, listener: GestureListener) => {
    if (!initialized) {
      init.then(() => addListener(event, listener))
      return
    }

    const eventString = writeStringToEmscriptenHeap(browserWasm, event)
    const id = browserWasm._c8EmAsm_addListener(detector_, eventString)
    browserWasm._free(eventString)

    if (!listenerIds_.has(listener)) {
      listenerIds_.set(listener, {})
    }

    listenerIds_.get(listener)![event] = id
    idListeners_.set(id, listener)
  }

  const removeListener = (event: GestureEvent, listener: GestureListener) => {
    if (!initialized) {
      init.then(() => removeListener(event, listener))
      return
    }

    const events = listenerIds_.get(listener) || {}

    const id = events[event]
    if (!id) {
      // eslint-disable-next-line no-console
      console.warn(`Unable to remove listener for event ${event}: not found`)
      return
    }
    const eventString = writeStringToEmscriptenHeap(browserWasm, event)
    browserWasm._c8EmAsm_removeListener(detector_, eventString, id)
    browserWasm._free(eventString)
    delete events[event]
    idListeners_.delete(id)
  }

  return {
    addListener,
    removeListener,
    destroy,
  }
}

export type {
  GestureEvent,
  GestureListener,
  GestureState,
}

export {
  gestureDetector,
}

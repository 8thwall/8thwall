let isDispatching = false

const dispatch = (el, event) => {
  if (isDispatching) {
    // Prevent recursive dispatches
    return
  }
  isDispatching = true
  el.dispatchEvent(event)
  isDispatching = false
}

const getElementAtPosition = (clientX, clientY) => (
  document.elementFromPoint(clientX, clientY)
)

const getTouchFromPointer = (pointer) => {
  const {target, identifier, clientX, clientY} = pointer

  const touch = new Touch({
    radiusX: 10,
    radiusY: 10,
    force: 1,
    screenX: clientX + window.screenLeft,
    screenY: clientY + window.screenTop,
    clientX,
    clientY,
    pageX: clientX + window.pageXOffset,
    pageY: clientY + window.pageYOffset,
    identifier,
    rotationAngle: 0,
    target,
  })

  return touch
}

const addToListMap = (map, key, element) => {
  if (!map.has(key)) {
    map.set(key, [element])
  } else {
    map.get(key).push(element)
  }
}

const replaceElementWithIdentifier = (list, newElement, identifier) => {
  const currentIndex = list.findIndex(e => e.identifier === identifier)
  list[currentIndex] = newElement
}

const removeElementByIdentifier = (list, identifier) => {
  list.splice(list.findIndex(t => t.identifier === identifier), 1)
}

const removeFromListMapByIdentifier = (map, key, identifier) => {
  const list = map.get(key)
  if (!list) {
    return
  }
  removeElementByIdentifier(list, identifier)
  if (list.length === 0) {
    map.delete(key)
  }
}

const createInteractionProvider = () => {
  const pointers_ = []
  const pointerMap_ = {}

  const activeTouches_ = []
  const activeTouchesByTarget_ = new Map()

  let pointerId_ = 1

  const startPointer = (position, targetOverride = null) => {
    const identifier = pointerId_++

    const target = targetOverride || getElementAtPosition(position.x, position.y) || document.body
    const pointer = {
      identifier,
      target,
      clientX: position.x,
      clientY: position.y,
      didMove: false,
      isMultiTouch: pointers_.length > 0,
    }

    if (pointers_.length === 1) {
      pointers_[0].isMultiTouch = true
    }

    const touch = getTouchFromPointer(pointer)

    pointerMap_[identifier] = pointer

    addToListMap(activeTouchesByTarget_, pointer.target, touch)
    pointers_.push(pointer)
    activeTouches_.push(touch)

    dispatch(target, new TouchEvent('touchstart', {
      view: window,
      bubbles: true,
      cancelable: true,
      changedTouches: [touch],
      touches: activeTouches_,
      targetTouches: activeTouchesByTarget_.get(target),
    }))

    return identifier
  }

  const updatePointerPosition = (identifier, position) => {
    const pointer = pointerMap_[identifier]

    const isNewPosition = pointer.clientX !== position.x ||
                          pointer.clientY !== position.y

    if (!isNewPosition) {
      return
    }

    pointer.clientX = position.x
    pointer.clientY = position.y
    pointer.didMove = true
  }

  /**
   *
   * @param {*} identifier Pointer identifier.
   * @param {*} opts {
   *  preventClick: boolean // Prevents click event from being dispatched.
   * }
   */
  const endPointer = (identifier, opts = {}) => {
    const pointer = pointerMap_[identifier]
    const {target} = pointer

    removeFromListMapByIdentifier(activeTouchesByTarget_, pointer.target, identifier)
    removeElementByIdentifier(activeTouches_, identifier)
    removeElementByIdentifier(pointers_, identifier)

    const endEvent = new TouchEvent('touchend', {
      view: window,
      bubbles: true,
      cancelable: true,
      changedTouches: [getTouchFromPointer(pointer)],
      touches: activeTouches_,
      targetTouches: activeTouchesByTarget_.get(target),
    })

    dispatch(target, endEvent)

    // If the touchend gets .preventDefault() called, don't emit mouse events
    // If the pointer was ever part of a multi-touch gesture, don't emit mouse events for it
    if (!opts.preventClick && !endEvent.defaultPrevented && !pointer.isMultiTouch) {
      const endTarget = getElementAtPosition(pointer.clientX, pointer.clientY)
      if (endTarget === pointer.target) {
        const clickEvent = new MouseEvent('click', {
          view: window,
          bubbles: true,
          cancelable: true,
          screenX: pointer.clientX + window.screenLeft,
          screenY: pointer.clientY + window.screenTop,
          clientX: pointer.clientX,
          clientY: pointer.clientY,
          button: 0,
          buttons: 1,
        })

        dispatch(target, clickEvent)
      }
    }
  }

  const cancelPointer = (identifier) => {
    const pointer = pointerMap_[identifier]
    const {target} = pointer

    removeFromListMapByIdentifier(activeTouchesByTarget_, pointer.target, identifier)
    removeElementByIdentifier(activeTouches_, identifier)
    removeElementByIdentifier(pointers_, identifier)

    dispatch(target, new TouchEvent('touchcancel', {
      view: window,
      bubbles: true,
      cancelable: true,
      changedTouches: [getTouchFromPointer(pointer)],
      touches: activeTouches_,
      targetTouches: activeTouchesByTarget_.get(target),
    }))
  }

  const flushMoveEvents = () => {
    if (!pointers_.length) {
      return
    }

    const movedByTarget = new Map()

    pointers_.forEach((pointer) => {
      const {target, identifier} = pointer
      if (pointer.didMove) {
        pointer.didMove = false

        // Update touch in active touch list
        const touch = getTouchFromPointer(pointer)

        replaceElementWithIdentifier(activeTouches_, touch, identifier)
        replaceElementWithIdentifier(activeTouchesByTarget_.get(target), touch, identifier)

        addToListMap(movedByTarget, target, touch)
      }
    })

    // For each target where a touch moved: dispatch an event for each one
    movedByTarget.forEach((changedTouches, target) => {
      const endEvent = new TouchEvent('touchmove', {
        view: window,
        bubbles: true,
        cancelable: true,
        changedTouches,
        touches: activeTouches_,
        targetTouches: activeTouchesByTarget_.get(target),
      })
      dispatch(target, endEvent)
    })
  }

  return {
    startPointer,
    updatePointerPosition,
    endPointer,
    cancelPointer,
    flushMoveEvents,
  }
}

export {
  createInteractionProvider,
}

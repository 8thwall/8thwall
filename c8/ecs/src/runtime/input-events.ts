import type {Events} from './events-types'
import type {InputListenerApi} from './input-types'

interface GamepadConnectedEvent {
  gamepad: Gamepad
}

interface GamepadDisconnectedEvent {
  gamepad: Gamepad
}

const GAMEPAD_CONNECTED = 'input-gamepad-connected' as const
const GAMEPAD_DISCONNECTED = 'input-gamepad-disconnected' as const

enum BUTTON_STATUS {
  FIRST_FRAME_BUTTON_DOWN,
  FIRST_FRAME_BUTTON_UP,
}

enum KEY_STATUS {
  KEY_DOWN,
  FIRST_FRAME_DOWN,
  KEY_PRESSED,
  KEY_UP,
  FIRST_FRAME_UP,
  DELETE,
}

type MouseButtons = {
  primaryButton: KEY_STATUS
  auxillaryButton: KEY_STATUS
  secondaryButton: KEY_STATUS
  fourthButton: KEY_STATUS
  fifthButton: KEY_STATUS
}

type MousePositions = {
  clientX: number
  clientY: number
  movementX: number
  movementY: number
  scrollX: number
  scrollY: number
}

interface GamepadInput {
  gamepad: Gamepad
  btnIndexToButtonStatus: Map<number, BUTTON_STATUS>
}

const MOUSE_BUTTON_KEYS = [
  'primaryButton', 'auxillaryButton', 'secondaryButton', 'fourthButton', 'fifthButton',
] as const

// NOTE(johnny): The cooldown in Chrome is ~1 second before another pointer lock request
// can be made.
const POINTER_LOCK_COOLDOWN = 1600

const createInputListener = (
  events: Events,
  element: HTMLElement
) => {
  const {globalId} = events
  let attached_ = false
  let pointerLock_ = false
  const gpdIndexToGamepadInput_: Map<number, GamepadInput> = new Map()
  const codeToKeyStatus_: Map<string, KEY_STATUS> = new Map()
  let mouseMoving_ = false
  let mouseScrolling_ = false
  const mouseButtonsToKeyStatus_: MouseButtons = {
    primaryButton: KEY_STATUS.DELETE,
    auxillaryButton: KEY_STATUS.DELETE,
    secondaryButton: KEY_STATUS.DELETE,
    fourthButton: KEY_STATUS.DELETE,
    fifthButton: KEY_STATUS.DELETE,
  }
  const mousePositionsToValue_: MousePositions = {
    clientX: 0,
    clientY: 0,
    movementX: 0,
    movementY: 0,
    scrollX: 0,
    scrollY: 0,
  }
  const touchIdToStatus_: Map<number, KEY_STATUS> = new Map()
  let _lastExitTime = 0

  const getGamepadInputFromIndex = (
    index?: number
  ): GamepadInput | undefined => (index !== undefined
    ? gpdIndexToGamepadInput_.get(index)
    : gpdIndexToGamepadInput_.values().next().value)

  const handleKeyDown = (e: KeyboardEvent) => {
    if (!codeToKeyStatus_.has(e.code)) {
      codeToKeyStatus_.set(e.code, KEY_STATUS.KEY_DOWN)
    }
  }

  const handleKeyUp = (e: KeyboardEvent) => {
    codeToKeyStatus_.set(e.code, KEY_STATUS.KEY_UP)
  }

  const handleGamepadLoop = (forceGamepadUpdate?: boolean) => {
    if (!attached_) {
      return
    }
    if (mouseMoving_) {
      mouseMoving_ = false
    } else if (mousePositionsToValue_.movementX || mousePositionsToValue_.movementY) {
      mousePositionsToValue_.movementX = 0
      mousePositionsToValue_.movementY = 0
    }
    if (mouseScrolling_) {
      mouseScrolling_ = false
    } else if (mousePositionsToValue_.scrollX || mousePositionsToValue_.scrollY) {
      mousePositionsToValue_.scrollX = 0
      mousePositionsToValue_.scrollY = 0
    }

    if (gpdIndexToGamepadInput_.size !== 0 || forceGamepadUpdate) {
      const gamepads = navigator?.getGamepads?.()
      // Delete gamepads in gamepads_ that are not in gamepads
      const gamepadIndices = new Set(gamepads?.map(gamepad => gamepad?.index))
      Array.from(gpdIndexToGamepadInput_.keys()).forEach((index) => {
        if (!gamepadIndices.has(index)) {
          gpdIndexToGamepadInput_.delete(index)
        }
      })

      if (gamepads) {
        gamepads.forEach((gamepad) => {
          if (!gamepad) {
            return
          }
          const oldGamepad = gpdIndexToGamepadInput_.get(gamepad.index)
          if (oldGamepad) {
            const oldButtons = oldGamepad.gamepad.buttons
            gamepad.buttons.forEach(
              (button, idx) => {
                if (button.pressed && !oldButtons[idx].pressed) {
                  oldGamepad.btnIndexToButtonStatus.set(idx, BUTTON_STATUS.FIRST_FRAME_BUTTON_DOWN)
                } else if (!button.pressed && oldButtons[idx].pressed) {
                  oldGamepad.btnIndexToButtonStatus.set(idx, BUTTON_STATUS.FIRST_FRAME_BUTTON_UP)
                } else if (oldGamepad.btnIndexToButtonStatus.has(idx)) {
                  oldGamepad.btnIndexToButtonStatus.delete(idx)
                }
              }
            )
          }
          gpdIndexToGamepadInput_.set(gamepad.index, {
            gamepad, btnIndexToButtonStatus: oldGamepad?.btnIndexToButtonStatus || new Map(),
          })
        })
      }
    }

    codeToKeyStatus_.forEach((keyStatus, code) => {
      if (keyStatus === KEY_STATUS.KEY_PRESSED) {
        return
      }
      const newKeyStatus = keyStatus + 1
      if (newKeyStatus < KEY_STATUS.DELETE) {
        codeToKeyStatus_.set(code, newKeyStatus)
      } else {
        codeToKeyStatus_.delete(code)
      }
    })

    Object.keys(mouseButtonsToKeyStatus_).forEach((button) => {
      const keyStatus = mouseButtonsToKeyStatus_[button as keyof MouseButtons]
      if (keyStatus === KEY_STATUS.KEY_PRESSED) {
        return
      }
      // NOTE(johnny): Move keyStatus up one level. Look at enum KEY_STATUS for more info.
      const newKeyStatus = keyStatus + 1
      if (newKeyStatus <= KEY_STATUS.DELETE) {
        mouseButtonsToKeyStatus_[button as keyof MouseButtons] = newKeyStatus
      }
    })

    touchIdToStatus_.forEach((status, id) => {
      if (status === KEY_STATUS.KEY_PRESSED) {
        return
      }
      const newKeyStatus = status + 1
      if (newKeyStatus < KEY_STATUS.DELETE) {
        touchIdToStatus_.set(id, newKeyStatus)
      } else {
        touchIdToStatus_.delete(id)
      }
    })
  }
  const handleGamepadConnected = (event: GamepadEvent) => {
    gpdIndexToGamepadInput_.set(event.gamepad.index, {
      gamepad: event.gamepad,
      btnIndexToButtonStatus: new Map(),
    })
    events.dispatch(globalId, GAMEPAD_CONNECTED, {
      gamepad: event.gamepad,
    })
  }

  const handleGamepadDisconnected = (event: GamepadEvent) => {
    gpdIndexToGamepadInput_.delete(event.gamepad.index)
    events.dispatch(globalId, GAMEPAD_DISCONNECTED, {
      gamepad: event.gamepad,
    })
  }

  const handleMouseMovement = (event: MouseEvent) => {
    mouseMoving_ = true
    mousePositionsToValue_.clientX = event.clientX
    mousePositionsToValue_.clientY = event.clientY
    mousePositionsToValue_.movementX = event.movementX
    mousePositionsToValue_.movementY = event.movementY
  }

  const handleMouseDown = (event: MouseEvent) => {
    mouseButtonsToKeyStatus_[MOUSE_BUTTON_KEYS[event.button]] = KEY_STATUS.KEY_DOWN
  }

  const handleMouseUp = (event: MouseEvent) => {
    mouseButtonsToKeyStatus_[MOUSE_BUTTON_KEYS[event.button]] = KEY_STATUS.KEY_UP
  }

  const handleMouseScroll = (event: WheelEvent) => {
    mouseScrolling_ = true
    mousePositionsToValue_.scrollX = event.deltaX
    mousePositionsToValue_.scrollY = event.deltaY
  }

  const getButton = (
    input: number,
    gamepadIdx?: number
  ): boolean => {
    const gamepadInput = getGamepadInputFromIndex(gamepadIdx)
    if (!gamepadInput) {
      return false
    }
    return gamepadInput.gamepad.buttons[input]?.pressed
  }

  const getButtonDown = (input: number, gamepadIdx?: number) => {
    const gamepadInput = getGamepadInputFromIndex(gamepadIdx)
    if (!gamepadInput) {
      return false
    }
    return gamepadInput.btnIndexToButtonStatus.get(input) === BUTTON_STATUS.FIRST_FRAME_BUTTON_DOWN
  }

  const getButtonUp = (input: number, gamepadIdx?: number) => {
    const gamepadInput = getGamepadInputFromIndex(gamepadIdx)
    if (!gamepadInput) {
      return false
    }
    return gamepadInput.btnIndexToButtonStatus.get(input) === BUTTON_STATUS.FIRST_FRAME_BUTTON_UP
  }

  const getAxis = (gamepadIdx?: number) => {
    const gamepadInput = getGamepadInputFromIndex(gamepadIdx)
    if (!gamepadInput) {
      return undefined
    }
    return gamepadInput.gamepad.axes
  }

  const pointerLockChanged = () => {
    if (!document.pointerLockElement) {
      _lastExitTime = performance.now()
    }
  }

  const handlePointerLock = () => {
    if (!window.document.pointerLockElement && pointerLock_ &&
      performance.now() - _lastExitTime > POINTER_LOCK_COOLDOWN) {
      element.requestPointerLock()
    }
  }

  const exitPointerLock = () => {
    if (document.pointerLockElement) {
      document.exitPointerLock()
    }
  }

  const enablePointerLockRequest = () => {
    pointerLock_ = true
  }

  const disablePointerLockRequest = () => {
    pointerLock_ = false
  }

  const isPointerLockActive = () => !!document.pointerLockElement

  const handleBlur = () => {
    codeToKeyStatus_.clear()
    gpdIndexToGamepadInput_.clear()
  }

  const getKey = (code: string) => {
    const keyStatus = codeToKeyStatus_.get(code)
    return !!(keyStatus && keyStatus <= KEY_STATUS.KEY_PRESSED)
  }

  const getKeyDown = (code: string) => codeToKeyStatus_.get(code) === KEY_STATUS.FIRST_FRAME_DOWN

  const getKeyUp = (code: string) => codeToKeyStatus_.get(code) === KEY_STATUS.FIRST_FRAME_UP

  const getGamepads = () => Array.from(gpdIndexToGamepadInput_.values())
    .map(gamepadInput => gamepadInput.gamepad)

  const getMousePosition = () => ([
    mousePositionsToValue_.clientX,
    mousePositionsToValue_.clientY,
  ] as const)

  const getMouseVelocity = () => ([
    mousePositionsToValue_.movementX,
    mousePositionsToValue_.movementY,
  ] as const)

  const getMouseScroll = () => ([
    mousePositionsToValue_.scrollX,
    mousePositionsToValue_.scrollY,
  ] as const)

  const getMouseButton = (button: number) => {
    const buttonStatus = mouseButtonsToKeyStatus_[MOUSE_BUTTON_KEYS[button]]
    return !!(buttonStatus && buttonStatus <= KEY_STATUS.KEY_PRESSED)
  }

  const getMouseDown = (button: number) => (
    mouseButtonsToKeyStatus_[MOUSE_BUTTON_KEYS[button]] === KEY_STATUS.FIRST_FRAME_DOWN
  )

  const getMouseUp = (button: number) => (
    mouseButtonsToKeyStatus_[MOUSE_BUTTON_KEYS[button]] === KEY_STATUS.FIRST_FRAME_UP
  )

  const handleTouchDown = (event: TouchEvent) => {
    Array.from(event.changedTouches).forEach((touch) => {
      touchIdToStatus_.set(touch.identifier, KEY_STATUS.KEY_DOWN)
    })
  }

  const handleTouchUp = (event: TouchEvent) => {
    Array.from(event.changedTouches).forEach((touch) => {
      touchIdToStatus_.set(touch.identifier, KEY_STATUS.KEY_UP)
    })
  }

  const getTouch = (identifier?: number) => {
    if (identifier === undefined) {
      return Array.from(touchIdToStatus_.values())
        .some(status => status <= KEY_STATUS.KEY_PRESSED)
    }
    const status = touchIdToStatus_.get(identifier)
    return !!(status && status <= KEY_STATUS.KEY_PRESSED)
  }

  const getTouchDown = (identifier?: number) => {
    if (identifier === undefined) {
      return Array.from(touchIdToStatus_.values())
        .some(status => status === KEY_STATUS.FIRST_FRAME_DOWN)
    }
    return touchIdToStatus_.get(identifier) === KEY_STATUS.FIRST_FRAME_DOWN
  }

  const getTouchUp = (identifier?: number) => {
    if (identifier === undefined) {
      return Array.from(touchIdToStatus_.values())
        .some(status => status === KEY_STATUS.FIRST_FRAME_UP)
    }
    return touchIdToStatus_.get(identifier) === KEY_STATUS.FIRST_FRAME_UP
  }

  const getTouchIds = () => Array.from(touchIdToStatus_.keys())

  const attach = () => {
    attached_ = true
    window.addEventListener('keydown', handleKeyDown)
    window.addEventListener('keyup', handleKeyUp)
    window.addEventListener('blur', handleBlur)
    window.addEventListener('contextmenu', handleBlur)
    window.addEventListener('gamepadconnected', handleGamepadConnected)
    window.addEventListener('gamepaddisconnected', handleGamepadDisconnected)
    element.addEventListener('click', handlePointerLock)
    element.addEventListener('mousemove', handleMouseMovement)
    element.addEventListener('mousedown', handleMouseDown)
    element.addEventListener('mouseup', handleMouseUp)
    element.addEventListener('wheel', handleMouseScroll)
    element.addEventListener('touchstart', handleTouchDown)
    element.addEventListener('touchend', handleTouchUp)
    element.addEventListener('touchcancel', handleTouchUp)
    document.addEventListener('pointerlockchange', pointerLockChanged)

    handleGamepadLoop(true)
  }

  const detach = () => {
    attached_ = false
    codeToKeyStatus_.clear()
    gpdIndexToGamepadInput_.clear()
    window.removeEventListener('keydown', handleKeyDown)
    window.removeEventListener('keyup', handleKeyUp)
    window.removeEventListener('blur', handleBlur)
    window.removeEventListener('contextmenu', handleBlur)
    window.removeEventListener('gamepadconnected', handleGamepadConnected)
    window.removeEventListener('gamepaddisconnected', handleGamepadDisconnected)
    element.removeEventListener('click', handlePointerLock)
    element.removeEventListener('mousemove', handleMouseMovement)
    element.removeEventListener('mousedown', handleMouseDown)
    element.removeEventListener('mouseup', handleMouseUp)
    element.removeEventListener('wheel', handleMouseScroll)
    element.removeEventListener('touchstart', handleTouchDown)
    element.removeEventListener('touchend', handleTouchUp)
    element.removeEventListener('touchcancel', handleTouchUp)
    document.removeEventListener('pointerlockchange', pointerLockChanged)
  }

  const api: InputListenerApi = {
    getAxis,
    getGamepads,
    getKey,
    getKeyDown,
    getKeyUp,
    getButton,
    getButtonDown,
    getButtonUp,
    enablePointerLockRequest,
    disablePointerLockRequest,
    getMouseButton,
    getMouseDown,
    getMouseUp,
    getMousePosition,
    getMouseVelocity,
    getMouseScroll,
    isPointerLockActive,
    exitPointerLock,
    getTouch,
    getTouchDown,
    getTouchUp,
    getTouchIds,
  }

  return {
    api,
    handleGamepadLoop,
    attach,
    detach,
  }
}

export {
  createInputListener,
  GAMEPAD_CONNECTED,
  GAMEPAD_DISCONNECTED,
}

export type {
  InputListenerApi,
  GamepadConnectedEvent,
  GamepadDisconnectedEvent,
}

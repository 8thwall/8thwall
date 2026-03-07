import type {DeepReadonly} from 'ts-essentials'

import type {InputMap} from '../shared/scene-graph'

interface InputManagerApi {
  setActiveMap: (name: string) => void
  getActiveMap: () => string
  getAction: (action: string) => number
  readInputMap: (inputMap: DeepReadonly<InputMap>) => void
}

interface InputListenerApi {
  getAxis: (gamepadIdx?: number) => DeepReadonly<number[]> | undefined
  getGamepads: () => DeepReadonly<Gamepad[]>
  getKey: (code: string) => boolean
  getKeyDown: (code: string) => boolean
  getKeyUp: (code: string) => boolean
  getButton: (input: number, gamepadIdx?: number) => boolean
  getButtonDown: (input: number, gamepadIdx?: number) => boolean
  getButtonUp: (input: number, gamepadIdx?: number) => boolean
  enablePointerLockRequest: () => void
  disablePointerLockRequest: () => void
  isPointerLockActive: () => boolean
  exitPointerLock: () => void
  getMouseButton: (value: number) => boolean
  getMouseDown: (value: number) => boolean
  getMouseUp: (value: number) => boolean
  getMousePosition: () => DeepReadonly<[number, number]>
  getMouseVelocity: () => DeepReadonly<[number, number]>
  getMouseScroll: () => DeepReadonly<[number, number]>
  getTouch: (identifier?: number) => boolean
  getTouchDown: (identifier?: number) => boolean
  getTouchUp: (identifier?: number) => boolean
  getTouchIds: () => number[]
}

interface InputApi extends InputListenerApi, InputManagerApi{
  attach: () => void
  detach: () => void
}

export type {
  InputListenerApi,
  InputManagerApi,
  InputApi,
}

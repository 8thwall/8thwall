import type {DeepReadonly} from 'ts-essentials'

import type {InputMap} from '../shared/scene-graph'
import type {InputListenerApi, InputManagerApi} from './input-types'

interface ManagerBinding {
  input: string[]
  modifiers: string[][]
}

type Direction = 'up' | 'down' | 'right' | 'left'

// NOTE(johnny) The return value is a value from 0.05 < x <= 1 in the direction that was requested.
function getAxisValue(axis: number, direction: Direction): number | null {
  if (Math.abs(axis) < 0.05) {
    return null
  }

  switch (direction) {
    case 'up':
    case 'left':
      return axis < 0 ? -axis : null
    case 'down':
    case 'right':
      return axis > 0 ? axis : null
    default:
      return null
  }
}

const getValueFromDirection = (
  direction: Direction,
  xAxis: number,
  yAxis: number
) => {
  switch (direction) {
    case 'up':
    case 'down':
      return getAxisValue(yAxis, direction)
    case 'right':
    case 'left':
      return getAxisValue(xAxis, direction)
    default:
      return null
  }
}

// NOTE(johnny) The return value is a value from 0 < x <= infinity in the direction.
const getMouseValueFromDirection = (direction: Direction, axis: number) => {
  switch (direction) {
    case 'down':
    case 'right':
      return axis > 0 ? axis : null
    case 'up':
    case 'left':
      return axis < 0 ? -axis : null
    default:
      return null
  }
}

// @param input an input specifier indicating which input to check
// @param inputListener the underlying API to read data from
// @return [0, inf) where 0 means the input is not active, 1 is active for button,
// other input might have higher values
// @example getInputValue(['gamepad','button',0], ...)
// returns whether the gamepad button 0 was pressed
const getValueFromInput = (input: DeepReadonly<string[]>, inputListener: InputListenerApi) => {
  if (input[0] === 'keyboard') {
    if (inputListener.getKey(input[1])) {
      return 1
    }
  } else if (input[0] === 'gamepad') {
    if (input[1] === 'button') {
      if (inputListener.getButton(parseInt(input[2], 10))) {
        return 1
      }
    } else if (input[1] === 'axis') {
      const axis = inputListener.getAxis()
      if (axis) {
        if (input[2] === 'left') {
          const value = getValueFromDirection(input[3] as Direction, axis[0], axis[1])
          if (value !== null) {
            return value
          }
        } else if (input[2] === 'right') {
          const value = getValueFromDirection(input[3] as Direction, axis[2], axis[3])
          if (value !== null) {
            return value
          }
        }
      }
    }
  } else if (input[0] === 'mouse') {
    if (input[1] === 'button') {
      if (inputListener.getMouseButton(parseInt(input[2], 10))) {
        return 1
      }
    } else if (input[1] === 'scroll') {
      const scroll = inputListener.getMouseScroll()
      if (scroll) {
        const direction = input[2]
        if (direction === 'up' || direction === 'down') {
          const value = getMouseValueFromDirection(direction, scroll[1])
          if (value) {
            return value
          }
        } else if (direction === 'left' || direction === 'right') {
          const value = getMouseValueFromDirection(direction, scroll[0])
          if (value) {
            return value
          }
        }
      }
    } else if (input[1] === 'move') {
      const move = inputListener.getMouseVelocity()
      if (move) {
        const direction = input[2]
        if (direction === 'up' || direction === 'down') {
          const value = getMouseValueFromDirection(direction, move[1])
          if (value) {
            return value
          }
        } else if (direction === 'left' || direction === 'right') {
          const value = getMouseValueFromDirection(direction, move[0])
          if (value) {
            return value
          }
        }
      }
    }
  } else if (input[0] === 'touch') {
    const touch = inputListener.getTouch()
    if (touch) {
      return 1
    }
  }
  return null
}

const createInputManager = (inputListener: InputListenerApi) => {
  const inputMapToActions_ = new Map<string, Map<string, DeepReadonly<ManagerBinding>[]>>()
  const activeActionsToValues_ = new Map<string, number>()
  let activeMap_ = 'default'
  let attached_ = false

  const addActionMap = (name: string) => {
    if (inputMapToActions_.has(name)) {
      throw new Error('Map already exists')
    }
    inputMapToActions_.set(name, new Map())
  }

  const addAction = (map: string, action: string) => {
    const actionMap = inputMapToActions_.get(map)
    if (!actionMap) {
      throw new Error('Map does not exist')
    }
    if (actionMap.has(action)) {
      throw new Error('Action already exists')
    }
    actionMap.set(action, [])
  }

  const addBinding = (
    map: string,
    actionName: string,
    input: string,
    modifiers: DeepReadonly<string[]>
  ) => {
    const actionMap = inputMapToActions_.get(map)
    let splitModifiers: string[][] = []
    if (!actionMap) {
      throw new Error('Map does not exist')
    }

    const action = actionMap.get(actionName)
    if (!action) {
      throw new Error('Action does not exist')
    }

    if (modifiers) {
      splitModifiers = modifiers.map(modifier => modifier.split(':'))
    }
    action.push({input: input.split(':'), modifiers: splitModifiers})
  }

  const setActiveMap = (name: string) => {
    activeMap_ = name
  }

  const getActiveMap = () => activeMap_

  const handleActions = () => {
    if (!attached_) {
      return
    }

    activeActionsToValues_.clear()
    const map = inputMapToActions_.get(activeMap_)
    if (!map) {
      return
    }
    map.forEach((bindings, action) => {
      bindings.forEach((binding) => {
        const {input, modifiers} = binding
        const value = getValueFromInput(input, inputListener)
        if (!value) {
          return
        }

        const passedModifierCheck = !modifiers || (
          modifiers.every(modifier => getValueFromInput(modifier, inputListener))
        )

        if (passedModifierCheck) {
          activeActionsToValues_.set(action, value)
        }
      })
    })
  }

  const readInputMap = (inputMap: DeepReadonly<InputMap>) => {
    inputMapToActions_.clear()
    Object.keys(inputMap).forEach((map) => {
      if (!inputMapToActions_.has(map)) {
        addActionMap(map)
      }
      inputMap[map].forEach((action) => {
        addAction(map, action.name)
        action.bindings.forEach((binding) => {
          addBinding(map, action.name, binding.input, binding.modifiers)
        })
      })
    })
  }

  const getAction = (action: string) => activeActionsToValues_.get(action) || 0

  const attach = () => {
    attached_ = true
  }

  const detach = () => {
    activeActionsToValues_.clear()
    attached_ = false
  }

  const api: InputManagerApi = {
    setActiveMap,
    getActiveMap,
    getAction,
    // NOTE(johnny): This function should not be exposed publicly, but we need this accessible in
    // entity.ts to read the input map from the scene graph.
    readInputMap,
  }

  return {
    api,
    handleActions,
    attach,
    detach,
  }
}

export type {
  InputManagerApi,
}

export {
  createInputManager,
}

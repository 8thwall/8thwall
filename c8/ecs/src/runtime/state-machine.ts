import type {World} from './world'
import type {EventListener} from './events'
import type {Eid} from '../shared/schema'
import {createInstanced} from '../shared/instanced'
import {getStateId, StateDefiner} from './state-definer'
import {StateGroupDefiner} from './stategroup-definer'
import type {
  BaseMachineDefProps, Callback, CleanupList, ListenerParams, MachineId,
  StateMachine, StateMachineDefiner, StateMachineDefinerContext, StateMachineDefinition, Trigger,
  TriggerHandle, State, StateGroup, StateId, IStateDefiner, IStateGroupDefiner,
} from './state-machine-types'

let stateMachineContext: StateMachineDefinerContext<unknown> | null = null
let lastMachineId = 0

const generateId = () => ++lastMachineId
const machineMap = createInstanced<World, Map<MachineId, StateMachine<unknown>>>(() => new Map())

/**
 * get all groups that contain a given state
 * @param machine the state machine to get the groups from
 * @param state the state to get the groups for
 * @returns list of containing group indices
 */
const getGroups = <CallbackArgument>(
  machine: StateMachine<CallbackArgument>, state: StateId
) => machine.groups
    .map((group, index): [StateGroup<CallbackArgument>, number] => [group, index])
    .filter(([group]) => !group.substates?.length ||
  group.substates.some(substate => getStateId(substate) === getStateId(state)))
    .map(([, index]) => index)

/**
 * get all groups that contain the current state
 * @param machine the state machine to get the groups from
 * @returns list of current groups indices
 */
const currentStateGroups = <CallbackArgument>(
  machine: StateMachine<CallbackArgument>
) => getGroups(machine, machine.currentState)

/**
 * function to clear all currently attached triggers to the state machine
 * @param machine the state machine to clear triggers from
 * @param groups the group indinces to clear triggers from
 * @returns void
 */
const clearTriggers = <CallbackArgument>(
  machine: StateMachine<CallbackArgument>, groups: number[]
) => {
  const clear = (list: CleanupList) => {
    while (list?.length) {
      list.pop()!()
    }
  }
  clear(machine.currentCleanup)
  groups.forEach((source) => {
    clear(machine.groupCleanup[source])
  })
}

/**
 * attach the state machine current state triggers and event listeners to the state machine
 * @param machine the state machine to attach triggers to
 * @returns void
 * @throws Error if the current state is unknown
 * @throws Error if triggers are already started
 */
const attachTriggers = <CallbackArgument>(
  machine: StateMachine<CallbackArgument>, groups: number[]
) => {
  const state = machine.states[machine.currentState]
  if (!state) {
    throw new Error(`Unknown state (start): ${machine.currentState}`)
  }

  if (Object.values(machine.currentCleanup).some(c => c.length)) {
    throw new Error('Triggers already started')
  }
  machine.currentCleanup = []
  groups.forEach((idx) => {
    if (Object.values(machine.groupCleanup[idx]).some(c => c.length)) {
      throw new Error('Triggers already started')
    }
    machine.groupCleanup[idx] = []
  })

  const attach = (cleanup: CleanupList, nextState: StateId, triggers: Trigger[]) => {
    triggers.forEach((trigger) => {
      switch (trigger.type) {
        case 'event': {
          const listener: EventListener = (event) => {
            if (trigger.beforeTransition?.(event)) {
              return
            }
            if (trigger.where && !trigger.where(event)) {
              return
            }
            // eslint-disable-next-line @typescript-eslint/no-use-before-define
            transitionTo(machine, getStateId(nextState))
          }
          const target = trigger.target ?? machine.eid
          machine.world.events.addListener(target, trigger.event, listener)
          cleanup.push(() => {
            machine.world.events.removeListener(target, trigger.event, listener)
          })
          break
        }
        case 'timeout': {
          const timeout = machine.world.time.setTimeout(() => {
            // eslint-disable-next-line @typescript-eslint/no-use-before-define
            transitionTo(machine, getStateId(nextState))
          }, trigger.timeout)
          cleanup.push(() => {
            machine.world.time.clearTimeout(timeout)
          })
          break
        }
        case 'custom': {
          const transition = () => {
            // eslint-disable-next-line @typescript-eslint/no-use-before-define
            transitionTo(machine, getStateId(nextState))
          }
          trigger.handle.listen(transition)
          cleanup.push(() => {
            trigger.handle.unlisten(transition)
          })
          break
        }
        default:
          throw new Error('Unknown trigger type')
      }
    })
  }

  Object.entries(state.triggers).forEach(([nextState, triggers]) => {
    attach(machine.currentCleanup, nextState, triggers)
  })

  groups.forEach((idx) => {
    Object.entries(machine.groups[idx].triggers).forEach(([nextState, triggers]) => {
      attach(machine.groupCleanup[idx], nextState, triggers)
    })
  })

  const listen = (cleanup: CleanupList, params: ListenerParams) => {
    const target = typeof params.target === 'function' ? params.target() : params.target
    machine.world.events.addListener(target, params.name, params.listener)
    cleanup.push(() => {
      machine.world.events.removeListener(target, params.name, params.listener)
    })
  }

  state.listeners?.forEach((params) => {
    listen(machine.currentCleanup, params)
  })

  groups.forEach((idx) => {
    machine.groups[idx].listeners?.forEach((params) => {
      listen(machine.groupCleanup[idx], params)
    })
  })
}

const transitionTo = <CallbackArgument>(
  machine: StateMachine<CallbackArgument>, state: string
) => {
  const oldState = machine.states[machine.currentState]
  const newState = machine.states[state]
  if (!oldState) {
    throw new Error(`Unknown state (exit): ${machine.currentState}`)
  } else if (!newState) {
    throw new Error(`Unknown state (enter): ${state}`)
  }
  const prevGroups = getGroups(machine, machine.currentState)
  const nextGroups = getGroups(machine, state)
  const oldGroups = prevGroups.filter(group => !nextGroups.includes(group))
  const newGroups = nextGroups.filter(group => !prevGroups.includes(group))

  const argument = machine.prepareCallback()
  clearTriggers(machine, oldGroups)
  oldState.onExit?.()
  oldGroups.map(index => machine.groups[index])
    .forEach(group => group.onExit?.())
  newGroups.map(index => machine.groups[index])
    .forEach(group => group.onEnter?.(argument))
  newState.onEnter?.(argument)
  machine.currentState = state
  attachTriggers(machine, newGroups)
}

/**
 * Function to define a new state
 * @param name the name of the state
 * @returns a new state
 */
const defineState = <CallbackArgument = void>(
  name: string
): IStateDefiner<CallbackArgument> => {
  if (!stateMachineContext?.states) {
    throw new Error('State must be defined within a state machine definition')
  }
  if (stateMachineContext.states.find(s => s.name === name)) {
    throw new Error(`State already exists: ${name}`)
  }
  const state = new StateDefiner<CallbackArgument>(name)
  stateMachineContext.states.push(state)
  return state
}

/**
 * Function to define a new group
 * @param substates the substates of the group (leaving blank will capture all states)
 * @returns a new group
 */
const defineStateGroup = <CallbackArgument = void>(
  substates?: Array<StateId | IStateGroupDefiner<unknown>>
): IStateGroupDefiner<CallbackArgument> => {
  if (!stateMachineContext?.groups) {
    throw new Error('StateGroup must be defined within a state machine definition')
  }
  const allSubstates = substates?.map(s => (s instanceof StateGroupDefiner ? s.substates : s))
    .flat().filter(s => s !== undefined) as StateId[] | undefined
  const group = new StateGroupDefiner<CallbackArgument>(allSubstates)
  stateMachineContext.groups.push(group)
  return group
}

/**
 * define a custom trigger that can be called to cause a transition
 * @returns a new custom trigger definition
 */
const defineTrigger = (): TriggerHandle => {
  const callbacks: Set<Callback> = new Set()
  return {
    trigger() {
      // ignore callbacks added after we started iterating
      const copy = [...callbacks]
      for (const cb of copy) {
        // skip callbacks that have been removed since the copy
        if (callbacks.has(cb)) {
          try {
            cb()
          } catch (err) {
            // eslint-disable-next-line no-console
            console.error(err)
          }
        }
      }
    },
    listen(cb) {
      callbacks.add(cb)
    },
    unlisten(cb) {
      callbacks.delete(cb)
    },
  }
}

/**
 * Function to generate the state machine definition object
 * @param world the world to create the state machine in
 * @param eid the entity that owns the state machine
 * @param definer the state machine definer function
 * @returns the definition object of your state machine
 */
const generateStateMachineDefiner = <CallbackArgument>(
  world: World,
  eid: Eid,
  definer: StateMachineDefiner
): StateMachineDefinition<CallbackArgument> => {
  stateMachineContext = {states: [], groups: []}
  definer({world, eid, entity: world.getEntity(eid)})
  const initialStates = stateMachineContext.states.filter(s => s.isInitial)
  if (initialStates.length === 0) {
    throw new Error('No initial state defined')
  }
  if (initialStates.length > 1) {
    throw new Error('Multiple initial states defined')
  }
  const initialState = initialStates[0].name
  const res = {
    initialState,
    states: stateMachineContext.states.reduce((acc, s) => {
      acc[s.name] = s.generateState()
      return acc
    }, {} as Record<string, State<CallbackArgument>>),
    groups: stateMachineContext.groups.reduce((acc, s) => {
      acc.push(s.generateStateGroup())
      return acc
    }, [] as StateGroup<CallbackArgument>[]),
  }
  stateMachineContext = null
  return res
}

/**
 * Create a state machine
 * @param world the world to create the state machine in
 * @param eid the entity that owns the state machine
 * @param definition the state machine definition. This can be either an object or a function that
 *                   generate the definition object
 * @returns the id of the created state machine
 */
const createStateMachine = <CallbackArgument = void>(
  world: World,
  eid: Eid,
  definition: StateMachineDefinition<CallbackArgument> | StateMachineDefiner
): MachineId => {
  const resolvedDefinition = typeof definition === 'function'
    ? generateStateMachineDefiner<CallbackArgument>(world, eid, definition)
    : definition

  const {states, groups, initialState} = resolvedDefinition
  const prepareCallbackFn = resolvedDefinition.prepareCallback

  const machine: StateMachine<CallbackArgument> = {
    machineId: generateId(),
    world,
    eid,
    states,
    groups: groups ?? [],
    currentState: initialState,
    currentCleanup: [],
    groupCleanup: groups?.map(() => []) ?? [],
    prepareCallback: prepareCallbackFn ?? (() => {}) as (() => CallbackArgument),
  }

  const argument = machine.prepareCallback()
  const initialGroups = currentStateGroups(machine)
  initialGroups.map(name => machine.groups[name])
    .forEach(group => group.onEnter?.(argument))
  states?.[initialState].onEnter?.(argument)

  attachTriggers(machine, initialGroups)
  machineMap(world).set(machine.machineId, machine)
  return machine.machineId
}

const destroyStateMachine = <CallbackArgument>(machine: StateMachine<CallbackArgument>) => {
  const state = machine.states[machine.currentState]
  if (!state) {
    throw new Error(`Unknown state in destroy: ${machine.currentState}`)
  }
  const groups = currentStateGroups(machine)
  clearTriggers(machine, groups)
  state.onExit?.()
  groups.map(name => machine.groups[name])
    .forEach(group => group.onExit?.())
}

const deleteStateMachine = (world: World, machineId: MachineId) => {
  const machine = machineMap(world).get(machineId)
  if (machine) {
    destroyStateMachine(machine)
    machineMap(world).delete(machineId)
  }
}

const tickStateMachine = (world: World, machineId: MachineId) => {
  const machine = machineMap(world).get(machineId)
  if (machine) {
    const state = machine.states[machine.currentState]
    const ticks = [
      ...currentStateGroups(machine).map(group => machine.groups[group].onTick),
      state.onTick,
    ].filter(Boolean)
    if (ticks.length) {
      const argument = machine.prepareCallback()
      ticks.forEach(tick => tick?.(argument))
    }
  }
}

export {
  createStateMachine,
  deleteStateMachine,
  tickStateMachine,
  defineState,
  defineStateGroup,
  defineTrigger,
  generateStateMachineDefiner,
}

export type {
  State,
  StateGroup,
  MachineId,
  StateMachineDefinition,
  StateMachineDefiner,
  BaseMachineDefProps,
}

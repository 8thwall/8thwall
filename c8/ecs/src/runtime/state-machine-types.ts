import type {Eid, Schema, WriteData} from '../shared/schema'
import type {Entity} from './entity-types'
import type {EventListener, QueuedEvent} from './events'
import type {DataForEvent} from './events-types'
import type {World} from './world'
import type {WorldAttribute} from './world-attribute'

const append = <T>(arr: T[], value: T | T[]) => {
  if (Array.isArray(value)) {
    arr.push(...value)
  } else {
    arr.push(value)
  }
}

type TransitionCallback<CallbackArgument = void> = CallbackArgument extends void
  ? () => void
  : (arg: CallbackArgument) => void

interface State<CallbackArgument = void> {
  // list of next states this current state can transition to and the triggers to do so
  triggers: Record<string, Trigger[]>
  onEnter?: TransitionCallback<CallbackArgument>
  onTick?: TransitionCallback<CallbackArgument>
  onExit?: () => void
  listeners?: ListenerParams[]
}

interface IStateDefiner<CallbackArgument = void> {
  name: string
  initial: () => this
  onEnter: (cb: State<CallbackArgument>['onEnter']) => this
  onTick: (cb: State<CallbackArgument>['onTick']) => this
  onExit: (cb: State<CallbackArgument>['onExit']) => this
  onEvent: <T extends string>(
    event: T,
    nextState: StateId,
    args?: Omit<EventTrigger<T>, 'type' | 'event'>
  ) => this
  wait: (timeout: number, nextState: StateId) => this
  onTrigger: (trigger: TriggerHandle, nextState: StateId) => this
  listen: <T extends string>(
    target: EidGetter,
    name: T,
    listener: EventListener<DataForEvent<T>>
  ) => this
}

interface IStateDefinerInternal<CallbackArgument> {
  name: string
  isInitial: boolean
  generateState: () => State<CallbackArgument>
}

type StateId = string | {name: string}

interface StateGroup<CallbackArgument = void> {
  substates?: StateId[]
  triggers: Record<string, Trigger[]>
  onEnter?: TransitionCallback<CallbackArgument>
  onTick?: TransitionCallback<CallbackArgument>
  onExit?: () => void
  listeners?: ListenerParams[]
}

interface IStateGroupDefiner<CallbackArgument = void> {
  onEnter: (cb: StateGroup<CallbackArgument>['onEnter']) => this
  onTick: (cb: StateGroup<CallbackArgument>['onTick']) => this
  onExit: (cb: StateGroup<CallbackArgument>['onExit']) => this
  onEvent: (event: string, nextState: StateId, args?: Omit<EventTrigger, 'type' | 'event'>) => this
  wait: (timeout: number, nextState: StateId) => this
  onTrigger: (trigger: TriggerHandle, nextState: StateId) => this
  listen: (target: EidGetter, name: string, listener: EventListener) => this
}

interface IStateGroupDefinerInternal<CallbackArgument> {
  substates?: StateId[]
  generateStateGroup: () => StateGroup<CallbackArgument>
}
interface StateMachineDefinerContext<CallbackArgument = void> {
  states: IStateDefinerInternal<CallbackArgument>[]
  groups: IStateGroupDefinerInternal<CallbackArgument>[]
}

type MachineId = number
type CleanupList = Array<() => void>

interface StateMachine<CallbackArgument = void> {
  machineId: MachineId
  world: World
  eid: Eid
  states: Record<string, State<CallbackArgument>>
  groups: StateGroup<CallbackArgument>[]
  currentState: string
  currentCleanup: CleanupList
  groupCleanup: CleanupList[]
  prepareCallback: () => CallbackArgument
}

interface StateMachineDefinition<CallbackArgument = void> {
  initialState: string
  states: Record<string, State<CallbackArgument>>
  groups?: StateGroup<CallbackArgument>[]
  prepareCallback?: CallbackArgument extends void ? never : () => CallbackArgument
}

interface BaseMachineDefProps {
  world: World
  eid: Eid
  entity: Entity
}

type StateMachineDefiner = (props: BaseMachineDefProps) => void

type ComponentCallbackArgs<S extends Schema, D extends Schema> = {
  schema: WriteData<S>
  data: WriteData<D>
}

interface ComponentStateMachineDefProps<
  S extends Schema, D extends Schema
> extends BaseMachineDefProps {
  schemaAttribute: WorldAttribute<S>
  dataAttribute: WorldAttribute<D>
  defineState: (name: string) => IStateDefiner<ComponentCallbackArgs<S, D>>
  defineStateGroup: (
    substates?: Array<StateId | IStateGroupDefiner<unknown>>
  ) => IStateGroupDefiner<ComponentCallbackArgs<S, D>>
}

type ComponentStateMachineDefiner<S extends Schema, D extends Schema> = (
  props: ComponentStateMachineDefProps<S, D>
) => void

type ComponentStateMachineDefinition<S extends Schema, D extends Schema> =
  Omit<StateMachineDefinition<ComponentCallbackArgs<S, D>>, 'prepareCallback'>

/**
 * a trigger that transitions to the next state when an event is received
 * @param type a constant string to identify the trigger type
 * @param event the type of event that will trigger the transition
 * @param target optional entity id to listen for the event on
 * @param where optional condition to determine whether to transition
 * @param beforeTransition optional callback to run before transitioning (deprecated)
 */
type EventTrigger<T extends string = string> = {
  type: 'event'
  event: T
  target?: Eid
  where?: (event: QueuedEvent<DataForEvent<T>>) => boolean
  /** @deprecated */
  beforeTransition?: (event: QueuedEvent<DataForEvent<T>>) => boolean
}

/**
 * a trigger that transitions to the next state after a timeout
 * @param timeout number of ms before transitioning to the next state
 */
type TimeoutTrigger = {
  type: 'timeout'
  timeout: number
}

type Callback = () => void

type TriggerHandle = {
  trigger: () => void
  listen: (cb: Callback) => void
  unlisten: (cb: Callback) => void
}

/**
 * a trigger that transitions instantly when activated.
 * @param caller the function to trigger the transition
 */
type CustomTrigger = {
  type: 'custom'
  handle: TriggerHandle
}

type Trigger = EventTrigger<any> | TimeoutTrigger | CustomTrigger

type EidGetter = Eid | (() => Eid)

type ListenerParams = {
  target: EidGetter
  name: string
  listener: EventListener<any>
}

export {
  append,
}

export type {
  State,
  IStateDefiner,
  IStateDefinerInternal,
  StateId,
  StateGroup,
  IStateGroupDefiner,
  IStateGroupDefinerInternal,
  StateMachineDefinerContext,
  MachineId,
  CleanupList,
  StateMachine,
  StateMachineDefinition,
  BaseMachineDefProps,
  StateMachineDefiner,
  ComponentStateMachineDefProps,
  ComponentStateMachineDefiner,
  ComponentStateMachineDefinition,
  EventTrigger,
  TimeoutTrigger,
  CustomTrigger,
  Trigger,
  TriggerHandle,
  EidGetter,
  ListenerParams,
  Callback,
  ComponentCallbackArgs,
}

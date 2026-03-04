import type {EventListener} from './events'
import type {DataForEvent} from './events-types'
import {
  append, EidGetter, EventTrigger, IStateDefiner, IStateDefinerInternal, State, StateId, Trigger,
  TriggerHandle,
} from './state-machine-types'

const getStateId = (state: StateId) => (
  typeof state === 'string' ? state : state.name
)

/**
 * Fluent API for defining a state
 */
class StateDefiner<CallbackArgs = void> implements
IStateDefiner<CallbackArgs>, IStateDefinerInternal<CallbackArgs> {
  readonly name: string

  isInitial: boolean = false;

  onEnterImpl?: State<CallbackArgs>['onEnter'] | undefined;

  onTickImpl?: State<CallbackArgs>['onTick'] | undefined;

  onExitImpl?: State<CallbackArgs>['onExit'] | undefined;

  triggers: State<CallbackArgs>['triggers'] = {}

  eventListeners: NonNullable<State<CallbackArgs>['listeners']> = []

  constructor(name: string) {
    this.name = name
  }

  /**
   * generate the state object for definer
   * @returns the state object
   */
  generateState(): State<CallbackArgs> {
    return {
      triggers: this.triggers,
      ...(this.onEnterImpl && {onEnter: this.onEnterImpl}),
      ...(this.onTickImpl && {onTick: this.onTickImpl}),
      ...(this.onExitImpl && {onExit: this.onExitImpl}),
      listeners: this.eventListeners,
    }
  }

  /**
   * mark this state as the initial state
   * @returns this state definer
   */
  initial() {
    this.isInitial = true
    return this
  }

  /**
   * set a callback to run when entering this state
   * @param cb the callback to run
   * @returns this state definer
   */
  onEnter(cb: State<CallbackArgs>['onEnter']) {
    this.onEnterImpl = cb
    return this
  }

  /**
   * set a callback to run on each tick while in this state
   * @param cb the callback to run
   * @returns this state definer
   */
  onTick(cb: State<CallbackArgs>['onTick']) {
    this.onTickImpl = cb
    return this
  }

  /**
   * set a callback to run when exiting this state
   * @param cb the callback to run
   * @returns this state definer
   */
  onExit(cb: State<CallbackArgs>['onExit']) {
    this.onExitImpl = cb
    return this
  }

  /**
   * add trigger(s) to transition to the next state
   * @param nextState the state to add triggers for
   * @param trigger the trigger(s) to add
   * @returns this state definer
   */
  addTrigger(nextState: StateId, trigger: Trigger | Trigger[]) {
    const nextStateName = getStateId(nextState)
    const triggers = this.triggers[nextStateName] ?? []
    append(triggers, trigger)
    this.triggers[nextStateName] = triggers
    return this
  }

  /**
   * clear all triggers for a specific state or all states
   * @param nextState clear triggers for a specific state
   * @returns this state definer
   */
  clearTriggers(nextState?: StateId) {
    if (nextState) {
      delete this.triggers[getStateId(nextState)]
    } else {
      this.triggers = {}
    }
    return this
  }

  /**
   * trigger a transition to the next state when an event is received
   * @param event the event to listen for
   * @param nextState the next state to transition to
   * @param args optional arguments
   * @returns this state definer
   */
  onEvent<T extends string>(
    event: T,
    nextState: StateId,
    args: Omit<EventTrigger<T>, 'type' | 'event'> = {}
  ) {
    this.addTrigger(nextState, {
      type: 'event',
      event,
      ...args,
    })
    return this
  }

  /**
   * wait for a timeout before transitioning to the next state
   * @param timeout number of ms before transitioning to the next state
   * @param nextState the next state to transition to
   * @returns this state definer
   */
  wait(timeout: number, nextState: StateId) {
    return this.addTrigger(
      nextState, {type: 'timeout', timeout}
    )
  }

  /**
   * instantly transition to the next state when a custom trigger is invoked
   * @param trigger the custom trigger that should cause a transition when invoked
   * @param nextState the next state to transition to
   * @returns this state definer
   */
  onTrigger(trigger: TriggerHandle, nextState: StateId) {
    return this.addTrigger(nextState, {type: 'custom', handle: trigger})
  }

  /**
   * add event listener to the state which will be added
   * @param target the target to listen to
   * @param name the event to listen for
   * @param listener the callback to run when the event is received
   * @returns this state definer
   */
  listen<T extends string>(target: EidGetter, name: T, listener: EventListener<DataForEvent<T>>) {
    this.eventListeners.push({target, name, listener})
    return this
  }
}

export {
  StateDefiner,
  getStateId,
}

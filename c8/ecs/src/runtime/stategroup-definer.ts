import type {EventListener} from './events'
import {getStateId} from './state-definer'
import {
  append, EidGetter, EventTrigger, IStateGroupDefinerInternal, StateGroup, Trigger, TriggerHandle,
  StateId, IStateGroupDefiner,
} from './state-machine-types'

class StateGroupDefiner<CallbackArgs = void> implements
IStateGroupDefiner<CallbackArgs>, IStateGroupDefinerInternal<CallbackArgs> {
  substates?: StateGroup<CallbackArgs>['substates'] = undefined

  triggers: StateGroup<CallbackArgs>['triggers'] = {}

  onEnterImpl?: StateGroup<CallbackArgs>['onEnter'] | undefined

  onTickImpl?: StateGroup<CallbackArgs>['onTick'] | undefined

  onExitImpl?: StateGroup<CallbackArgs>['onExit'] | undefined

  eventListeners: NonNullable<StateGroup<CallbackArgs>['listeners']> = []

  constructor(substates?: StateGroup<CallbackArgs>['substates']) {
    this.substates = substates ? [...substates] : undefined
  }

  /**
   * generate the group object for definer
   * @returns the stater group object
   */
  generateStateGroup(): StateGroup<CallbackArgs> {
    return {
      substates: this.substates,
      triggers: this.triggers,
      onEnter: this.onEnterImpl,
      onTick: this.onTickImpl,
      onExit: this.onExitImpl,
      listeners: this.eventListeners,
    }
  }

  /**
   * set a callback to run when entering any of the substates from outside the group
   * @param cb the callback to run when entering the group
   * @returns this state group definer
   */
  onEnter(cb: StateGroup<CallbackArgs>['onEnter']) {
    this.onEnterImpl = cb
    return this
  }

  /**
   * set a callback to run on each tick while in this group
   * @param cb the callback to run
   * @returns this state group definer
   */
  onTick(cb: StateGroup<CallbackArgs>['onTick']) {
    this.onTickImpl = cb
    return this
  }

  /**
   * set a callback to run when exiting this group
   * @param cb the callback to run
   * @returns this state group definer
   */
  onExit(cb: StateGroup<CallbackArgs>['onExit']) {
    this.onExitImpl = cb
    return this
  }

  /**
   * add trigger(s) to all substates that transition to the next state
   * @param nextState the state to add triggers for
   * @param trigger the trigger(s) to add
   * @returns this state group definer
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
   * @returns this state group definer
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
   * @returns this state group definer
   */
  onEvent(
    event: string,
    nextState: StateId,
    args: Omit<EventTrigger, 'type' | 'event'> = {}
  ) {
    return this.addTrigger(nextState, {
      type: 'event',
      event,
      ...args,
    })
  }

  /**
   * wait for a timeout before transitioning to the next state
   * @param timeout number of ms before transitioning to the next state
   * @param nextState the next state to transition to
   * @returns this state group definer
   */
  wait(timeout: number, nextState: StateId) {
    return this.addTrigger(nextState, {
      type: 'timeout',
      timeout,
    })
  }

  /**
   * instantly transition to the next state when a custom trigger is invoked
   * @param trigger the custom trigger that should cause a transition when invoked
   * @param nextState the next state to transition to
   * @returns this state group definer
   */
  onTrigger(trigger: TriggerHandle, nextState: StateId) {
    return this.addTrigger(nextState, {type: 'custom', handle: trigger})
  }

  /**
   * add event listener to the state which will be added
   * @param target the target to listen to
   * @param name the event to listen for
   * @param listener the callback to run when the event is received
   * @returns this state group definer
   */
  listen(target: EidGetter, name: string, listener: EventListener) {
    this.eventListeners.push({target, name, listener})
    return this
  }
}

export {
  StateGroupDefiner,
}

import type {DeepReadonly} from 'ts-essentials'

import type {GraphObject} from './scene-graph'
import type {TransformMap} from './transform-map'

// Taken from studio-event-stream.ts
type CallbackConfig = {
  callback: (msg: any) => void
  postMessage: boolean
  webrtc: boolean
  sockets: boolean
}

// Taken from studio-event-stream.ts
type EventStreamManager = {
  handleSocketMessage: (msg: DebugMessage) => void
  send: (message: DebugMessage, attemptPostMessage?: boolean, attemptWebrtc?: boolean) => void
  sendViaSockets: (message: DebugMessage) => void
  sendViaPostMessage: (message: DebugMessage) => void
  sendViaWebrtc: (message: DebugMessage) => void
  // Listen to an event message via some transport as desired
  // By default, callbacks will be registered to fire on postMessage and socket events
  listen: (
    callback: (message: DebugMessage) => void,
    config?: Partial<Pick<CallbackConfig, 'webrtc' | 'sockets' | 'postMessage'>>,
  ) => void
  cancelListen: (callback: (message: DebugMessage) => void) => void
}

type ConnectionStatus = 'connected' | 'disconnected' | 'unknown'

type ResetLevel = 'none' | 'hard'

type DebugState = {
  paused?: boolean
  activeSpaceId?: string
}

type AttachMessage = {
  action: 'ECS_ATTACH'
  pageId: string
  debugId: string
  state: DebugState
  doc: string
  version?: number
  mode?: 'base'
  resetLevel?: ResetLevel
}

type DetachMessage = {
  action: 'ECS_DETACH'
  debugId: string
}

type DebugEditMessage = {
  action: 'ECS_DEBUG_EDIT'
  debugId: string
  diffs: string[]
  ephemeralUpdates: DeepReadonly<GraphObject>[]
}

type BaseEditMessage = {
  action: 'ECS_BASE_EDIT'
  debugId: string
  diffs: string[]
}

type ReadyMessage = {
  action: 'ECS_READY'
  sessionId: string
  pageId: string
  ua: string
  screenHeight: number
  screenWidth: number
  connectionStatus: ConnectionStatus
  simulatorId?: string
  version?: number
}

type CloseMessage = {
  action: 'ECS_CLOSE'
  sessionId: string
}

type WorldUpdateMessage = {
  action: 'ECS_WORLD_UPDATE'
  debugId: string
  diffs: string[]
  spaceId?: string
}

type TransformUpdateMessage = {
  action: 'ECS_TRANSFORM_UPDATE'
  debugId: string
  updatedTransforms: TransformMap
  deletedIds: string[]
}

type StateUpdateMessage = {
  action: 'ECS_STATE_UPDATE'
  debugId: string
  state: DebugState
}

type ResetSceneMessage = {
  action: 'ECS_RESET_SCENE'
  debugId: string
}

type RollcallMessage = {
  action: 'ECS_ROLLCALL'
}

type DocRefreshMessage = {
  action: 'ECS_DOC_REFRESH'
  debugId: string
  doc: string
}

type AttachConfirmMessage = {
  action: 'ECS_ATTACH_CONFIRM'
  debugId: string
}

type DebugMessage = AttachMessage | DetachMessage | DebugEditMessage | ReadyMessage |
  CloseMessage | WorldUpdateMessage | StateUpdateMessage | RollcallMessage | DocRefreshMessage |
  TransformUpdateMessage | BaseEditMessage | ResetSceneMessage | AttachConfirmMessage

type DebugStream = EventStreamManager

export type {
  DebugMessage,
  DebugStream,
  AttachMessage,
  DetachMessage,
  DebugEditMessage,
  BaseEditMessage,
  ReadyMessage,
  CloseMessage,
  WorldUpdateMessage,
  StateUpdateMessage,
  DocRefreshMessage,
  DebugState,
  ConnectionStatus,
  ResetLevel,
  AttachConfirmMessage,
}

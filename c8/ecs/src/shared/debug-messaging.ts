import type {DeepReadonly} from 'ts-essentials'

import type {GraphObject} from './scene-graph'
import type {TransformMap} from './transform-map'

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
  mode: 'base'
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

type EvalMessage = {
  action: 'EVAL'
  cmd: string
}

type DebugHudMessage = {
  action: 'DEBUG_HUD'
  enable: boolean
}

type MessageWithIds = {
  deviceId: string
  sessionId: string
  simulatorId: string | undefined
}

type DeviceInfo = {
  screenHeight: number
  screenWidth: number
  ua: string
}

type InitialDebugHudStatusMessage = MessageWithIds & DeviceInfo & {
  action: 'INITIAL_DEBUG_HUD_STATUS'
  status: boolean
}

type SetDebugHudStatus = MessageWithIds & {
  action: 'SET_DEBUG_HUD_STATUS'
  status: boolean
}

type SourceLocation = {
  file?: string
  line?: number | null
  column?: number | null
}

type StackTraceEntry = SourceLocation & {
  function: string | undefined
}

type StackTrace = (StackTraceEntry | null)[]

type ConsoleLog = {
  fn: string
  args: unknown[]
  timestamp: number
  sourceLocation?: SourceLocation
  stack?: StackTrace
}

type ConsoleActivityMessage = MessageWithIds & DeviceInfo & {
  action: 'CONSOLE_ACTIVITY'
  logs: ConsoleLog[]
}

type SessionStartMessage = {
  action: 'SESSION_START'
  deviceId: string
  sessionId: string
  timestamp: number
}

type DebugMessage = AttachMessage | DetachMessage | DebugEditMessage | ReadyMessage |
  CloseMessage | WorldUpdateMessage | StateUpdateMessage | RollcallMessage | DocRefreshMessage |
  TransformUpdateMessage | BaseEditMessage | ResetSceneMessage | AttachConfirmMessage
  | EvalMessage | DebugHudMessage
  | InitialDebugHudStatusMessage | SetDebugHudStatus | ConsoleActivityMessage | SessionStartMessage

type DebugCallback = (message: DebugMessage) => void

type DebugStream = {
  send: (message: DebugMessage) => void
  listen: (callback: DebugCallback) => void
  cancelListen: (callback: DebugCallback) => void
}

type ScopedDebugMessage = {
  appKey: string
  sessionId: string
  data: DebugMessage
}

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
  DebugCallback,
  EvalMessage,
  DebugHudMessage,
  InitialDebugHudStatusMessage,
  SetDebugHudStatus,
  ConsoleActivityMessage,
  SessionStartMessage,
  ScopedDebugMessage,
}

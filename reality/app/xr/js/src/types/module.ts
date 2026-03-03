import type * as Pipeline from './pipeline'
import type * as Callbacks from './callbacks'
import type {XrPermission} from './common'

/* eslint-disable import/group-exports */

export type EventListener = {
  event: Pipeline.EventName
  process: (data: unknown) => void
}

export type Module = {
  name: Pipeline.ModuleName
  listeners?: EventListener[]
  requiredPermissions?: XrPermission[]
} & Partial<Callbacks.CallbackNameToHandler>

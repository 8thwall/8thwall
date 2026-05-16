import {createListenerPool} from '@repo/reality/shared/listener-pool'
import type {ScopedDebugMessage} from '@repo/c8/ecs/src/shared/debug-messaging'

const fromDevicePool = createListenerPool<ScopedDebugMessage>()

const toDevicePool = createListenerPool<ScopedDebugMessage>()

export {
  fromDevicePool,
  toDevicePool,
}

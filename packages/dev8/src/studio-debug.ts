import {DEBUG_ACTOR_ID} from './shared/ecs/shared/sync-session'
import type {
  AttachMessage, DebugMessage, DebugStream, DetachMessage, ConnectionStatus,
} from './shared/ecs/shared/debug-messaging'
import type {IApplication} from './shared/ecs/shared/application-type'
import {stringToBytes} from './shared/ecs/shared/data'
import {TRANSFORM_MAP_VERSION} from './shared/ecs/shared/transform-map'
import {createBaseSyncSession} from './shared/ecs/shared/base-sync-session'

const resolveApplication = () => new Promise<IApplication>((resolve) => {
  if ((window as any).ecs && (window as any).ecs.application.getWorld()) {
    resolve((window as any).ecs.application)
  } else {
    const ecsInitListener = () => {
      if ((window as any).ecs && (window as any).ecs.application) {
        resolve((window as any).ecs.application)
      }
    }

    window.addEventListener('ecsInit', ecsInitListener, {once: true})
  }
})

const getInitializedApplication = () => {
  const application = (window as any).ecs?.application
  if (application?.getWorld()) {
    return application as IApplication
  }

  return null
}

const createStudioDebugManager = (
  sessionId: string, ua: string, simulatorId: string, eventStream: DebugStream
) => {
  const pageId = Math.random().toString(36).substring(7)
  const screenHeight = window.screen.height * window.devicePixelRatio
  const screenWidth = window.screen.width * window.devicePixelRatio

  let session: ReturnType<typeof createBaseSyncSession> | undefined

  let started = false

  let application: IApplication

  const handleAttach = async (msg: AttachMessage) => {
    if (session || msg.pageId !== pageId) {
      return
    }

    // NOTE(christoph): This should be instant because the crdt module is pre-loaded in ready().
    const crdtModule = await import('./shared/ecs/shared/crdt')
    const doc = crdtModule.loadSceneDoc(stringToBytes(msg.doc), DEBUG_ACTOR_ID)

    if (msg.mode === 'base') {
      session = createBaseSyncSession(
        msg.debugId,
        application,
        eventStream,
        msg.state,
        doc
      )
    } else {
      throw new Error('Unexpected mode in attach')
    }

    session.attach()
  }

  const detachSession = () => {
    session?.detach()
    session = undefined
  }

  const handleDetach = (msg: DetachMessage) => {
    if (!session || msg.debugId !== session.debugId) {
      return
    }
    detachSession()
  }

  const sendReady = (connectionStatus: ConnectionStatus) => {
    eventStream.send({
      action: 'ECS_READY',
      sessionId,
      pageId,
      screenHeight,
      screenWidth,
      ua,
      simulatorId,
      connectionStatus,
      version: TRANSFORM_MAP_VERSION,
    })
  }

  const handleRollCall = () => {
    let connectionStatus: ConnectionStatus = 'disconnected'
    if (session) {
      detachSession()
      connectionStatus = 'connected'
    }

    sendReady(connectionStatus)
  }

  const handleMessage = (msg: DebugMessage) => {
    switch (msg.action) {
      case 'ECS_ATTACH':
        handleAttach(msg)
        break
      case 'ECS_DETACH':
        handleDetach(msg)
        break
      case 'ECS_ROLLCALL':
        handleRollCall()
        break
      default:
    }
  }

  let bundleReadyPromise: Promise<void> | undefined
  const originalApplication: IApplication = (window as any).ecs?.application
  if (originalApplication) {
    const query = new URLSearchParams(window.location.search)
    if (query.get('liveSyncMode') === 'inline') {
    // NOTE(christoph): If we are running in an inline mode, we need to make init a noop
    // so that when the application calls init, it doesn't do anything.
    // Instead, we'll initialize the application on ECS_ATTACH.
      application = {...originalApplication}
      bundleReadyPromise = new Promise((resolve) => {
        originalApplication.init = async () => {
          resolve()
        }
      })
    }
  }

  return {
    ready: async () => {
      if (started) {
        return
      }
      started = true
      await bundleReadyPromise
      application = application || await resolveApplication()
      // Pre-load the crdt module
      await import('./shared/ecs/shared/crdt')
      sendReady('unknown')
      eventStream.listen(handleMessage)
    },
    close: () => {
      eventStream.send({action: 'ECS_CLOSE', sessionId})
    },
  }
}

export {
  createStudioDebugManager,
  getInitializedApplication,
}

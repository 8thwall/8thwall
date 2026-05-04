import React from 'react'
import type {DebugMessage} from '@ecs/shared/debug-messaging'
import {v4 as uuid} from 'uuid'
import {SceneDoc, createEmptySceneDoc} from '@ecs/shared/crdt'
import {bytesToString} from '@ecs/shared/data'

import type {SceneContext} from '../scene-context'
import useCurrentApp from '../../common/use-current-app'
import useActions from '../../common/use-actions'
import editorActions from '../../editor/editor-actions'
import {useStudioDebug} from '../../editor/app-preview/use-studio-debug-state'
import type {StudioDebugSession} from '../../editor/editor-reducer'
import {useChangeEffect} from '../../hooks/use-change-effect'
import {useStudioStateContext} from '../studio-state-context'
import {deriveActiveSpace} from './active-space'
import {useAppPreviewWindow} from '../../common/app-preview-window-context'
import {useWindowMessageHandler} from '../../hooks/use-window-message-handler'

type DebugSession = {
  debugId: string
  pageId: string
  baseDoc: SceneDoc
  simulatorId?: string
  activeSpaceId?: string
}

interface DebugSceneOptions {
  inlineSimulatorId: string
  baseScene: SceneContext['scene']
}

const EDITOR_ACTOR_ID = 'ed'  // Arbitrary hex digits

type RemoteDebugStatus = 'waiting' | 'attach-sent' | 'attach-confirmed'

const useRemoteScene = ({inlineSimulatorId, baseScene}: DebugSceneOptions) => {
  const debugSessionRef = React.useRef<DebugSession | null>(null)

  const [status, setStatus] = React.useState<RemoteDebugStatus>('waiting')

  const {studioDebugState} = useStudioDebug()
  const {currentSession, connected} = studioDebugState

  const stateCtx = useStudioStateContext()
  const {restartKey, isPreviewPaused: paused} = stateCtx.state

  const activeSpaceId = baseScene && deriveActiveSpace(baseScene, stateCtx.state.activeSpace)?.id

  const app = useCurrentApp()

  const {
    ensureSimulatorStateReady,
  } = useActions(editorActions)

  React.useEffect(() => {
    ensureSimulatorStateReady(app.appKey)
  }, [app.appKey])

  const {getInlinePreviewWindow} = useAppPreviewWindow()

  const sendData = (
    data: DebugMessage
  ) => {
    const targetWindow = getInlinePreviewWindow()
    targetWindow?.postMessage(data, '*')
  }

  const sendAttach = (session: Pick<StudioDebugSession, 'pageId' | 'simulatorId'>) => {
    const debugId = uuid()
    const {pageId, simulatorId} = session

    const doc = createEmptySceneDoc(EDITOR_ACTOR_ID)

    if (!baseScene) {
      return
    }
    doc.update(() => baseScene)
    debugSessionRef.current = {
      debugId,
      pageId,
      baseDoc: doc,
      simulatorId,
      activeSpaceId,
    }
    // Note(Julie): When reattaching during an ongoing session (refresh) we'll want to send the
    // current debug state to retain the previous behavior. This will change in the future to
    // handle other cases, e.g. switching to a different device with a running scene should not
    // be affected by the current editor state
    sendData({
      action: 'ECS_ATTACH',
      pageId,
      debugId,
      state: {
        paused,
        activeSpaceId,
      },
      doc: bytesToString(doc.save()),
      mode: 'base',
    })
    setStatus('attach-sent')
  }

  const sendDetach = () => {
    if (!debugSessionRef.current) {
      return
    }

    sendData({
      action: 'ECS_DETACH',
      debugId: debugSessionRef.current.debugId,
    })
    debugSessionRef.current = null
    setStatus('waiting')
  }

  const handleDebugMessage = (event: MessageEvent) => {
    const {data: msg} = event
    switch (msg.action) {
      case 'ECS_READY': {
        const {pageId, simulatorId} = msg
        const connectSimulator = simulatorId === inlineSimulatorId
        if (connectSimulator) {
          sendAttach({
            pageId,
            simulatorId,
          })
        }
        break
      }
      case 'ECS_ATTACH_CONFIRM': {
        const {debugId} = msg
        if (!debugSessionRef.current || debugSessionRef.current.debugId !== debugId) {
          return
        }
        setStatus('attach-confirmed')
        break
      }
      default:
    }
  }

  useWindowMessageHandler(handleDebugMessage)

  useChangeEffect(([prevSession, prevConnected]) => {
    if (prevConnected && !connected) {
      sendDetach()
      return
    }
    if (prevSession?.pageId !== currentSession?.pageId) {
      sendDetach()
    }
    if (currentSession && connected && currentSession?.pageId !== prevSession?.pageId) {
      sendAttach(currentSession)
    }
  }, [currentSession, connected] as const)

  React.useEffect(() => {
    if (!debugSessionRef.current) {
      return
    }
    sendData({
      action: 'ECS_STATE_UPDATE',
      debugId: debugSessionRef.current.debugId,
      state: {paused},
    })
  }, [paused])

  React.useEffect(() => {
    if (!debugSessionRef.current || !activeSpaceId ||
      debugSessionRef.current.activeSpaceId === activeSpaceId
    ) {
      return
    }
    sendData({
      action: 'ECS_STATE_UPDATE',
      debugId: debugSessionRef.current.debugId,
      state: {activeSpaceId},
    })
    debugSessionRef.current.activeSpaceId = activeSpaceId
  }, [activeSpaceId])

  useChangeEffect(([prevRestartKey]) => {
    if (!debugSessionRef.current) {
      return
    }
    if (prevRestartKey && prevRestartKey !== restartKey) {
      sendData({
        action: 'ECS_RESET_SCENE',
        debugId: debugSessionRef.current.debugId,
      })
    }
  }, [restartKey])

  React.useEffect(() => {
    if (!debugSessionRef.current) {
      return
    }
    const {baseDoc} = debugSessionRef.current
    const prevBaseDoc = baseDoc.raw()
    baseDoc.update(() => baseScene)
    sendData({
      action: 'ECS_BASE_EDIT',
      debugId: debugSessionRef.current.debugId,
      diffs: baseDoc.getChanges(prevBaseDoc).map(bytesToString),
    })
  }, [baseScene])

  return {status}
}

export {
  useRemoteScene,
}

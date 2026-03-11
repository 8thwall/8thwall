import React, {useRef, useEffect} from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import {moonlight, brandBlack, blueberry, gray4, gray5, persimmon} from '../static/styles/settings'
import {useWebsocketHandler} from '../hooks/use-websocket-handler'
import {STUDIO_USE_SOCKET_URL} from '../common/websocket-constants'
import {FloatingTray} from '../ui/components/floating-tray'
import {FloatingIconButton} from '../ui/components/floating-icon-button'
import {useStudioAgentStateContext} from './studio-agent/studio-agent-context'
import WebSocketPool from '../websockets/websocket-pool'
import {useStudioStateContext} from './studio-state-context'
import {useSceneContext} from './scene-context'
import {sceneInterfaceContext} from './scene-interface-context'
import {LEFT_PANEL_WIDTH} from './floating-left-panel'
import {makeStudioAgentApi} from './studio-agent/studio-agent-api'
import type {MessageRequest, MessageResponse} from './studio-agent/types'
import {useStudioComponentsContext} from './studio-components-context'
import {useDerivedScene} from './derived-scene-context'
import {useAuthFetch} from '../hooks/use-auth-fetch'
import {useSelector} from '../hooks'
import {INTERNAL_MESSAGES} from './studio-agent/constants'
import {combine} from '../common/styles'
import {useLocalSyncContext} from './local-sync-context'
import {useUpdateEffect} from '../hooks/use-change-effect'
import {useCurrentGit} from '../git/hooks/use-current-git'
import {useEnclosedApp} from '../apps/enclosed-app-context'
import imageTargetsActions from '../image-targets/actions'
import useActions from '../common/use-actions'
import {IMAGE_TARGET_STUDIO_USE_GALLERY_ID} from '../apps/image-targets/image-target-constants'
import useCurrentAccount from '../common/use-current-account'

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const useStyles = createUseStyles({
  // Putting the container on the left so the right hand side can be used for the simulator
  floatingContainer: {
    'left': `calc(${LEFT_PANEL_WIDTH} + 25px)`,
    'width': `calc(${LEFT_PANEL_WIDTH} + 100px)`,
    'top': '11px',
    'maxHeight': `calc(${LEFT_PANEL_WIDTH} + 200px)`,
    'display': 'flex',
    'position': 'absolute',
    'fontSize': '12px',
    'pointerEvents': 'auto',
    'borderRadius': '0.5em',
    'background': moonlight,
    'flexDirection': 'column',
  },
  messagesContainer: {
    'display': 'flex',
    'flexDirection': 'column',
    'scrollBehavior': 'smooth',
    'padding': '0.5rem',
    'borderRadius': '0.5em',
    'overflowY': 'auto',
    'flex': '1',
    'minHeight': 0,
  },
  messageItem: {
    'marginBottom': '0.5rem',
    'padding': '0.5rem',
    'backgroundColor': moonlight,
    'borderRadius': '0.25rem',
    'borderLeft': `3px solid ${blueberry}`,
    '& h4': {
      margin: '0 0 0.5rem 0',
      color: blueberry,
      fontWeight: 600,
    },
    '& p': {
      margin: '0',
      color: gray5,
      wordBreak: 'break-word',
      whiteSpace: 'pre-wrap',
    },
  },
  messageItemInternal: {
    'borderLeft': `3px solid ${persimmon}`,
    '& h4': {
      color: persimmon,
    },
  },
  emptyMessage: {
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    'height': '100%',
    'textAlign': 'center',
    'padding': '1rem',
    'color': gray4,
    'fontStyle': 'italic',
  },
  clearButton: {
    'color': brandBlack,
    'border': `1px solid ${brandBlack}`,
    'borderRadius': '0.25rem',
    'cursor': 'pointer',
    'marginTop': 'auto',
    'padding': '0.5rem',
  },
})

const AiAssistantModal: React.FC = () => {
  const classes = useStyles()
  // NOTE(dat): Give us the viewport camera type, which object is selected, the active space
  const sceneInterfaceCtx = React.useContext(sceneInterfaceContext)
  const studioAgentCtx = useStudioAgentStateContext()
  // List of prompts for the demo
  // Replace all objects in my scene with 100 spheres in a circle with random colors
  // Move all my objects a random amount in the y axis
  // Organize all my objects in the scene into a grid around the origin
  // Organize all my objects in the scene into a pyramid
  // Replace the scene with a table made of primitives
  // Replace the scene with a house made of primitives
  const messagesEndRef = useRef<HTMLDivElement>(null)
  const {playedMsgs} = studioAgentCtx.state

  useEffect(() => {
    messagesEndRef.current?.scrollIntoView({behavior: 'smooth'})
  }, [playedMsgs])

  const undoStackSteps = studioAgentCtx.state.undoStack.steps
  const focusChange = (requestId: string) => {
    const undoStep = undoStackSteps[requestId]
    if (undoStep?.focalPoint) {
      sceneInterfaceCtx.cameraLookAt(undoStep.focalPoint)
    }
    if (undoStep?.affectedSceneIds?.length) {
      sceneInterfaceCtx.selectIds(undoStep.affectedSceneIds)
    }
  }

  return (
    <div className={classes.floatingContainer}>
      <div className={classes.messagesContainer}>
        {playedMsgs.length > 0
          ? (
            <>
              {playedMsgs.map(({requestId, action, parameters}) => (
                <div
                  key={requestId}
                  className={combine(
                    classes.messageItem,
                    INTERNAL_MESSAGES.includes(action) && classes.messageItemInternal
                  )}
                >
                  <h4>{action}</h4>
                  <p>{JSON.stringify(parameters, null, 2)}</p>
                  {undoStackSteps[requestId]?.focalPoint &&
                    <button type='button' onClick={() => focusChange(requestId)}>Focus</button>
                  }
                </div>
              ))}
              <div ref={messagesEndRef} />
              <button
                type='button'
                className={classes.clearButton}
                onClick={() => studioAgentCtx.setState({playedMsgs: []})}
              >
                Clear Messages
              </button>
            </>
          )
          : (
            <div className={classes.emptyMessage}>
              No messages received. Prompt in your MCP client please.
            </div>
          )}
      </div>
    </div>
  )
}

const AiButton = ({onClick}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const {state: studioAgentState} = useStudioAgentStateContext()
  return (
    <FloatingTray>
      <FloatingIconButton
        stroke='ring'
        onClick={onClick}
        isActive={studioAgentState.open}
        text={studioAgentState.open
          ? t('scene_page.button.hide_ai_modal', {ns: 'cloud-studio-pages'})
          : t('scene_page.button.show_ai_modal', {ns: 'cloud-studio-pages'})
          }
      />
    </FloatingTray>
  )
}

const AiAssistant = () => {
  const {t} = useTranslation(['cloud-studio-pages'])
  // @ts-expect-error TODO(christoph): Clean up
  const accountUuid = useSelector(state => state.accounts.selectedAccount)
  const authFetch = useAuthFetch()
  const studioStateCtx = useStudioStateContext()
  const sceneCtx = useSceneContext()
  const derivedScene = useDerivedScene()
  const studioAgentCtx = useStudioAgentStateContext()
  const studioComponentCtx = useStudioComponentsContext()
  const localSyncCtx = useLocalSyncContext()
  const git = useCurrentGit()
  const app = useEnclosedApp()
  const user: unknown = null
  const account = useCurrentAccount()

  const {fetchImageTargetsForApp} = useActions(imageTargetsActions)
  const imageTargetFetch = () => fetchImageTargetsForApp(
    app.uuid, IMAGE_TARGET_STUDIO_USE_GALLERY_ID, 500
  )

  const StudioAgentApi = makeStudioAgentApi(sceneCtx, derivedScene, studioStateCtx, studioAgentCtx,
    studioComponentCtx, authFetch, accountUuid, app.uuid, git, imageTargetFetch, t)

  useWebsocketHandler(async (msg: MessageRequest) => {
    let response: Partial<MessageResponse>
    const fn = StudioAgentApi[msg.action]
    try {
      response = await fn({...msg.parameters}, msg.requestId)
    } catch (error) {
      response = {
        isError: true,
        error: error instanceof Error ? error.message : error,
      }
    }

    const outChannel = msg.sender.startsWith('vscode')
      ? `vscode/${localSyncCtx.appKey}`
      : msg.sender

    WebSocketPool.broadcastRawMessage<MessageResponse>({baseUrl: STUDIO_USE_SOCKET_URL}, {
      requestId: msg.requestId,
      action: msg.action,
      type: 'publish',
      channel: outChannel,
      response,
    })

    const shouldHideMessage = INTERNAL_MESSAGES.includes(msg.action) && !BuildIf.ALL_QA
    if (msg.action && !shouldHideMessage) {
      studioAgentCtx.setState({playedMsgs: [...studioAgentCtx.state.playedMsgs, msg]})
    }
  }, {baseUrl: STUDIO_USE_SOCKET_URL})

  React.useEffect(() => {
    WebSocketPool.broadcastRawMessage({baseUrl: STUDIO_USE_SOCKET_URL}, {
      type: 'subscribe',
      channel: 'studio-use',
    })

    WebSocketPool.broadcastRawMessage({baseUrl: STUDIO_USE_SOCKET_URL}, {
      type: 'publish',
      channel: `vscode/${localSyncCtx.appKey}`,
      data: {
        action: 'projectStart',
        // @ts-expect-error TODO(christoph): Clean up
        givenName: user?.given_name,
        // @ts-expect-error TODO(christoph): Clean up
        familyName: user?.family_name,
        // @ts-expect-error TODO(christoph): Clean up
        email: user?.email,
        workspaceName: account?.name,
      },
    })
  }, [])

  useUpdateEffect(() => {
    const {appKey, localBuildUrl} = localSyncCtx
    if (!appKey || !localBuildUrl) {
      return
    }

    const url = new URL(localBuildUrl)
    const localWebSocketUrl = `ws://${url.host}/ws`

    WebSocketPool.broadcastRawMessage({baseUrl: STUDIO_USE_SOCKET_URL}, {
      type: 'publish',
      channel: 'mcp8',
      // channel: `mcp8-${localSyncCtx.appKey}`, --- IGNORE ---
      data: {
        appKey,
        localWebSocketUrl,
      },
    })
  }, [localSyncCtx.appKey, localSyncCtx.localBuildUrl])

  return BuildIf.ALL_QA &&
    <AiButton
      onClick={() => {
        studioAgentCtx.setState({open: !studioAgentCtx.state.open})
      }}
    />
}

export {
  AiAssistantModal,
  AiButton,
  AiAssistant,
}

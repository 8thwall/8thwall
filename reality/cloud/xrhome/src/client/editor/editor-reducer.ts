import localForage from 'localforage'
import type {DeepReadonly} from 'ts-essentials'

import type {XrConfig} from '@ecs/shared/scene-graph'

import ConsoleLogStreams from './console-log-streams'
import type {ILogStream} from './logs/types'
import {getSessionTitle} from './debug-session-info'

const INPUT_HISTORY_LIMIT = 100
const DEFAULT_HEIGHT_PERCENT = 0.4

type AspectOptionId = 'responsive'

type StudioDebugSession = {
  id: string
  title: string
  pageId: string
  enableTransformMap: boolean
  autoConnected?: boolean
  simulatorId?: string
}

type StudioDebugState = DeepReadonly<{
  currentSession?: StudioDebugSession
  studioDebugSessions?: StudioDebugSession[]
  connected?: boolean
}>

type ScopedEditorState = DeepReadonly<{
  logStreams: ILogStream[]
  isNewClientWithoutBuild?: boolean
  simulatorState?: SimulatorState
  studioDebugState?: StudioDebugState
  fileBrowserHeightPercent: number
}>

const initialScopedState: ScopedEditorState = {
  logStreams: [],
  studioDebugState: {
    studioDebugSessions: [],
  },
  simulatorState: {
    responsive: true,
    aspectOptionId: 'responsive',
  },
  fileBrowserHeightPercent: DEFAULT_HEIGHT_PERCENT,
}

type SimulatorState = DeepReadonly<{
  aspectOptionId?: AspectOptionId
  inlinePreviewVisible?: boolean
  width?: number
  height?: number
  isLandscape?: boolean
  sequence?: string
  variation?: string
  isLiveView?: boolean
  studioPause?: boolean
  isPaused?: boolean
  start?: number
  end?: number
  responsive?: boolean
  // Need to keep this state because navigating away from Studio would close inline simulator.
  // Coming back will result in a different simulatorId. However, we still want to connect
  // immediately to this inline simulator.
  inlinePreviewDebugActive?: boolean
  cameraXrConfig?: XrConfig
  panelWidth?: number
  panelHeight?: number
  responsiveWidth?: number
  responsiveHeight?: number
  imageTargetName?: string
  imageTargetType?: string
  imageTargetUrl?: string
  imageTargetOriginalUrl?: string
  imageTargetMetadata?: string
  imageTargetQuaternion?: [number, number, number, number]
  mockCoordinates?: {lat: number, lng: number}
  mockLat?: number
  mockLng?: number
  mockCoordinateValue?: string
}>

type HmdLinkState = {
  code: string
  deviceName: string
  shouldAutoConnectNext: boolean
};

type EditorState = DeepReadonly<{
  byKey: Record<string, ScopedEditorState>
  tabState: {}
  consoleInput: {
    commandHistory: string[]
    isCommandHistoryLoaded: boolean
  }
  leftPaneSplitSize: number
  modulePaneSplitSize: number
  previewLinkDebugMode: boolean
  hmdLink: HmdLinkState
}>

const initialState: EditorState = {
  byKey: {},
  tabState: {},
  consoleInput: {
    commandHistory: [],
    isCommandHistoryLoaded: false,
  },
  leftPaneSplitSize: 0,
  modulePaneSplitSize: 0,
  previewLinkDebugMode: false,
  hmdLink: {
    code: '',
    deviceName: '',
    shouldAutoConnectNext: false,
  },
}

type ScopedUpdate = Partial<ScopedEditorState> |
                    ((state: ScopedEditorState) => Partial<ScopedEditorState>)

const getUpdateForScopedState = (
  state: EditorState, key: string, updater: ScopedUpdate
) => {
  const scopedEditorState = state.byKey[key] || initialScopedState
  const update = typeof updater === 'function' ? updater(scopedEditorState) : updater
  const scopedEditorStateWithUpdate = {...scopedEditorState, ...update}
  const byKeyWithUpdate = {...state.byKey, [key]: scopedEditorStateWithUpdate}
  return {...state, byKey: byKeyWithUpdate}
}

type Handler = (state: EditorState, action: any) => EditorState

const handlers: Record<string, Handler> = {
  'CONSOLE_INPUT_ADD': (state, action) => {
    const {command} = action
    const {commandHistory} = state.consoleInput

    // Empty command or duplicate command.
    const trimmedCommand = command?.trim()
    if (!trimmedCommand || trimmedCommand === commandHistory[0]) {
      return state
    }

    const truncatedCommandHistory = commandHistory.length >= INPUT_HISTORY_LIMIT
      ? commandHistory.slice(0, commandHistory.length - 1)
      : commandHistory

    const updatedConsoleInput = {
      ...state.consoleInput,
      commandHistory: [command, ...truncatedCommandHistory],
    }

    localForage.setItem('command-history', updatedConsoleInput.commandHistory)
    return {...state, consoleInput: updatedConsoleInput}
  },
  'CONSOLE_INPUT_HISTORY_LOAD': (state, action) => {
    const {inputHistory} = action
    const updatedConsoleInput = {
      ...state.consoleInput,
      commandHistory: inputHistory || [],
      isCommandHistoryLoaded: true,
    }
    return {...state, consoleInput: updatedConsoleInput}
  },
  'USER_LOGOUT': () => ({...initialState}),
  'EDITOR_LOGS_ADD': (state, action) => {
    const {key, logs} = action

    const editorState = state.byKey[key] || {...initialScopedState}

    const stateUpdateLogs = ConsoleLogStreams.updateLogStates(logs, editorState)
    return getUpdateForScopedState(state, key, {...editorState, ...stateUpdateLogs})
  },
  'EDITOR_LOG_STREAM_DELETE': (state, action) => {
    const {key, logStreamName} = action

    const editorState = state.byKey[key]
    if (!editorState) {
      return state
    }

    return getUpdateForScopedState(state, key, {
      logStreams: editorState.logStreams.filter(l => l.name !== logStreamName),
    })
  },
  'EDITOR_LOG_STREAM_CLEAR': (state, action) => {
    const {key, logStreamName} = action

    const appEditorState = state.byKey[key]

    const logStreamIndex = appEditorState.logStreams.findIndex(s => s.name === logStreamName)

    if (logStreamIndex !== -1) {
      const newLogStreams = [...appEditorState.logStreams]
      newLogStreams[logStreamIndex] = {...newLogStreams[logStreamIndex], logs: []}
      return getUpdateForScopedState(state, key, {logStreams: newLogStreams})
    } else {
      return state
    }
  },
  'EDITOR_LOG_STREAM_CLEAR_ON_RUN': (state, action) => {
    const {key, logStreamName, timestamp} = action
    const {logStreams} = state.byKey[key]
    const logStreamIndex = logStreams.findIndex(ls => ls.name === logStreamName)

    if (logStreams[logStreamIndex]?.isClearOnRunActive) {
      const newLogStreams = [...logStreams]
      const clearedLogs = newLogStreams[logStreamIndex].logs.filter(l => l.timestamp > timestamp)
      newLogStreams[logStreamIndex] = {...newLogStreams[logStreamIndex], logs: clearedLogs}
      return getUpdateForScopedState(state, key, {logStreams: newLogStreams})
    } else {
      return state
    }
  },
  'EDITOR_IS_CLEAR_ON_RUN_ACTIVE_TOGGLE': (state, action) => {
    const {key, logStreamName} = action
    const {logStreams} = state.byKey[key]
    const logStreamIndex = logStreams.findIndex(s => s.name === logStreamName)
    if (logStreamIndex !== -1) {
      const newLogStreams = [...logStreams]
      const {isClearOnRunActive} = newLogStreams[logStreamIndex]
      newLogStreams[logStreamIndex] = {
        ...newLogStreams[logStreamIndex],
        isClearOnRunActive: !isClearOnRunActive,
      }
      return getUpdateForScopedState(state, key, {logStreams: newLogStreams})
    } else {
      return state
    }
  },
  'SET_LEFT_PANE_SPLIT_SIZE': (state, action) => (
    {...state, leftPaneSplitSize: action.size}
  ),
  'SET_MODULE_PANE_SPLIT_SIZE': (state, action) => (
    {...state, modulePaneSplitSize: action.size}
  ),
  'SET_STUDIO_FILE_BROWSER_HEIGHT_PERCENT': (state, action) => {
    const {key, size} = action
    return getUpdateForScopedState(state, key, {
      fileBrowserHeightPercent: size ?? DEFAULT_HEIGHT_PERCENT,
    })
  },
  'SET_LOG_STREAM_DEBUG_HUD_STATUS': (state, action) => {
    const {key, sessionId, status} = action
    const {logStreams} = state.byKey[key]
    const logStreamIndex = logStreams.findIndex(ls => ls.name === sessionId)
    let newLogStreams
    if (logStreamIndex !== -1) {
      newLogStreams = [...logStreams]
      newLogStreams[logStreamIndex] = {...logStreams[logStreamIndex], isDebugHudActive: status}
    } else {
      const {deviceId, ua, screenHeight, screenWidth} = action
      const newLogStream = ConsoleLogStreams.createStream(
        sessionId, deviceId, [], ua, screenHeight, screenWidth, status, true
      )
      newLogStreams = ConsoleLogStreams.addStreamToStreams(newLogStream, logStreams)
    }
    return getUpdateForScopedState(state, key, {logStreams: newLogStreams})
  },
  'SET_PREVIEW_LINK_DEBUG_MODE': (state, action) => (
    {...state, previewLinkDebugMode: action.status}
  ),
  'SET_HMD_LINK_STATE': (state, action) => (
    {
      ...state,
      hmdLink: {
        ...state.hmdLink, ...action.hmdLink,
      },
    }
  ),
  'SET_SIMULATOR_STATE': (state, action) => getUpdateForScopedState(state, action.appKey, {
    simulatorState: action.status,
  }),
  'UPDATE_SIMULATOR_STATE': (state, action) => getUpdateForScopedState(state, action.appKey, s => ({
    simulatorState: {
      ...s?.simulatorState,
      ...action.status,
    },
  })),
  'EDITOR_SET_STUDIO_DEBUG_SESSION': (state, action) => {
    const {
      appKey, ua, simulatorId, pageId, screenWidth, screenHeight, sessionId, enableTransformMap,
      autoConnected,
    } = action
    const title = getSessionTitle(ua, screenHeight, screenWidth, pageId)
    const newSession = {
      id: sessionId, title, pageId, simulatorId, enableTransformMap, autoConnected,
    }
    return getUpdateForScopedState(state, appKey, (s) => {
      const studioDebugSessions = s?.studioDebugState?.studioDebugSessions ?? []
      const sessionIdx = studioDebugSessions?.findIndex(ds => (ds.id === sessionId))
      return {
        studioDebugState: {
          currentSession: sessionIdx === -1 ? newSession : studioDebugSessions[sessionIdx],
          studioDebugSessions: sessionIdx === -1
            ? [...studioDebugSessions, newSession]
            : studioDebugSessions,
          connected: true,
        },
      }
    })
  },
  'EDITOR_ADD_STUDIO_DEBUG_SESSION': (state, action) => {
    const {
      appKey, ua, simulatorId, pageId, screenWidth, screenHeight, sessionId, enableTransformMap,
    } = action
    const title = getSessionTitle(ua, screenHeight, screenWidth, pageId)
    const session = {id: sessionId, title, pageId, simulatorId, enableTransformMap}
    return getUpdateForScopedState(state, appKey, (s) => {
      const currentSession = s?.studioDebugState?.currentSession
      const studioDebugSessions = s?.studioDebugState?.studioDebugSessions ?? []
      const sessionIdx = studioDebugSessions?.findIndex(ds => (ds.id === sessionId))
      const updatedDebugSessions = [...studioDebugSessions]
      let updatedSession = session
      if (sessionIdx > -1) {
        updatedSession = {
          ...studioDebugSessions[sessionIdx],
          ...session,
        }
        updatedDebugSessions[sessionIdx] = updatedSession
      } else {
        updatedDebugSessions.push(updatedSession)
      }
      return {
        studioDebugState: {
          ...s?.studioDebugState,
          currentSession: currentSession?.id === sessionId ? updatedSession : currentSession,
          studioDebugSessions: updatedDebugSessions,
        },
      }
    })
  },
  'EDITOR_REMOVE_STUDIO_DEBUG_SESSION': (state, action) => {
    const {appKey, sessionId} = action
    return getUpdateForScopedState(state, appKey, (s) => {
      const currentSession = s?.studioDebugState?.currentSession
      const studioDebugSessions = s?.studioDebugState?.studioDebugSessions ?? []
      const currentSessionRemoved = currentSession?.id === sessionId
      return {
        studioDebugState: {
          currentSession: currentSessionRemoved ? undefined : currentSession,
          studioDebugSessions: [
            ...studioDebugSessions?.filter(session => session.id !== sessionId),
          ],
          connected: currentSessionRemoved ? false : s?.studioDebugState?.connected,
        },
      }
    })
  },
  'EDITOR_FILTER_STUDIO_DEBUG_SESSIONS': (state, action) => {
    const {appKey, availableSessionIds} = action
    return getUpdateForScopedState(state, appKey, (s) => {
      const studioDebugSessions = s?.studioDebugState?.studioDebugSessions ?? []
      return {
        studioDebugState: {
          ...s.studioDebugState,
          studioDebugSessions: [
            ...studioDebugSessions.filter(ds => availableSessionIds[ds.id]),
          ],
        },
      }
    })
  },
  'EDITOR_DEBUG_SESSION_SELECT': (state, action) => (
    getUpdateForScopedState(state, action.appKey, (s) => {
      const studioDebugSessions = s?.studioDebugState?.studioDebugSessions ?? []
      const currentSessionIdx = studioDebugSessions?.findIndex(ds => (ds.id === action.session.id))
      const updatedDebugSessions = [...studioDebugSessions]
      if (currentSessionIdx > -1) {
        updatedDebugSessions[currentSessionIdx] = {
          ...action.session,
        }
      }
      return {
        studioDebugState: {
          ...s?.studioDebugState,
          studioDebugSessions: updatedDebugSessions,
          currentSession: action.session,
          connected: true,
        },
        simulatorState: {
          ...s?.simulatorState,
          inlinePreviewDebugActive: action.inlinePreviewSession,
        },
      }
    })),
  'EDITOR_DEBUG_SESSION_DISCONNECT': (state, action) => (
    getUpdateForScopedState(state, action.appKey, s => ({
      studioDebugState: {
        ...s?.studioDebugState,
        connected: false,
      },
    }))
  ),
  'EDITOR_DEBUG_SESSION_CANCEL': (state, action) => (
    getUpdateForScopedState(state, action.appKey, s => ({
      studioDebugState: {
        ...s?.studioDebugState,
        currentSession: undefined,
        connected: false,
      },
    }))
  ),
  'EDITOR_SELECT_DEBUG_SIMULATOR_SESSION': (state, action) => {
    const {appKey, simulatorId} = action
    return getUpdateForScopedState(state, appKey, (s) => {
      const currentSession = s?.studioDebugState?.currentSession
      const studioDebugSessions = s?.studioDebugState?.studioDebugSessions ?? []
      const sessionIdx = studioDebugSessions.findIndex(ds => (ds.simulatorId === simulatorId))
      return {
        studioDebugState: {
          ...s?.studioDebugState,
          currentSession: sessionIdx === -1 ? currentSession : studioDebugSessions[sessionIdx],
          connected: true,
        },
        simulatorState: {
          ...s?.simulatorState,
          inlinePreviewDebugActive: true,
        },
      }
    })
  },
  'EDITOR_REMOVE_DEBUG_SIMULATOR_SESSION': (state, action) => {
    const {appKey, simulatorId} = action
    return getUpdateForScopedState(state, appKey, (s) => {
      const currentSession = s?.studioDebugState?.currentSession
      const studioDebugSessions = s?.studioDebugState?.studioDebugSessions ?? []
      const connected = s?.studioDebugState?.connected
      return {
        studioDebugState: {
          currentSession: currentSession?.simulatorId === simulatorId ? undefined : currentSession,
          studioDebugSessions: [
            ...studioDebugSessions.filter(ds => ds.simulatorId !== simulatorId),
          ],
          connected: currentSession?.simulatorId === simulatorId ? false : connected,
        },
        simulatorState: {
          ...s?.simulatorState,
          inlinePreviewDebugActive: currentSession?.simulatorId === simulatorId
            ? false
            : !!s?.simulatorState?.inlinePreviewDebugActive,
        },
      }
    })
  },
  'EDITOR_CLIENT_CREATED': (state, action) => (
    getUpdateForScopedState(state, action.key, {isNewClientWithoutBuild: true})
  ),
  'EDITOR_SIMULATOR_BUILD_REQUIRED': (state, action) => (
    getUpdateForScopedState(state, action.key, {isNewClientWithoutBuild: true})
  ),
  'EDITOR_CLIENT_BUILT': (state, action) => (
    getUpdateForScopedState(state, action.key, {isNewClientWithoutBuild: false})
  ),
}

const Reducer = (state = {...initialState}, action) => (
  handlers[action.type]?.(state, action) || state
)

export default Reducer

export type {
  EditorState,
  ScopedEditorState,
  SimulatorState,
  StudioDebugState,
  StudioDebugSession,
  HmdLinkState,
  AspectOptionId,
}

export {
  DEFAULT_HEIGHT_PERCENT,
}

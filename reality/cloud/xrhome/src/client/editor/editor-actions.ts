/* eslint-disable local-rules/hardcoded-copy */
import localForage from 'localforage'

import {dispatchify} from '../common'
import type {AsyncThunk, DispatchifiedActions} from '../common/types/actions'
import type {SimulatorState, StudioDebugSession} from './editor-reducer'

const getStudioFileBrowserHeightPercentKey = (repoId: string) => (
  `studio-file-browser-height-percent-${repoId}`
)

const addConsoleInput = (command: string) => (dispatch) => {
  dispatch({type: 'CONSOLE_INPUT_ADD', command})
}

const deleteEditorLogStream = (key, logStreamName) => (dispatch) => {
  dispatch({type: 'EDITOR_LOG_STREAM_DELETE', key, logStreamName})
}

const clearEditorLogStream = (key, logStreamName) => (dispatch) => {
  dispatch({type: 'EDITOR_LOG_STREAM_CLEAR', key, logStreamName})
}

const loadConsoleInputHistory = () => async (dispatch, getState) => {
  const {isCommandHistoryLoaded} = getState().editor.consoleInput
  if (!isCommandHistoryLoaded) {
    const inputHistory = await localForage.getItem('command-history')
    dispatch({type: 'CONSOLE_INPUT_HISTORY_LOAD', inputHistory})
  }
}

const toggleIsClearOnRunActive = (key: string, logStreamName: string) => (dispatch) => {
  dispatch({type: 'EDITOR_IS_CLEAR_ON_RUN_ACTIVE_TOGGLE', key, logStreamName})
}

const setFileBrowserHeightPercent = (key: string, size?: number) => (dispatch) => {
  dispatch({type: 'SET_STUDIO_FILE_BROWSER_HEIGHT_PERCENT', key, size})
}

const saveFileBrowserHeightPercent = (key: string, size?: number) => async (dispatch) => {
  await localForage.setItem(getStudioFileBrowserHeightPercentKey(key), size)
  dispatch({type: 'SET_STUDIO_FILE_BROWSER_HEIGHT_PERCENT', key, size})
}

const setLogStreamDebugHudStatus = (
  key: string, sessionId: string, status: boolean
) => (dispatch) => {
  dispatch({type: 'SET_LOG_STREAM_DEBUG_HUD_STATUS', key, sessionId, status})
}

const setPreviewLinkDebugModeSelected = (status: boolean) => (dispatch) => {
  localForage.setItem('preview-link-debug-mode', status)
  dispatch({type: 'SET_PREVIEW_LINK_DEBUG_MODE', status})
}

const loadPreviewLinkDebugModeSelected = () => async (dispatch) => {
  const status = await localForage.getItem('preview-link-debug-mode')
  dispatch({type: 'SET_PREVIEW_LINK_DEBUG_MODE', status})
}

const simulatorStateKey = (appKey: string) => `simulator-state-${appKey}`

const updateSimulatorState = (
  appKey: string,
  status: Partial<SimulatorState>
): AsyncThunk<void> => async (dispatch, getState) => {
  dispatch({type: 'UPDATE_SIMULATOR_STATE', appKey, status})
  const updatedState = getState().editor.byKey[appKey].simulatorState
  await localForage.setItem(simulatorStateKey(appKey), updatedState)
}

const loadSimulatorState = (appKey: string): AsyncThunk<void> => async (dispatch) => {
  const status = await localForage.getItem(simulatorStateKey(appKey)) || {}
  dispatch({type: 'SET_SIMULATOR_STATE', appKey, status})
}

const ensureSimulatorStateReady = (
  appKey: string
): AsyncThunk<void> => async (dispatch, getState) => {
  const scopedState = getState().editor.byKey[appKey]
  if (!scopedState?.simulatorState) {
    await dispatch(loadSimulatorState(appKey))
  }
}

const selectDebugSession = (
  appKey: string, session: StudioDebugSession, inlinePreviewSession: boolean
): AsyncThunk<void> => async (dispatch, getState) => {
  dispatch({type: 'EDITOR_DEBUG_SESSION_SELECT', appKey, session, inlinePreviewSession})
  const updatedState = getState().editor.byKey[appKey].simulatorState
  await localForage.setItem(simulatorStateKey(appKey), updatedState)
}

const disconnectDebugSession = (appKey: string) => (dispatch) => {
  dispatch({type: 'EDITOR_DEBUG_SESSION_DISCONNECT', appKey})
}

const selectSimulatorSession = (
  appKey: string, simulatorId: string
): AsyncThunk<void> => async (dispatch, getState) => {
  dispatch({type: 'EDITOR_SELECT_DEBUG_SIMULATOR_SESSION', appKey, simulatorId})
  const updatedState = getState().editor.byKey[appKey]?.simulatorState
  await localForage.setItem(simulatorStateKey(appKey), updatedState)
}

const removeSimulatorSession = (
  appKey: string, simulatorId: string
): AsyncThunk<void> => async (dispatch, getState) => {
  dispatch({type: 'EDITOR_REMOVE_DEBUG_SIMULATOR_SESSION', appKey, simulatorId})
  const updatedState = getState().editor.byKey[appKey]?.simulatorState
  await localForage.setItem(simulatorStateKey(appKey), updatedState)
}

const rawActions = {
  addConsoleInput,
  clearEditorLogStream,
  deleteEditorLogStream,
  disconnectDebugSession,
  ensureSimulatorStateReady,
  loadConsoleInputHistory,
  loadPreviewLinkDebugModeSelected,
  removeSimulatorSession,
  saveFileBrowserHeightPercent,
  selectDebugSession,
  selectSimulatorSession,
  setFileBrowserHeightPercent,
  setLogStreamDebugHudStatus,
  setPreviewLinkDebugModeSelected,
  toggleIsClearOnRunActive,
  updateSimulatorState,
}

export type EditorActions = DispatchifiedActions<typeof rawActions>

export default dispatchify(rawActions)

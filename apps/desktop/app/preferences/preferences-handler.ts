import type {
  HubPreferences, InstalledPrograms,
} from '@repo/reality/shared/desktop/preferences-types'
import {dialog} from 'electron'

import {makeJsonResponse} from '../../json-response'
import {getPreference, setPreference} from '../../local-preferences'
import {withErrorHandlingResponse} from '../../errors'
import {getAvailableEditors} from './code-editor'
import {branches, methods, RequestHandler} from '../../requests'

const loadPreferences = (): HubPreferences => {
  const codeEditorPath = getPreference('codeEditorProgram') || ''
  return {
    codeEditorPath,
    firstTimeStatus: getPreference('firstTimeStatus') === 'complete' ? 'complete' : 'pending',
    theme: getPreference('theme') as 'light' | 'dark' | 'system' || 'system',
  }
}

const handleGetPreferences: RequestHandler = () => (
  makeJsonResponse(loadPreferences())
)

const handlePatchPreferences: RequestHandler = async (request) => {
  const body = await request.json()
  const {codeEditorPath, firstTimeStatus, theme} = body as Partial<HubPreferences>

  if (codeEditorPath !== undefined) {
    setPreference('codeEditorProgram', codeEditorPath)
  }

  if (firstTimeStatus !== undefined) {
    setPreference('firstTimeStatus', firstTimeStatus)
  }

  if (theme !== undefined) {
    setPreference('theme', theme)
  }

  return makeJsonResponse({})
}

const handleChooseEditor: RequestHandler = async () => {
  const returnValue = await dialog.showOpenDialog({
    properties: ['openFile'],
  })
  if (returnValue.canceled || !returnValue.filePaths.length) {
    return makeJsonResponse({})
  }
  setPreference('codeEditorProgram', returnValue.filePaths[0])
  return makeJsonResponse({})
}

const handleGetInstalledPrograms: RequestHandler = async () => {
  const availableEditors = await getAvailableEditors()

  const state: InstalledPrograms = {
    availableEditors,
  }

  return makeJsonResponse(state)
}

const handlePreferencesRequest = withErrorHandlingResponse(branches({
  '/current': methods({
    GET: handleGetPreferences,
    PATCH: handlePatchPreferences,
  }),
  '/installed-programs': methods({GET: handleGetInstalledPrograms}),
  '/choose-editor': methods({POST: handleChooseEditor}),
}))

export {
  handlePreferencesRequest,
}

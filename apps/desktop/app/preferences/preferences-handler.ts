import type {
  HubPreferences, InstalledPrograms,
} from '@repo/reality/shared/desktop/preferences-types'
import {dialog} from 'electron'

import {makeJsonResponse} from '../../json-response'
import {getPreference, setPreference} from '../../local-preferences'
import {withErrorHandlingResponse} from '../../errors'
import {getAvailableEditors} from './code-editor'

const loadPreferences = (): HubPreferences => {
  const codeEditorPath = getPreference('codeEditorProgram') || ''
  return {
    codeEditorPath,
    firstTimeStatus: getPreference('firstTimeStatus') === 'complete' ? 'complete' : 'pending',
    theme: getPreference('theme') as 'light' | 'dark' | 'system' || 'system',
  }
}

const handleGetPreferences = withErrorHandlingResponse((): Response => (
  makeJsonResponse(loadPreferences())
))

const handlePatchPreferences = withErrorHandlingResponse(async (request) => {
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
})

const handleChooseEditor = withErrorHandlingResponse(async () => {
  const returnValue = await dialog.showOpenDialog({
    properties: ['openFile'],
  })
  if (returnValue.canceled || !returnValue.filePaths.length) {
    return makeJsonResponse({})
  }
  setPreference('codeEditorProgram', returnValue.filePaths[0])
  return makeJsonResponse({})
})

const handleChooseEditorRequest = (request: Request) => {
  switch (request.method) {
    case 'POST':
      return handleChooseEditor(request)
    default:
      return new Response('Not found', {status: 404})
  }
}

const handleBasePreferencesRequest = (request: Request) => {
  switch (request.method) {
    case 'GET':
      return handleGetPreferences(request)
    case 'PATCH':
      return handlePatchPreferences(request)
    default:
      return new Response('Not found', {status: 404})
  }
}

const handleGetInstalledPrograms = withErrorHandlingResponse(async (): Promise<Response> => {
  const availableEditors = await getAvailableEditors()

  const state: InstalledPrograms = {
    availableEditors,
  }

  return makeJsonResponse(state)
})

const handleInstalledProgramsRequest = (request: Request) => {
  switch (request.method) {
    case 'GET':
      return handleGetInstalledPrograms(request)
    default:
      return new Response('Not found', {status: 404})
  }
}

const handlePreferencesRequest = (request: Request) => {
  const requestUrl = new URL(request.url)
  const {pathname} = requestUrl

  switch (pathname) {
    case '/current':
      return handleBasePreferencesRequest(request)
    case '/installed-programs':
      return handleInstalledProgramsRequest(request)
    case '/choose-editor':
      return handleChooseEditorRequest(request)
    default: {
      // eslint-disable-next-line no-console
      console.error('Unknown preferences request:', pathname)
      return new Response('Not Found', {status: 404})
    }
  }
}

export {
  handlePreferencesRequest,
}

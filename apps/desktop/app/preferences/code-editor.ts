import fs from 'fs/promises'
import {app, shell} from 'electron'

import type {CodeEditor} from '@repo/reality/shared/desktop/preferences-types'

import {spawn} from 'child_process'

import {makeRunQueue} from '@repo/reality/shared/run-queue'

import {getPreference} from '../../local-preferences'

type PathsByArch = Partial<Record<typeof process.platform, string>>

const pathForArch = (paths: PathsByArch) => paths[process.platform] || ''

const HOME_DIR = app.getPath('home')

const VSCODE: CodeEditor = {
  identifier: 'vscode',
  name: 'Visual Studio Code',
  path: pathForArch({
    darwin: '/Applications/Visual Studio Code.app',
    win32: 'C:\\Program Files\\Microsoft VS Code\\Code.exe',
  }),
}

const POSSIBLE_EDITORS: CodeEditor[] = [
  VSCODE,
  {
    name: 'Cursor',
    path: pathForArch({
      darwin: '/Applications/Cursor.app',
      win32: `${HOME_DIR}\\AppData\\Local\\Programs\\cursor\\Cursor.exe`,
    }),
  },
  {
    name: 'Windsurf',
    path: pathForArch({
      darwin: '/Applications/Windsurf.app',
      win32: `${HOME_DIR}\\AppData\\Local\\Programs\\Windsurf\\Windsurf.exe`,
    }),
  },
]

const openQueue = makeRunQueue()

const editorSupported = async (editor: CodeEditor): Promise<boolean> => {
  try {
    if (!editor.path) {
      return false
    }
    await fs.access(editor.path)
    return true
  } catch (err: any) {
    if (err.code === 'ENOENT') {
      return false
    }
    throw err  // Re-throw unexpected errors
  }
}

const getAvailableEditors = async (): Promise<CodeEditor[]> => {
  const availableEditors: CodeEditor[] = []

  for (const editor of POSSIBLE_EDITORS) {
    // eslint-disable-next-line no-await-in-loop
    if (await editorSupported(editor)) {
      availableEditors.push(editor)
    }
  }

  return availableEditors
}

const getCodeEditor = async (): Promise<CodeEditor | undefined> => {
  const preference = getPreference('codeEditorProgram')
  if (!preference) {
    if (await editorSupported(VSCODE)) {
      return VSCODE
    }
    return undefined
  }

  const preferenceMatch = POSSIBLE_EDITORS.find(editor => editor.path === preference)
  if (preferenceMatch) {
    return preferenceMatch
  }

  return {path: preference, name: 'Custom'}
}

const openInCodeEditor = async (projectLocation: string, filePath?: string) => {
  const codeEditor = await getCodeEditor()
  if (!codeEditor) {
    shell.openExternal(new URL(filePath || projectLocation, 'file://').toString())
    return
  }

  const args: string[] = []

  if (process.platform === 'darwin') {
    args.push('open', '-a')
  }

  args.push(codeEditor.path)

  const supportsFolderPath = codeEditor.identifier === 'vscode'

  if (supportsFolderPath) {
    args.push(projectLocation)
    if (filePath) {
      args.push(filePath)
    }
  } else {
    args.push(filePath || projectLocation)
  }

  await openQueue.next(async () => {
    spawn(args[0], args.slice(1), {
      stdio: 'ignore',
      detached: true,
    })

    // NOTE(christoph): If you trigger vscode too quickly in succession, it opens the same workspace
    // multiple times.
    await new Promise(resolve => setTimeout(resolve, 500))
  })
}

export {
  getCodeEditor,
  getAvailableEditors,
  openInCodeEditor,
}

type HubPreferences = {
  codeEditorPath?: string
  firstTimeStatus?: 'pending' | 'complete'
  theme: 'light' | 'dark' | 'system'
}

type CodeEditor = {
  identifier?: 'vscode'
  name: string
  path: string
}

type InstalledPrograms = {
  availableEditors: Array<CodeEditor>
}

export type {
  HubPreferences,
  InstalledPrograms,
  CodeEditor,
}

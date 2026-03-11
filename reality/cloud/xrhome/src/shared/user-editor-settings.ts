interface UserEditorSettings {
  autoFormat: boolean
  minimap: boolean
  localLinkSharing: boolean
  keyboardHandler: 'vim' | 'emacs' | 'vscode' | null
  liveSync: boolean
  saveDebugEdits: boolean
  viewExpanseDiff: boolean
}

export {
  UserEditorSettings,
}

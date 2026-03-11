import type {UserEditorSettings} from './use-user-editor-settings'

const sanitizeKeyboardHandlerValue = value => (
  ['vim', 'emacs', 'vscode'].includes(value) ? value : null
)

const defaultEditorSettings: UserEditorSettings = {
  autoFormat: false,
  minimap: true,
  keyboardHandler: null,
  localLinkSharing: false,
  liveSync: false,
  saveDebugEdits: true,
  viewExpanseDiff: false,
}

// Takes an object and returns an object that conforms to the correct settings format with
// defaults filled in for missing fields
const extractUserEditorSettings = (settings) => {
  const result = {...defaultEditorSettings}

  if (settings.autoFormat === true) {
    result.autoFormat = true
  }

  if (settings.minimap === false) {
    result.minimap = false
  }

  if (settings.localLinkSharing === true) {
    result.localLinkSharing = true
  }

  if (settings.liveSync === true) {
    result.liveSync = true
  }

  if (settings.saveDebugEdits === false) {
    result.saveDebugEdits = false
  }

  if (settings.viewExpanseDiff === true) {
    result.viewExpanseDiff = true
  }

  result.keyboardHandler = sanitizeKeyboardHandlerValue(settings.keyboardHandler)

  return result
}

const parseUserEditorSettings = (settingsString) => {
  try {
    return extractUserEditorSettings(JSON.parse(settingsString))
  } catch (err) {
    return {...defaultEditorSettings}
  }
}

export {
  extractUserEditorSettings,
  parseUserEditorSettings,
}

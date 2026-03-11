import {parseUserEditorSettings} from './editor-settings'
import type {UserEditorSettings} from '../../shared/user-editor-settings'

// TODO(christoph): Make this dynamic
const SETTINGS = parseUserEditorSettings(null)

const useUserEditorSettings = (): UserEditorSettings => (
  SETTINGS
)

export {useUserEditorSettings}

export type {UserEditorSettings}

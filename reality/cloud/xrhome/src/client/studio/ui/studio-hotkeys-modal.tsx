import React from 'react'
import {useTranslation} from 'react-i18next'

import {FloatingTrayModal} from '../../ui/components/floating-tray-modal'
import {createThemedStyles} from '../../ui/theme'
import {LinkButton} from '../../ui/components/link-button'
import {useUserEditorSettings} from '../../user/use-user-editor-settings'
import {isMac} from '../../editor/device-models'
import {
  CLOUD_STUDIO_SHORTCUTS, MACOS_SHORTCUTS_INDEX, WINDOWS_SHORTCUTS_INDEX,
} from '../../apps/widgets/keyboard-shortcut-constants'

const useStyles = createThemedStyles(theme => ({
  buttonTray: {
    textAlign: 'center',
    padding: '1em 1em 0 1em',
  },
  modalContainer: {
    padding: '2em 0',
  },
  header: {
    textAlign: 'center',
    fontSize: '1.5em',
    fontWeight: 700,
    padding: '0 1em',
  },
  groupContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '1em',
    padding: '2em',
    minHeight: 0,
    maxHeight: '30em',
    overflowY: 'auto',
  },
  group: {
    border: theme.studioSectionBorder,
    borderRadius: '0.5em',
    padding: '0.5em',
    display: 'flex',
    flexDirection: 'column',
    gap: '0.75em',
  },
  groupHeading: {
    color: theme.fgMuted,
  },
  actionRow: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
    minWidth: '30em',
    gap: '0.25em',
  },
  actionLabel: {
    wordWrap: 'break-word',
    maxWidth: '25em',
  },
  hotkeyGroup: {
    display: 'flex',
    gap: '0.25em',
    alignItems: 'flex-end',
  },
  hotkeyRow: {
    display: 'flex',
    gap: '0.25em',
    justifyContent: 'flex-end',
  },
  hotkey: {
    padding: '0.25em',
    background: theme.stepperArrowBackground,
    borderRadius: '0.25em',
    minWidth: '2em',
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    whiteSpace: 'nowrap',
  },
  keyboardMode: {
    textAlign: 'center',
    padding: '1em',
  },
}))

interface IActionRow {
  label: string
  action: string
}

const ActionRow: React.FC<IActionRow> = ({label, action}) => {
  const classes = useStyles()
  const {t} = useTranslation(['app-pages'])
  const keyGroups = action.split(',')

  return (
    <div className={classes.actionRow}>
      <div className={classes.actionLabel}>{t(label)}</div>
      <div className={classes.hotkeyGroup}>
        {keyGroups.map((keyGroup, index) => (
          <React.Fragment key={keyGroup}>
            <div className={classes.hotkeyRow}>
              {keyGroup.split('-').map(key => (
                <div key={key} className={classes.hotkey}>{key}</div>
              ))}
            </div>
            {index < keyGroups.length - 1 && <div>,</div>}
          </React.Fragment>
        ))}
      </div>
    </div>

  )
}

const keyboardOptions = ['Ace', 'Vim', 'Emacs', 'VSCode'].map(name => ({
  value: name.toLowerCase(),
  content: name,
}))

interface IStudioHotkeysModal {
  trigger: React.ReactNode
}

const StudioHotkeysModal: React.FC<IStudioHotkeysModal> = ({
  trigger,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['app-pages', 'common'])
  const editorSettings = useUserEditorSettings()
  const keybinding = editorSettings.keyboardHandler || 'ace'
  const keybindingName = keyboardOptions.find(option => option.value === keybinding)?.content
  const categories = CLOUD_STUDIO_SHORTCUTS
  const isMacOs = isMac(navigator.userAgent, navigator.platform)
  const os = isMacOs ? MACOS_SHORTCUTS_INDEX : WINDOWS_SHORTCUTS_INDEX

  return (
    <FloatingTrayModal
      trigger={trigger}
    >
      {collapse => (
        <div className={classes.modalContainer}>
          <div className={classes.header}>
            {t('project_settings_page.dev_preference_settings_view.button.keyboard_shortcuts')}
          </div>
          <div className={classes.keyboardMode}>
            {keybindingName}
            {' - '}
            {/* eslint-disable-next-line local-rules/hardcoded-copy */}
            {isMacOs ? 'macOS/iPadOS' : 'Windows/Linux'}
          </div>
          <div className={classes.groupContainer}>
            {Object.entries(categories).map(([categoryName, category]) => (
              <div className={classes.group} key={categoryName}>
                <div className={classes.groupHeading}>{t(categoryName)}</div>
                {Object.entries(category).map(
                  ([action, shortcut]) => (
                    shortcut[os]
                      ? <ActionRow key={action} label={action} action={shortcut[os]} />
                      : null
                  )
                )}
              </div>
            ))}
          </div>
          <div className={classes.buttonTray}>
            <LinkButton onClick={collapse}>
              {t('button.close', {ns: 'common'})}
            </LinkButton>
          </div>
        </div>
      )}
    </FloatingTrayModal>
  )
}

export {
  StudioHotkeysModal,
}

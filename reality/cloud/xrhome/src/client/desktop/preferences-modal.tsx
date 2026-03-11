import React from 'react'
import {useSuspenseQuery} from '@tanstack/react-query'
import {createUseStyles} from 'react-jss'

import {useTranslation, Trans} from 'react-i18next'

import {StandardModal} from '../ui/components/standard-modal'
import {Loader} from '../ui/components/loader'
import AutoHeadingScope from '../widgets/auto-heading-scope'
import AutoHeading from '../widgets/auto-heading'
import {SpaceBetween} from '../ui/layout/space-between'
import {IconButton} from '../ui/components/icon-button'
import {StandardLink} from '../ui/components/standard-link'
import {Icon} from '../ui/components/icon'
import type {HubPreferences, InstalledPrograms} from '../../shared/desktop/preferences-types'
import {StandardDropdownField} from '../ui/components/standard-dropdown-field'
import {useLocaleChange} from '../user/use-locale'
import {getSupportedLocale8wOptions} from '../../shared/i18n/i18n-locales'

interface IPreferencesModal {
  onClose: () => void
}

const useStyles = createUseStyles({
  preferencesEditView: {
    position: 'relative',
    maxWidth: '80vw',
    width: '600px',
    padding: '1rem',
  },
  topRightActions: {
    position: 'absolute',
    top: '1rem',
    right: '1rem',
  },
  modalTitle: {
    fontFamily: 'Geist Mono, monospace !important',
    fontSize: '1.125rem',
    fontWeight: 700,
    userSelect: 'none',
  },
})

const usePreferences = () => useSuspenseQuery({
  queryKey: ['preferences'],
  queryFn: async (): Promise<HubPreferences> => {
    const res = await fetch('preferences:///current')
    if (!res.ok) {
      throw new Error('Failed to fetch preferences')
    }
    return res.json()
  },
})

const useInstalledPrograms = () => useSuspenseQuery({
  queryKey: ['preferences-installed-programs'],
  staleTime: 0,
  refetchOnWindowFocus: true,  // We want to update if the user installs vscode while open
  queryFn: async (): Promise<InstalledPrograms> => {
    const res = await fetch('preferences:///installed-programs')
    if (!res.ok) {
      throw new Error('Failed to fetch available editors')
    }
    return res.json()
  },
})

const useCodeEditorSelection = () => {
  const prefs = usePreferences()
  const installedPrograms = useInstalledPrograms()
  const {t} = useTranslation(['studio-desktop-pages'])

  const vsCodeOption = installedPrograms.data.availableEditors.find(editor => (
    editor.identifier === 'vscode'
  ))

  const currentValue = prefs.data.codeEditorPath || vsCodeOption?.path || ''

  const handleEditorChange = async (editorPath: string) => {
    const res = await fetch('preferences:///current', {
      method: 'PATCH',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({codeEditorPath: editorPath}),
    })

    if (!res.ok) {
      throw new Error('Failed to update preferences')
    }
    prefs.refetch()
  }

  const handleSelectionChange = async (selectedPath: string) => {
    if (selectedPath === 'browse') {
      const res = await fetch('preferences:///choose-editor', {
        method: 'POST',
      })
      if (!res.ok) {
        throw new Error('Failed to choose editor')
      }
      prefs.refetch()
    } else {
      handleEditorChange(selectedPath)
    }
  }

  const visibleOptions = installedPrograms.data.availableEditors.map(editor => ({
    content: editor.name,
    value: editor.path,
  }))

  if (currentValue && !visibleOptions.some(option => option.value === currentValue)) {
    visibleOptions.push({
      content: `${t('preferences_modal.option.custom')} (${currentValue})`,
      value: currentValue,
    })
  }

  visibleOptions.push({
    content: t('preferences_modal.button.browse'),
    value: 'browse',
  })

  return [currentValue, visibleOptions, handleSelectionChange] as const
}

const useThemeSelection = () => {
  const prefs = usePreferences()
  const currentValue = prefs.data.theme || 'system'
  const {t} = useTranslation(['studio-desktop-pages'])

  const visibleOptions = [
    {content: t('preferences_modal.option.theme_light'), value: 'light'},
    {content: t('preferences_modal.option.theme_dark'), value: 'dark'},
    {content: t('preferences_modal.option.theme_system'), value: 'system'},
  ]

  const handleThemeChange = async (theme: 'light' | 'dark' | 'system') => {
    const res = await fetch('preferences:///current', {
      method: 'PATCH',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({theme}),
    })

    if (!res.ok) {
      throw new Error('Failed to update theme preference')
    }
    prefs.refetch()
  }

  return [currentValue, visibleOptions, handleThemeChange] as const
}

interface IPreferencesEditView {
  onClose: () => void
}

const PreferencesEditView: React.FC<IPreferencesEditView> = ({onClose}) => {
  const installedPrograms = useInstalledPrograms()
  const classes = useStyles()
  const {t} = useTranslation(['studio-desktop-pages', 'common'])

  const vsCodeOption = installedPrograms.data?.availableEditors.find(editor => (
    editor.identifier === 'vscode'
  ))
  const hasVscode = Boolean(vsCodeOption)

  const [currentValue, visibleOptions, handleSelectionChange] = useCodeEditorSelection()
  const [currentLocale, setLocale] = useLocaleChange()
  const [currentTheme, visibleThemeOptions, handleThemeChange] = useThemeSelection()

  return (
    <div className={classes.preferencesEditView}>
      <AutoHeadingScope>
        <SpaceBetween direction='vertical'>
          <AutoHeading className={classes.modalTitle}>
            {t('preferences_modal.title.main')}
          </AutoHeading>
          <div className={classes.topRightActions}>
            <IconButton
              onClick={onClose}
              stroke='cancel'
              text={t('button.close', {ns: 'common'})}
            />
          </div>
          <SpaceBetween direction='vertical'>

            <div>
              <p>{t('preferences_modal.editor.recommended')}</p>
              {!hasVscode &&
                <Trans
                  ns='studio-desktop-pages'
                  i18nKey='preferences_modal.link.download_vscode_with_icon'
                  components={{
                    1: <StandardLink newTab href='https://code.visualstudio.com/download' />,
                    2: <Icon stroke='external' inline />,
                  }}
                />
              }
            </div>

            <StandardDropdownField
              label={t('preferences_modal.label.select_editor')}
              value={currentValue}
              options={visibleOptions}
              onChange={handleSelectionChange}
            />

            <StandardDropdownField
              label={t('preferences_modal.label.select_language')}
              value={currentLocale}
              options={getSupportedLocale8wOptions()}
              onChange={setLocale}
            />

            <StandardDropdownField
              label={t('preferences_modal.label.theme')}
              value={currentTheme}
              options={visibleThemeOptions}
              onChange={handleThemeChange}
            />
          </SpaceBetween>
        </SpaceBetween>
      </AutoHeadingScope>
    </div>
  )
}

const PreferencesModal: React.FC<IPreferencesModal> = ({onClose}) => (
  <StandardModal
    onOpenChange={onClose}
    trigger='render'
  >
    {() => (
      <React.Suspense fallback={<Loader />}>
        <PreferencesEditView onClose={onClose} />
      </React.Suspense>
    )}
  </StandardModal>
)

export {
  PreferencesModal,
  usePreferences,
  useInstalledPrograms,
  useCodeEditorSelection,
}

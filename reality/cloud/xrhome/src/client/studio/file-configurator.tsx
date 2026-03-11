import React from 'react'
import {useTranslation} from 'react-i18next'

import {EditorFileLocation, extractFilePath, extractRepoId} from '../editor/editor-file-location'
import {openFile} from './local-sync-api'
import {useEnclosedApp} from '../apps/enclosed-app-context'
import CodeHighlight, {filenameToLanguage} from '../browse/code-highlight'
import {useScopedGitFile} from '../git/hooks/use-current-git'
import {useCurrentRepoId} from '../git/repo-id-context'
import {createThemedStyles} from '../ui/theme'
import {FloatingPanelButton} from '../ui/components/floating-panel-button'
import {useTheme} from '../user/use-theme'
import {FileNameConfigurator} from './configuration/file-name-configurator'

const useStyles = createThemedStyles(theme => ({
  fileConfigurator: {
    flex: '1 0 0',
    display: 'flex',
    flexDirection: 'column',
  },
  actions: {
    padding: '1rem',
  },
  contentPreview: {
    flex: '1 0 0',
    minHeight: 0,
    border: `1px solid ${theme.fgMuted}`,
    borderRadius: '4px',
    padding: '0.5rem',
    overflowY: 'auto',
    display: 'block',
    margin: '0 1rem 1rem 1rem',
  },
}))

interface IFileConfigurator {
  location: EditorFileLocation
}

const FileConfigurator: React.FC<IFileConfigurator> = ({
  location,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const app = useEnclosedApp()
  const rawPath = extractFilePath(location)

  const rootRepoId = useCurrentRepoId()
  const repoId = extractRepoId(location) || rootRepoId
  const content = useScopedGitFile(repoId, rawPath)?.content
  const language = filenameToLanguage(rawPath)

  const classes = useStyles()
  const themeName = useTheme()

  return (
    <div className={classes.fileConfigurator}>
      <FileNameConfigurator location={location} />
      {Build8.PLATFORM_TARGET === 'desktop' && app && repoId === rootRepoId &&
        <div className={classes.actions}>
          <FloatingPanelButton
            spacing='full'
            onClick={() => openFile(app.appKey, rawPath)}
          >
            {t('file_configurator.button.open_externally')}
          </FloatingPanelButton>
        </div>
      }
      <pre className={classes.contentPreview}>
        <CodeHighlight
          content={content}
          language={language}
          themeMode={themeName}
          simpleMode
        />
      </pre>
    </div>
  )
}

export {
  FileConfigurator,
}

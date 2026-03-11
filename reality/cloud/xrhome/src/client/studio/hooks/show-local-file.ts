import {useTranslation} from 'react-i18next'

import {useEnclosedApp} from '../../apps/enclosed-app-context'
import {EditorFileLocation, extractRepoId, extractFilePath} from '../../editor/editor-file-location'
import {showFile} from '../local-sync-api'

const useShowLocalFile = Build8.PLATFORM_TARGET === 'desktop'
  ? (
    fileLocation: EditorFileLocation
  ) => {
    const appKey = useEnclosedApp()?.appKey
    const {t} = useTranslation(['cloud-editor-pages'])
    if (appKey && !extractRepoId(fileLocation)) {
      return {
        content: window.electron.os === 'mac'
          ? t('file_list.option.show_in_finder')
          : t('file_list.option.show_in_file_explorer'),
        onClick: () => showFile(appKey, extractFilePath(fileLocation)),
      }
    }

    return null
  }
  : () => null

export {useShowLocalFile}

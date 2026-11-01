import React from 'react'
import {useTranslation} from 'react-i18next'

import {useMaybeLocalSyncContext} from '../../studio/local-sync-context'
import {ConsoleMenuButton} from './console-menu'

interface ISystemLogsMenu {

}

const SystemLogsMenu: React.FC<ISystemLogsMenu> = () => {
  const {t} = useTranslation('cloud-editor-pages')
  const localSync = useMaybeLocalSyncContext()
  if (!localSync) {
    return null
  }
  return (
    <ConsoleMenuButton onClick={() => localSync.restartServer()}>
      {t('system_logs_menu.button.restart_server')}
    </ConsoleMenuButton>
  )
}

export {
  SystemLogsMenu,
}

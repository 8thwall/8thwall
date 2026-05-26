import React from 'react'
import {useTranslation} from 'react-i18next'
import {Menu} from 'semantic-ui-react'

import {useMaybeLocalSyncContext} from '../../studio/local-sync-context'

interface ISystemLogsMenu {

}

const SystemLogsMenu: React.FC<ISystemLogsMenu> = () => {
  const {t} = useTranslation('cloud-editor-pages')
  const localSync = useMaybeLocalSyncContext()
  if (!localSync) {
    return null
  }
  return (
    <Menu.Item onClick={() => localSync.restartServer()}>
      {t('system_logs_menu.button.restart_server')}
    </Menu.Item>
  )
}

export {
  SystemLogsMenu,
}

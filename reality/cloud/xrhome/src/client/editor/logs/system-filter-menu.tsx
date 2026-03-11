import * as React from 'react'
import {Menu} from 'semantic-ui-react'
import {useTranslation} from 'react-i18next'

interface ISystemFilterMenu {
  filterBuild: boolean
  filterRepo: boolean
  onToggleBuild: () => void
  onToggleRepo: () => void
}

const SystemFilterMenu: React.FunctionComponent<ISystemFilterMenu> = (props) => {
  const {t} = useTranslation(['cloud-editor-pages'])

  return (
    <Menu className='sub-filter-menu'>
      <Menu.Item
        active={props.filterBuild}
        onClick={props.onToggleBuild}
      >{t('editor_page.log_container.button.build')}
      </Menu.Item>
      <Menu.Item
        active={props.filterRepo}
        onClick={props.onToggleRepo}
      >{t('editor_page.log_container.button.repo')}
      </Menu.Item>
    </Menu>

  )
}

export default SystemFilterMenu

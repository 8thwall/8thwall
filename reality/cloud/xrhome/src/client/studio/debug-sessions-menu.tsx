import React from 'react'
import {useTranslation} from 'react-i18next'

import {Menu} from 'semantic-ui-react'

import {Icon} from '../ui/components/icon'
import useCurrentApp from '../common/use-current-app'
import {DevQRCodePopup} from '../editor/token/dev-qr-code-popup'
import {createThemedStyles} from '../ui/theme'
import {combine} from '../common/styles'

const useStyles = createThemedStyles({
  button: {
    cursor: 'pointer',
  },
})

const DebugSessionsMenu: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()

  const app = useCurrentApp()

  return (
    <Menu.Item>
      <DevQRCodePopup
        app={app}
        placement='top'
        shrink
        trigger={() => (
          <button
            className={combine('style-reset', classes.button)}
            type='button'
            a8='click;studio;project-connect-button'
          >
            <Icon size={0.75} inline stroke='plus' />
            {t('debug_sessions_menu.button.connect_device')}
          </button>
        )}
      />
    </Menu.Item>
  )
}

export {DebugSessionsMenu}

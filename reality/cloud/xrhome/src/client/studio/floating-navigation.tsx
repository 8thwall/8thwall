import React from 'react'
import {useHistory} from 'react-router-dom'
import {useTranslation} from 'react-i18next'

import {FloatingTray} from '../ui/components/floating-tray'
import {FloatingIconButton} from '../ui/components/floating-icon-button'
import {useSceneContext} from './scene-context'
import {HOME_PATH} from '../desktop/desktop-paths'

const FloatingNavigation: React.FC = () => {
  const {t} = useTranslation(['common'])
  const ctx = useSceneContext()
  const history = useHistory()
  return (
    <FloatingTray nonInteractive={ctx.isDraggingGizmo}>
      <FloatingIconButton
        a8='click;studio;navigation-menu-button'
        text={t('button.home', {ns: 'common'})}
        stroke='home'
        onClick={() => history.push(HOME_PATH)}
      />
    </FloatingTray>
  )
}

export {
  FloatingNavigation,
}

import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import {FloatingTray} from '../ui/components/floating-tray'
import {FloatingTrayButton} from '../ui/components/floating-tray-button'
import {PublishButton} from '../editor/publish-button'
import {useSceneContext} from './scene-context'

const useStyles = createUseStyles({
  trayContainer: {
    flex: '1 0 auto',
  },
})

interface IBuildControlTray {
}

const BuildControlTray: React.FC<IBuildControlTray> = () => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useStyles()
  const ctx = useSceneContext()

  return (
    <div className={classes.trayContainer}>
      <FloatingTray fillContainer nonInteractive={ctx.isDraggingGizmo}>
        <PublishButton
          renderButton={({disabled}) => (
            <FloatingTrayButton
              a8='click;studio;project-publish-button'
              isDisabled={disabled}
              color='purple'
              grow
            >
              {t('editor_page.button.publish')}
            </FloatingTrayButton>
          )}
        />
      </FloatingTray>
    </div>
  )
}

export {
  BuildControlTray,
}

import React from 'react'
import {useTranslation} from 'react-i18next'

import {IconButton} from '../../ui/components/icon-button'
import {useUiConfiguratorStyles} from './ui/ui-configurator-styles'

const DeleteGroupButton: React.FC<{
  onClick: React.MouseEventHandler<HTMLButtonElement>
}> = ({onClick}) => {
  const {t} = useTranslation(['common'])
  const classes = useUiConfiguratorStyles()

  return (
    <div className={classes.deleteButton}>
      <IconButton
        stroke='minus'
        text={t('button.delete')}
        onClick={onClick}
      />
    </div>
  )
}

export {DeleteGroupButton}

import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import {SelectMenu} from '../../studio/ui/select-menu'
import {MenuOptions} from '../../studio/ui/option-menu'
import {useStudioMenuStyles} from '../../studio/ui/studio-menu-styles'
import {setModeClearState, useAssetLabStateContext} from '../asset-lab-context'
import {Icon, IconStroke} from '../../ui/components/icon'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'

const useStyles = createUseStyles({
  optionWithIcon: {
    '& > :first-child': {
      marginRight: '0.5rem',
      marginLeft: 0,
    },
  },
  fullWidth: {
    width: '100%',
  },
})

const AssetLabGenerateButton = () => {
  const {t} = useTranslation(['asset-lab'])
  const menuStyles = useStudioMenuStyles()
  const assetLabCtx = useAssetLabStateContext()
  const classes = useStyles()

  const makeOptionContent = (icon: IconStroke, label: string) => (
    <span className={classes.optionWithIcon}><Icon stroke={icon} inline />{t(label)}...</span>
  )

  const options = [
    {
      content: makeOptionContent('flatTarget', 'asset_lab.option.generate_image'),
      onClick: () => {
        setModeClearState(assetLabCtx, 'image', {open: true, workflow: 'gen-image'})
      },
      a8: 'click;asset-lab;generate-image-button',
    },
    {
      content: makeOptionContent('meshCube', 'asset_lab.option.generate_model'),
      onClick: () => {
        setModeClearState(assetLabCtx, 'image', {open: true, workflow: 'gen-3d-model'})
      },
      a8: 'click;asset-lab;generate-model-button',
    },
    {
      content: makeOptionContent('guyRunningRight', 'asset_lab.option.generate_animation'),
      onClick: () => {
        setModeClearState(assetLabCtx, 'image', {open: true, workflow: 'gen-animated-char'})
      },
      a8: 'click;asset-lab;generate-animation-button',
    },
  ]

  return (
    <div className={classes.fullWidth}>
      <SelectMenu
        id='asset-lab-generate-dropdown'
        trigger={(
          <FloatingPanelButton
            spacing='full'
            height='small'
          >
            <Icon stroke='beaker' inline />
            {t('asset_lab.browser.generate')}
          </FloatingPanelButton>
        )}
        menuWrapperClassName={menuStyles.studioMenu}
        placement='right-start'
        margin={16}
        minTriggerWidth
      >
        {collapse => (
          <MenuOptions
            collapse={collapse}
            options={options}
          />
        )}
      </SelectMenu>
    </div>
  )
}

export {
  AssetLabGenerateButton,
}

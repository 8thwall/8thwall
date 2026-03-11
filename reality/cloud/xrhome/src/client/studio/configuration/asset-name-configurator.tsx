import React from 'react'

import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../../ui/theme'
import {SrOnly} from '../../ui/components/sr-only'
import {StandardTextField} from '../../ui/components/standard-text-field'
import {Keys} from '../common/keys'

const useStyles = createThemedStyles(theme => ({
  assetNameConfiguratorContainer: {
    fontSize: '12px',
    padding: '1rem 1rem',
    borderBottom: theme.studioSectionBorder,
    display: 'grid',
    gridTemplateColumns: 'auto 1fr',
    gridGap: '0.5em',
  },
  iconField: {
    'display': 'flex',
    'alignItems': 'center',
  },
}))

interface IAssetNameConfigurator {
  assetEditName: string
  setAssetEditName: (value: string) => void
  clearAssetEditName: () => void
  submitAssetEditName: () => void
  icon: React.ReactNode
}

const AssetNameConfigurator = React.forwardRef<HTMLInputElement, IAssetNameConfigurator>(({
  assetEditName,
  setAssetEditName,
  clearAssetEditName,
  submitAssetEditName,
  icon,
}, ref) => {
  const classes = useStyles()
  const renaming = React.useRef(false)

  const {t} = useTranslation('cloud-studio-pages')

  const handleNameChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    renaming.current = true
    setAssetEditName(e.target.value)
  }

  const handleBlur = () => {
    if (renaming.current) {
      submitAssetEditName()
      renaming.current = false
    }
    clearAssetEditName()
  }

  const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === Keys.ENTER) {
      e.currentTarget.blur()
    } else if (e.key === Keys.ESCAPE) {
      renaming.current = false
      e.currentTarget.blur()
    }
  }

  return (
    <div className={classes.assetNameConfiguratorContainer}>
      <div className={classes.iconField}>
        {icon}
      </div>
      <StandardTextField
        label={<SrOnly>{t('asset_configurator.asset_name_configurator.rename_label')}</SrOnly>}
        id='object-name'
        value={assetEditName}
        height='small'
        onChange={handleNameChange}
        onFocus={e => e.target.select()}
        ref={ref}
        onBlur={handleBlur}
        onKeyDown={handleKeyDown}
      />
    </div>
  )
})

export {
  AssetNameConfigurator,
}

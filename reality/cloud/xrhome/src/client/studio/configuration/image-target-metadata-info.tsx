import React from 'react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import type {IImageTarget} from '../../common/types/models'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {useStyles as useRowFieldStyles} from './row-fields'
import {combine} from '../../common/styles'
import {Icon} from '../../ui/components/icon'
import {typeToLabel, typeToIcon} from '../image-target-list-item'

const useStyles = createUseStyles({
  value: {
    justifyContent: 'end',
    textAlign: 'end',
    gap: '0.5em',
    marginBottom: 0,
  },
})

interface IImageTargetMetadataInfo {
  imageTarget: IImageTarget
}

const ImageTargetMetadataInfo: React.FC<IImageTargetMetadataInfo> = ({
  imageTarget,
}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const classes = useStyles()
  const rowFieldClasses = useRowFieldStyles()
  const metadata = JSON.parse(imageTarget.metadata)

  return (
    <>
      <div className={rowFieldClasses.row}>
        <div className={rowFieldClasses.flexItem}>
          <StandardFieldLabel
            label={t('asset_configurator.image_target_configurator.type.label')}
            mutedColor
          />
        </div>
        <div className={combine(rowFieldClasses.flexItem, rowFieldClasses.row, classes.value)}>
          <StandardFieldLabel
            label={t(typeToLabel(imageTarget.type))}
            mutedColor
          />
          <Icon stroke={typeToIcon(imageTarget.type)} color='muted' />
        </div>
      </div>
      <div className={rowFieldClasses.row}>
        <div className={rowFieldClasses.flexItem}>
          <StandardFieldLabel
            label={t('asset_configurator.image_target_configurator.size.label')}
            mutedColor
          />
        </div>
        <div className={combine(rowFieldClasses.flexItem, classes.value)}>
          <StandardFieldLabel
            label={`${metadata.width} x ${metadata.height}`}
            mutedColor
          />
        </div>
      </div>
    </>
  )
}

export {
  ImageTargetMetadataInfo,
  typeToIcon,
  typeToLabel,
}

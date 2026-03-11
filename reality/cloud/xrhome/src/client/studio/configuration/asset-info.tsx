import React from 'react'

import {createUseStyles} from 'react-jss'

import {StandardFieldLabel} from '../../ui/components/standard-field-label'

import {useStyles as useRowFieldStyles} from './row-fields'

import {combine} from '../../common/styles'
import {Loader} from '../../ui/components/loader'

const useStyles = createUseStyles({
  rightAlignText: {
    textAlign: 'right',
  },
})

type MetadataKey = 'size' | 'dimensions' | 'duration' | 'tris' | 'compression'

type MetadataValue = {
  label: string
  value: string
  loading?: boolean
}

type Metadata = {
  [key in MetadataKey]?: MetadataValue
}

interface IAssetInfo {
  metadata: Metadata
}

const AssetInfo: React.FunctionComponent<IAssetInfo> = ({metadata}) => {
  const rowFieldClasses = useRowFieldStyles()
  const classes = useStyles()
  const entries = Object.entries(metadata).filter(([, v]) => v.value !== null)

  return (
    <>
      {entries.map(([, metadataValue]) => (
        <div className={rowFieldClasses.row} key={metadataValue.label}>
          <div className={rowFieldClasses.flexItem}>
            <StandardFieldLabel
              label={metadataValue.label}
              mutedColor
            />
          </div>
          <div className={combine(rowFieldClasses.flexItem, classes.rightAlignText)}>
            {metadataValue.loading
              ? (
                <Loader inline size='tiny' />
              )
              : (
                <StandardFieldLabel
                  label={metadataValue.value}
                  mutedColor
                />
              )}
          </div>
        </div>
      ))}
    </>
  )
}

export {
  AssetInfo,
}

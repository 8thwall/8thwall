import React from 'react'
import {TFunction, useTranslation} from 'react-i18next'

import {RowBooleanField, RowSelectField} from './row-fields'
import {StandardTextAreaField} from '../../ui/components/standard-text-area-field'
import {USER_METADATA_LIMIT} from '../../../shared/xrengine-config'
import {StaticBanner} from '../../ui/components/banner'

const validateMetadata = (input: string, isJson: boolean, t: TFunction) => {
  if (!input) {
    return null
  } else if (input.length > USER_METADATA_LIMIT) {
    return t('image_target_page.edit_image_target.metadata_error', {
      metadataLimit: USER_METADATA_LIMIT, ns: 'app-pages',
    })
  } else if (!isJson) {
    return null
  }

  try {
    JSON.parse(input)
    return null
  } catch (err) {
    return t('asset_configurator.image_target_configurator.error.invalid_json')
  }
}

interface IImageTargetUserMetadata {
  hasUserMetadata: boolean
  setHasUserMetadata(hasUserMetadata: boolean): void
  userMetadata: string
  setUserMetadata(userMetadata: string | null): void
  userMetadataIsJson: boolean
  setUserMetadataIsJson(userMetadataIsJson: boolean): void
  userMetadataError?: string | null
}

const ImageTargetUserMetadata: React.FC<IImageTargetUserMetadata> = ({
  hasUserMetadata, setHasUserMetadata, userMetadata, setUserMetadata, userMetadataIsJson,
  setUserMetadataIsJson, userMetadataError,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'app-pages'])

  return (
    <>
      <RowBooleanField
        id='has-user-metadata'
        label={t('asset_configurator.image_target_configurator.user_metadata.add')}
        checked={hasUserMetadata}
        onChange={(event) => {
          const {checked} = event.target
          setHasUserMetadata(checked)
        }}
      />
      {hasUserMetadata &&
        <>
          <RowSelectField
            id='user-metadata-format'
            label={t('asset_configurator.image_target_configurator.user_metadata.format')}
            value={userMetadataIsJson ? 'json' : 'text'}
            options={[
              {
                value: 'json',
                content: t('asset_configurator.image_target_configurator.user_metadata.json'),
              },
              {
                value: 'text',
                content: t('asset_configurator.image_target_configurator.user_metadata.text'),
              },
            ]}
            onChange={(format) => {
              const isJson = format === 'json'
              setUserMetadataIsJson(isJson)
            }}
          />
          <StandardTextAreaField
            id='user-metadata'
            label=''
            value={userMetadata ?? ''}
            onChange={(e) => {
              const {value} = e.target
              setUserMetadata(value || null)
            }}
          />
          {userMetadataError && <StaticBanner type='warning' message={userMetadataError} />}
        </>
      }
    </>
  )
}

export {
  ImageTargetUserMetadata,
  validateMetadata,
}

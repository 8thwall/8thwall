import React from 'react'
import {useTranslation} from 'react-i18next'
import type {UiGraphSettings} from '@ecs/shared/scene-graph'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'

import {RowGroupFields, RowNumberOrPercentField} from '../row-fields'
import {SrOnly} from '../../../ui/components/sr-only'
import {InputNumberTextboxDropdown} from '../../../ui/components/input-number-textbox-dropdown'
import {FloatingMenuButton} from '../../../ui/components/floating-menu-button'
import {useUiConfiguratorStyles} from './ui-configurator-styles'
import type {UiPropertyName} from '../ui-configurator-types'
import {StandardFieldContainer} from '../../../ui/components/standard-field-container'
import {IconButton} from '../../../ui/components/icon-button'

const getNumericValue = (value: string | number | undefined): number => {
  if (value === undefined || value === '') return undefined
  if (typeof value === 'number') return value
  return Number(value.replace(/%$/, ''))
}

const isPercentage = (value: string | number | undefined): value is string => {
  if (value && typeof value === 'string') {
    return value.endsWith('%')
  }
  return false
}

const parseAspectRatio = (width: number, height: number): number => (height ? width / height : 0)

interface ISizeConfigurator {
  uiObjectId: string
  settings: UiGraphSettings
  isRoot: boolean
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  onDelete: (key: UiPropertyName) => void
}

const SizeConfigurator: React.FC<ISizeConfigurator> = ({
  settings, isRoot, uiObjectId, onChange, onDelete,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useUiConfiguratorStyles()

  const [aspectRatioLockedId, setAspectRatioLockedId] = React.useState<string | null>(null)
  const aspectRatioLocked = (aspectRatioLockedId === uiObjectId) &&
    (isPercentage(settings.width ?? UI_DEFAULTS.width) ===
    isPercentage(settings.height ?? UI_DEFAULTS.height))

  const percentageSizeSupported = !isRoot || settings.type !== '3d'

  return (
    <>
      <RowGroupFields
        label={<SrOnly>{t('ui_configurator.ui_size_label.label')}</SrOnly>}
        formatLabel={false}
      >
        <InputNumberTextboxDropdown
          ariaLabel={t('ui_configurator.ui_width.label')}
          value={percentageSizeSupported
            ? settings.width ?? UI_DEFAULTS.width
            : getNumericValue(settings.width ?? UI_DEFAULTS.width)}
          onChange={(width) => {
            if (!aspectRatioLocked) {
              onChange(current => ({...current, width}))
            } else {
              if (isPercentage(width) !== isPercentage(settings.height ?? UI_DEFAULTS.height)) {
                // break out of their aspect ratio lock since units are different
                setAspectRatioLockedId(null)
                onChange(current => ({...current, width}))
                return
              }

              const originalWidthNum = getNumericValue(settings.width ?? UI_DEFAULTS.width)
              const originalHeightNum = getNumericValue(settings.height ?? UI_DEFAULTS.height)

              const aspectRatio = parseAspectRatio(originalWidthNum, originalHeightNum)
              const newHeight = aspectRatio > 0 ? (getNumericValue(width) / aspectRatio) : 0
              onChange(current => ({
                ...current,
                width,
                height:
                  isPercentage(settings.height ?? UI_DEFAULTS.height) ? `${newHeight}%` : newHeight,
              }))
            }
          }}
          step={1}
          min={0}
          icon='W'
          menuMatchContainerWidth
          supportPercentage={percentageSizeSupported}
          defaultValue={UI_DEFAULTS.width}
          menuContent={collapse => (
            <FloatingMenuButton
              isDisabled={settings.maxWidth !== undefined}
              onClick={() => {
                onChange(current => ({...current, maxWidth: UI_DEFAULTS.maxWidth}))
                collapse()
              }}
            >
              {t('ui_configurator.ui_max_width.add_label')}
            </FloatingMenuButton>
          )}
        />
        <InputNumberTextboxDropdown
          ariaLabel={t('ui_configurator.ui_height.label')}
          value={percentageSizeSupported
            ? settings.height ?? UI_DEFAULTS.height
            : getNumericValue(settings.height ?? UI_DEFAULTS.height)}
          onChange={(height) => {
            if (!aspectRatioLocked) {
              onChange(current => ({...current, height}))
            } else {
              if (isPercentage(height) !== isPercentage(settings.width ?? UI_DEFAULTS.width)) {
                // break out of their aspect ratio lock since units are different
                setAspectRatioLockedId(null)
                onChange(current => ({...current, height}))
                return
              }

              const originalWidthNum = getNumericValue(settings.width ?? UI_DEFAULTS.width)
              const originalHeightNum = getNumericValue(settings.height ?? UI_DEFAULTS.height)

              const aspectRatio = parseAspectRatio(originalWidthNum, originalHeightNum)
              const newWidth = getNumericValue(height) * aspectRatio
              onChange(current => ({
                ...current,
                width:
                  isPercentage(settings.width ?? UI_DEFAULTS.width) ? `${newWidth}%` : newWidth,
                height,
              }))
            }
          }}
          step={1}
          min={0}
          icon='H'
          menuMatchContainerWidth
          supportPercentage={percentageSizeSupported}
          defaultValue={UI_DEFAULTS.height}
          menuContent={collapse => (
            <FloatingMenuButton
              isDisabled={settings.maxHeight !== undefined}
              onClick={() => {
                onChange(current => ({...current, maxHeight: UI_DEFAULTS.maxHeight}))
                collapse()
              }}
            >
              {t('ui_configurator.ui_max_height.add_label')}
            </FloatingMenuButton>
          )}
        />
        <StandardFieldContainer>
          <div className={classes.iconButton}>
            <IconButton
              stroke={aspectRatioLocked ? 'chain' : 'chainBroken'}
              text={t('ui_configurator.ui_padding.lock')}
              onClick={() => {
                if (aspectRatioLocked) {
                  // unlock the aspect ratio
                  setAspectRatioLockedId(null)
                  return
                }

                const width = settings.width ?? UI_DEFAULTS.width
                const height = settings.height ?? UI_DEFAULTS.height
                if (isPercentage(width) !== isPercentage(height)) {
                  // we'll copy over the fixed dimension, so we can match units before locking
                  if (isPercentage(width)) {
                    onChange(current => ({...current, width: height}))
                  } else {
                    onChange(current => ({...current, height: width}))
                  }
                }

                setAspectRatioLockedId(uiObjectId)
              }}
            />
          </div>
        </StandardFieldContainer>
      </RowGroupFields>
      {settings.maxWidth !== undefined && (
        <RowNumberOrPercentField
          id='ui-max-width'
          label={t('ui_configurator.ui_max_width.label')}
          value={settings.maxWidth ?? UI_DEFAULTS.maxWidth}
          onChange={value => onChange(current => ({...current, maxWidth: value}))}
          onDelete={() => onDelete('maxWidth')}
        />
      )}
      {settings.maxHeight !== undefined && (
        <RowNumberOrPercentField
          id='ui-max-height'
          label={t('ui_configurator.ui_max_height.label')}
          value={settings.maxHeight ?? UI_DEFAULTS.maxHeight}
          onChange={value => onChange(current => ({...current, maxHeight: value}))}
          onDelete={() => onDelete('maxHeight')}
        />
      )}
    </>
  )
}

export {
  SizeConfigurator,
  getNumericValue,
}

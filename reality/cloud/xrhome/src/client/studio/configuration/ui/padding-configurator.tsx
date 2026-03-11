import React from 'react'
import {useTranslation} from 'react-i18next'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'

import type {UiGraphSettings} from '@ecs/shared/scene-graph'

import {useUiConfiguratorStyles} from './ui-configurator-styles'
import {
  NumberOrPercentInput, useStyles as useRowStyles,
} from '../row-fields'
import {StandardFieldContainer} from '../../../ui/components/standard-field-container'
import {StandardFieldLabel} from '../../../ui/components/standard-field-label'
import {Icon} from '../../../ui/components/icon'
import {IconButton} from '../../../ui/components/icon-button'
import {combine} from '../../../common/styles'

interface IPaddingConfiguratorProps {
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
}

const UI_PADDING_PLACEHOLDER = '0'

const getIsPaddingLocked = (settings: UiGraphSettings): boolean => (
  settings.paddingTop === settings.paddingBottom &&
  settings.paddingLeft === settings.paddingRight
)

const PaddingConfigurator: React.FC<IPaddingConfiguratorProps> = ({
  settings,
  onChange,
}) => {
  const classes = useUiConfiguratorStyles()
  const rowClasses = useRowStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  const isCurrentPaddingLocked = getIsPaddingLocked(settings)
  const [paddingLockOverride, setIsPaddingLocked] = React.useState(isCurrentPaddingLocked)

  const isPaddingLocked = paddingLockOverride && isCurrentPaddingLocked

  return isPaddingLocked
    ? (
      <>
        <div className={rowClasses.row}>
          <StandardFieldLabel
            label={t('ui_configurator.ui_padding.label')}
            mutedColor
          />
        </div>
        <div
          className={combine(
            rowClasses.row, rowClasses.rowGroup, rowClasses.flexItem, rowClasses.rowAlignTop
          )}
        >
          <NumberOrPercentInput
            id='ui-padding-vertical'
            onChange={value => onChange(current => ({
              ...current,
              paddingTop: value,
              paddingBottom: value,
            }))
          }
            value={settings.paddingTop ?? UI_DEFAULTS.paddingTop}
            icon={<Icon stroke='paddingVertical' color='muted' />}
            placeholder={UI_PADDING_PLACEHOLDER}
            ariaLabel={t('ui_configurator.ui_padding_vertical.label')}
            defaultValue={UI_DEFAULTS.paddingTop}
          />
          <NumberOrPercentInput
            id='ui-padding-horizontal'
            onChange={value => onChange(current => ({
              ...current,
              paddingLeft: value,
              paddingRight: value,
            }))
          }
            value={settings.paddingLeft ?? UI_DEFAULTS.paddingLeft}
            icon={<Icon stroke='paddingHorizontal' color='muted' />}
            placeholder={UI_PADDING_PLACEHOLDER}
            ariaLabel={t('ui_configurator.ui_padding_horizontal.label')}
            defaultValue={UI_DEFAULTS.paddingLeft}
          />
          <div>
            <StandardFieldContainer>
              <div className={classes.iconButton}>
                <IconButton
                  stroke='chain'
                  text={t('ui_configurator.ui_padding.lock')}
                  onClick={() => {
                    onChange(current => ({
                      ...current,
                      paddingBottom: current.paddingTop,
                      paddingRight: current.paddingLeft,
                    }))
                    setIsPaddingLocked(false)
                  }}
                />
              </div>
            </StandardFieldContainer>
          </div>
        </div>
      </>
    )
    : (
      <>
        <div className={rowClasses.row}>
          <StandardFieldLabel
            label={t('ui_configurator.ui_padding.label')}
            mutedColor
          />
        </div>
        <div
          className={combine(
            rowClasses.row, rowClasses.rowGroup, rowClasses.flexItem, rowClasses.rowAlignTop
          )}
        >
          <div className={classes.columnContainer}>
            <NumberOrPercentInput
              id='ui-padding-top'
              onChange={value => onChange(current => ({
                ...current,
                paddingTop: value,
              }))
            }
              value={settings.paddingTop ?? UI_DEFAULTS.paddingTop}
              icon={<Icon stroke='paddingVerticalTop' color='muted' />}
              placeholder={UI_PADDING_PLACEHOLDER}
              ariaLabel={t('ui_configurator.ui_padding_top.label')}
              defaultValue={UI_DEFAULTS.paddingTop}
            />
            <NumberOrPercentInput
              id='ui-padding-bottom'
              onChange={value => onChange(current => ({
                ...current,
                paddingBottom: value,
              }))
            }
              value={settings.paddingBottom ?? UI_DEFAULTS.paddingBottom}
              icon={<Icon stroke='paddingVerticalBottom' color='muted' />}
              placeholder={UI_PADDING_PLACEHOLDER}
              ariaLabel={t('ui_configurator.ui_padding_bottom.label')}
              defaultValue={UI_DEFAULTS.paddingBottom}
            />
          </div>
          <div className={classes.columnContainer}>
            <NumberOrPercentInput
              id='ui-padding-left'
              onChange={value => onChange(current => ({
                ...current,
                paddingLeft: value,
              }))
            }
              value={settings.paddingLeft ?? UI_DEFAULTS.paddingLeft}
              icon={<Icon stroke='paddingHorizontalLeft' color='muted' />}
              placeholder={UI_PADDING_PLACEHOLDER}
              ariaLabel={t('ui_configurator.ui_padding_left.label')}
              defaultValue={UI_DEFAULTS.paddingLeft}
            />
            <NumberOrPercentInput
              id='ui-padding-right'
              onChange={value => onChange(current => ({
                ...current,
                paddingRight: value,
              }))
            }
              value={settings.paddingRight ?? UI_DEFAULTS.paddingRight}
              icon={<Icon stroke='paddingHorizontalRight' color='muted' />}
              placeholder={UI_PADDING_PLACEHOLDER}
              ariaLabel={t('ui_configurator.ui_padding_right.label')}
              defaultValue={UI_DEFAULTS.paddingRight}
            />
          </div>
          <StandardFieldContainer>
            <div className={classes.iconButton}>
              <IconButton
                stroke='chainBroken'
                text={t('ui_configurator.ui_padding.unlock')}
                onClick={() => {
                  onChange(current => ({
                    ...current,
                    paddingBottom: settings.paddingTop,
                    paddingRight: settings.paddingLeft,
                  }))
                  setIsPaddingLocked(true)
                }}
              />
            </div>
          </StandardFieldContainer>
        </div>
      </>
    )
}

export {PaddingConfigurator}

import type {UiGraphSettings} from '@ecs/shared/scene-graph'
import React from 'react'
import {useTranslation} from 'react-i18next'
import {UI_DEFAULTS} from '@ecs/shared/ui-constants'
import {numberPercentOrAuto} from '@ecs/shared/flex-styles'

import {useUiConfiguratorStyles} from './ui-configurator-styles'
import {
  NumberOrPercentInput, RowBooleanField, useStyles as useRowStyles,
} from '../row-fields'
import {Icon} from '../../../ui/components/icon'
import type {UiPropertyName} from '../ui-configurator-types'
import {StandardFieldContainer} from '../../../ui/components/standard-field-container'
import {IconButton} from '../../../ui/components/icon-button'
import {combine} from '../../../common/styles'

interface IMarginConfiguratorProps {
  settings: UiGraphSettings
  onChange: (updater: (current: UiGraphSettings) => UiGraphSettings) => void
  onDelete: (key: UiPropertyName) => void
}

const UI_MARGIN_PLACEHOLDER = '0'

const getIsMarginLocked = (settings: UiGraphSettings): boolean => (
  settings.marginTop === settings.marginBottom &&
  settings.marginLeft === settings.marginRight
)

const MarginConfigurator: React.FC<IMarginConfiguratorProps> = ({
  settings, onChange, onDelete,
}) => {
  const classes = useUiConfiguratorStyles()
  const rowClasses = useRowStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  const isCurrentMarginLocked = getIsMarginLocked(settings)
  const [marginLockOverride, setMarginLockOverride] = React.useState(isCurrentMarginLocked)

  const isMarginLocked = marginLockOverride && isCurrentMarginLocked

  const checkMargin = settings.margin !== undefined || settings.marginTop !== undefined ||
    settings.marginBottom !== undefined || settings.marginLeft !== undefined ||
    settings.marginRight !== undefined

  return (
    <>
      <RowBooleanField
        id='ui-margin'
        label={t('ui_configurator.ui_margin.label')}
        checked={checkMargin}
        onChange={(e) => {
          if (e.target.checked) {
            onChange(current => ({
              ...current,
              margin: UI_DEFAULTS.margin,
            }))
          } else {
            onDelete('margin')
            onDelete('marginTop')
            onDelete('marginRight')
            onDelete('marginBottom')
            onDelete('marginLeft')
          }
        }}
      />
      {checkMargin && (
        <div>
          {isMarginLocked
            ? (
              <div className={combine(
                rowClasses.row,
                rowClasses.rowGroup,
                rowClasses.flexItem,
                rowClasses.rowAlignTop
              )}
              >
                <NumberOrPercentInput
                  id='marginVertical'
                  onChange={value => onChange(current => ({
                    ...current,
                    marginTop: value,
                    marginBottom: value,
                  }))}
                  value={numberPercentOrAuto(settings.marginTop) ??
                  numberPercentOrAuto(settings.margin) ?? UI_DEFAULTS.marginTop}
                  placeholder={UI_MARGIN_PLACEHOLDER}
                  icon={<Icon stroke='paddingVertical' color='muted' />}
                  ariaLabel={t('ui_configurator.ui_margin.margin_vertical.label')}
                  defaultValue={UI_DEFAULTS.marginTop}
                />
                <NumberOrPercentInput
                  id='marginHorizontal'
                  onChange={value => onChange(current => ({
                    ...current,
                    marginLeft: value,
                    marginRight: value,
                  }))}
                  value={numberPercentOrAuto(settings.marginLeft) ??
                  numberPercentOrAuto(settings.margin) ?? UI_DEFAULTS.marginLeft}
                  placeholder={UI_MARGIN_PLACEHOLDER}
                  icon={<Icon stroke='paddingHorizontal' color='muted' />}
                  ariaLabel={t('ui_configurator.ui_margin.margin_horizontal.label')}
                  defaultValue={UI_DEFAULTS.marginLeft}
                />
                <StandardFieldContainer>
                  <div className={classes.iconButton}>
                    <IconButton
                      stroke='chain'
                      text={t('ui_configurator.ui_margin.lock')}
                      onClick={() => {
                        onChange(current => ({
                          ...current,
                          marginBottom: current.marginTop,
                          marginRight: current.marginLeft,
                        }))
                        setMarginLockOverride(false)
                      }}
                    />
                  </div>
                </StandardFieldContainer>
              </div>
            )
            : (
              <div className={combine(
                rowClasses.row,
                rowClasses.rowGroup,
                rowClasses.flexItem,
                rowClasses.rowAlignTop
              )}
              >
                <div className={classes.columnContainer}>
                  <NumberOrPercentInput
                    id='marginTop'
                    onChange={value => onChange(current => ({
                      ...current,
                      marginTop: value,
                    }))
                  }
                    value={numberPercentOrAuto(settings.marginTop) ??
                    numberPercentOrAuto(settings.margin) ?? UI_DEFAULTS.marginTop}
                    placeholder={UI_MARGIN_PLACEHOLDER}
                    icon={<Icon stroke='paddingVerticalTop' color='muted' />}
                    ariaLabel={t('ui_configurator.ui_margin_top.label')}
                    defaultValue={UI_DEFAULTS.marginTop}
                  />
                  <NumberOrPercentInput
                    id='marginBottom'
                    onChange={value => onChange(current => ({
                      ...current,
                      marginBottom: value,
                    }))
                  }
                    value={numberPercentOrAuto(settings.marginBottom) ??
                    numberPercentOrAuto(settings.margin) ?? UI_DEFAULTS.marginBottom}
                    placeholder={UI_MARGIN_PLACEHOLDER}
                    icon={<Icon stroke='paddingVerticalBottom' color='muted' />}
                    ariaLabel={t('ui_configurator.ui_margin_bottom.label')}
                    defaultValue={UI_DEFAULTS.marginBottom}
                  />
                </div>
                <div className={classes.columnContainer}>
                  <NumberOrPercentInput
                    id='marginLeft'
                    onChange={value => onChange(current => ({
                      ...current,
                      marginLeft: value,
                    }))
                  }
                    value={numberPercentOrAuto(settings.marginLeft) ??
                    numberPercentOrAuto(settings.margin) ?? UI_DEFAULTS.marginLeft}
                    placeholder={UI_MARGIN_PLACEHOLDER}
                    icon={<Icon stroke='paddingHorizontalLeft' color='muted' />}
                    ariaLabel={t('ui_configurator.ui_margin_left.label')}
                    defaultValue={UI_DEFAULTS.marginLeft}
                  />
                  <NumberOrPercentInput
                    id='marginRight'
                    onChange={value => onChange(current => ({
                      ...current,
                      marginRight: value,
                    }))
                  }
                    value={numberPercentOrAuto(settings.marginRight) ??
                    numberPercentOrAuto(settings.margin) ?? UI_DEFAULTS.marginRight}
                    placeholder={UI_MARGIN_PLACEHOLDER}
                    icon={<Icon stroke='paddingHorizontalRight' color='muted' />}
                    ariaLabel={t('ui_configurator.ui_margin_right.label')}
                    defaultValue={UI_DEFAULTS.marginRight}
                  />
                </div>
                <StandardFieldContainer>
                  <div className={classes.iconButton}>
                    <IconButton
                      stroke='chainBroken'
                      text={t('ui_configurator.ui_margin.unlock')}
                      onClick={() => {
                        onChange(current => ({
                          ...current,
                          marginBottom: settings.marginTop,
                          marginRight: settings.marginLeft,
                        }))
                        setMarginLockOverride(true)
                      }}
                    />
                  </div>
                </StandardFieldContainer>
              </div>
            )}
        </div>
      )}
    </>
  )
}

export {MarginConfigurator}

import React from 'react'

import {useTranslation} from 'react-i18next'

import type {UiGraphSettings} from '@ecs/shared/scene-graph'

import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {createThemedStyles} from '../../ui/theme'
import {combine} from '../../common/styles'
import {useStyles as useRowFieldStyles} from './row-fields'
import {Icon} from '../../ui/components/icon'
import {SelectMenu} from '../ui/select-menu'
import {MenuOption, MenuOptions} from '../ui/option-menu'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import type {UiGroup} from './ui-configurator-types'
import {isUiGroupPresent} from './ui/ui-group-definition'
import {RowContent} from './row-content'

const useStyles = createThemedStyles(theme => ({
  button: {
    display: 'flex',
    gap: '1em',
    alignItems: 'center',
    justifyContent: 'space-between',
    overflow: 'hidden',
    whiteSpace: 'nowrap',
  },
  placeholder: {
    color: theme.fgMuted,
    fontStyle: 'italic',
  },
  floatingMenu: {
    padding: 0,
    gap: 0,
  },
  menuContainer: {
    padding: '0.5em',
    maxHeight: '200px',
    overflow: 'auto',
  },
}))

interface IGroupedUIPropertySelector {
  onSelect: (property: UiGroup) => void
  settings: UiGraphSettings
}

const GroupedUIPropertySelector: React.FC<IGroupedUIPropertySelector> = ({onSelect, settings}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()
  const studioClasses = useStudioMenuStyles()
  const rowFieldStyles = useRowFieldStyles()

  const maybeGroup = (group: UiGroup, content: string): MenuOption | null => {
    if (isUiGroupPresent(group, settings)) {
      return null
    }
    return {
      content,
      onClick: () => onSelect(group),
    }
  }

  const options: MenuOption[] = [
    maybeGroup('background', t('ui_property_strings.category.background')),
    maybeGroup('border', t('ui_property_strings.category.border')),
    maybeGroup('layout', t('ui_property_strings.category.layout')),
    maybeGroup('text', t('ui_property_strings.category.text')),
  ].filter(Boolean)

  if (options.length === 0) {
    return null
  }

  return (
    <RowContent>
      <StandardFieldContainer>
        <SelectMenu
          id='ui-group-select'
          menuWrapperClassName={combine(
            studioClasses.studioMenu,
            classes.floatingMenu,
            classes.menuContainer
          )}
          trigger={(
            <div className={classes.button}>
              <button
                className={combine('style-reset', rowFieldStyles.select, classes.button)}
                type='button'
              >
                <div className={classes.placeholder}>{t('ui_property_selector.placeholder')}</div>
                <div className={studioClasses.chevronIcon}>
                  <Icon block color='gray4' stroke='chevronDown' />
                </div>
              </button>
            </div>
          )}
          matchTriggerWidth
        >
          {collapse => (
            <MenuOptions
              collapse={collapse}
              options={options}
            />
          )}
        </SelectMenu>
      </StandardFieldContainer>
    </RowContent>
  )
}

export {
  GroupedUIPropertySelector,
}

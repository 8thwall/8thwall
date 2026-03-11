import React from 'react'
import {useTranslation} from 'react-i18next'

import {Icon} from '../../ui/components/icon'
import {ALL_INPUT_CATEGORIES, validateInput} from './input-strings'
import {combine} from '../../common/styles'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {createThemedStyles} from '../../ui/theme'
import {useStyles as useRowFieldStyles} from './row-fields'
import {SubMenuSelectWithSearch} from '../ui/submenu-select-with-search'

const useStyles = createThemedStyles(theme => ({
  floatingMenu: {
    padding: 0,
    gap: 0,
  },
  searchResultsContainer: {
    padding: '0.5em',
    maxHeight: '200px',
    overflow: 'auto',
  },
  selectContainer: {
    flex: 1,
    fontSize: '12px',
  },
  button: {
    display: 'flex',
    gap: '1em',
    alignItems: 'center',
    justifyContent: 'space-between',
    overflow: 'hidden',
    whiteSpace: 'nowrap',
    padding: '0.325em 0.5em',
  },
  searchInputContainer: {
    margin: '0.5em 0.5em 0 0.5em',
  },
  placeholder: {
    color: theme.fgMuted,
    fontStyle: 'italic',
    padding: '0.125rem 0',
  },
  selectedBinding: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5em',
  },
}))

interface IBindingConfiguratorMenu {
  name: string
  onChange: (modifier: string) => void
  placeholder: string
}

const BindingConfiguratorMenu: React.FC<IBindingConfiguratorMenu> = (
  {name, onChange, placeholder}
) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages'])
  const studioClasses = useStudioMenuStyles()
  const rowFieldStyles = useRowFieldStyles()
  const binding = validateInput(name)

  return (
    <div className={classes.selectContainer}>
      <SubMenuSelectWithSearch
        onChange={onChange}
        categories={ALL_INPUT_CATEGORIES}
        trigger={(
          <button
            className={combine('style-reset', rowFieldStyles.select, classes.button)}
            type='button'
          >
            {binding &&
              <span className={classes.selectedBinding}>
                <Icon stroke={binding?.icon} />
                {binding?.ns ? t(binding.content.toString()) : binding?.content.toString()}
              </span>
                }
            {!binding && <div className={classes.placeholder}>{placeholder}</div>}
            <div className={studioClasses.chevronIcon}>
              <Icon block color='gray4' stroke='chevronDown' />
            </div>
          </button>
        )}
      />
    </div>
  )
}

export {
  BindingConfiguratorMenu,
}

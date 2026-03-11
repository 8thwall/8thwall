import React from 'react'

import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../../ui/theme'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {Icon} from '../../ui/components/icon'
import {combine} from '../../common/styles'
import {SelectMenu} from './select-menu'
import {useStudioMenuStyles} from './studio-menu-styles'
import {FloatingMenuButton} from '../../ui/components/floating-menu-button'

const useStyles = createThemedStyles(theme => ({
  searchInputContainer: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    padding: '0.25rem 0',
    position: 'relative',
    flexGrow: 1,
    minWidth: 0,
  },
  searchInput: {
    'paddingLeft': '2.25em',
    'flexGrow': 1,
    'minWidth': 0,
    'fontSize': '12px',
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
  },
  searchIcon: {
    color: theme.fgMuted,
    display: 'flex',
    alignItems: 'center',
    padding: '0.25em 0.5em',
    position: 'absolute',
    left: 0,
    pointerEvents: 'none',
  },
  filterIcon: {
    'color': theme.fgMuted,
    'display': 'flex',
    'alignItems': 'center',
    'cursor': 'pointer',
    'padding': '0.25rem 0.5rem',
    '&:hover': {
      color: theme.fgMain,
    },
  },
  filterButton: {
    position: 'relative',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    width: '100%',
    minWidth: '6rem',
  },
  checkmark: {
    marginRight: '-0.25rem',
  },
  searchToolBar: {
    display: 'flex',
    justifyContent: 'space-between',
    padding: '0.5rem 1rem 0.25rem 1rem',
  },
  resultCount: {
    color: theme.fgMuted,
    fontSize: '12px',
  },
  clearSearchBtn: {
    'color': theme.fgMuted,
    'cursor': 'pointer',
    'fontSize': '12px',
    '&:hover': {
      color: theme.fgMain,
    },
  },
}))

interface ISearchBar {
  searchText: string
  setSearchText: (text: string) => void
  filterOptions?: (collapse: () => void) => React.ReactNode
  focusOnOpen?: boolean
  a8?: string
  disabled?: boolean
  onSubmit?: () => void
  onFocus?: () => void
  onBlur?: () => void
}

const SearchBar: React.FC<ISearchBar> = ({
  searchText, setSearchText, filterOptions, focusOnOpen, disabled, onSubmit, ...rest
}) => {
  const {t} = useTranslation(['common'])
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()
  const alreadyFocusedRef = React.useRef(false)

  const filterMenu = filterOptions && (
    <SelectMenu
      id='add-component-select-menu'
      menuWrapperClassName={menuStyles.studioMenu}
      trigger={(
        <div className={classes.filterIcon}>
          <Icon stroke='filter' />
        </div>
      )}
    >
      {filterOptions}
    </SelectMenu>
  )

  return (
    <StandardFieldContainer grow>
      <div className={classes.searchInputContainer}>
        <div className={classes.searchIcon}>
          <Icon stroke='search' />
        </div>
        <input
          ref={(el) => {
            if (el && focusOnOpen && !alreadyFocusedRef.current) {
              el.focus()
              alreadyFocusedRef.current = true
            }
          }}
          type='text'
          value={searchText}
          disabled={disabled}
          onChange={(e) => { setSearchText(e.target.value) }}
          className={combine('style-reset', classes.searchInput)}
          placeholder={t('input.placeholder.search')}
          onKeyDown={onSubmit && ((e) => {
            // eslint-disable-next-line local-rules/hardcoded-copy
            if (e.key === 'Enter') {
              onSubmit()
            }
          })}
          {...rest}
        />
        {filterMenu}
      </div>
    </StandardFieldContainer>
  )
}

interface IOptionalFilters {
  options: {type: string, label: string}[]
  noneOption: string
  filters: string[]
  setFilters: (filters: string[]) => void
  collapse: () => void
  translationNamespace?: string
}

const OptionalFilters: React.FC<IOptionalFilters> = ({
  options, noneOption, filters, setFilters, collapse,
  translationNamespace = 'cloud-studio-pages',
}) => {
  const {t} = useTranslation(translationNamespace)
  const classes = useStyles()
  return (
    <>
      <FloatingMenuButton
        onClick={() => {
          setFilters([])
          collapse()
        }}
      >
        <div className={classes.filterButton}>
          <div>{noneOption}</div>
          {filters.length === 0 &&
            <div className={classes.checkmark}>
              <Icon stroke='checkmark' color='highlight' block />
            </div>
          }
        </div>
      </FloatingMenuButton>
      {options.map(({type, label}) => (
        <FloatingMenuButton
          key={label}
          onClick={(e) => {
            if (e.shiftKey) {
              setFilters([...filters, type])
            } else {
              setFilters([type])
            }
            collapse()
          }}
        >
          <div className={classes.filterButton}>
            <div>{t(label)}</div>
            {filters.includes(type) &&
              <div className={classes.checkmark}>
                <Icon stroke='checkmark' color='highlight' block />
              </div>
            }
          </div>
        </FloatingMenuButton>
      ))}
    </>
  )
}

interface ISearchToolbar {
  resultCount: number
  showingPartialResult?: boolean
  clearSearch: () => void
}

const SearchToolbar: React.FC<ISearchToolbar> = ({
  resultCount, showingPartialResult = false, clearSearch,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()
  return (
    <div className={classes.searchToolBar}>
      <div className={classes.resultCount}>
        {t(showingPartialResult
          ? 'tree_hierarchy_search_results.incomplete_results_length'
          : 'tree_hierarchy_search_results.results_length',
        {count: resultCount})}
      </div>
      <button
        type='button'
        onClick={clearSearch}
        className={combine('style-reset', classes.clearSearchBtn)}
      >
        {t('button.clear_search', {ns: 'common'})}
      </button>
    </div>
  )
}

export {
  SearchBar,
  OptionalFilters,
  SearchToolbar,
}

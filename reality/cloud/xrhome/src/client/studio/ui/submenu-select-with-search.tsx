import React from 'react'

import {createUseStyles} from 'react-jss'

import {useTranslation} from 'react-i18next'

import {SubMenuCategory, SubMenuOptionsListing, SubMenuView} from './submenu'
import {SearchBar} from './search-bar'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {SelectMenu} from './select-menu'
import {useStudioMenuStyles} from './studio-menu-styles'
import {combine} from '../../common/styles'

const useStyles = createUseStyles({
  searchResultsContainer: {
    padding: '0.5em',
    maxHeight: '200px',
    overflow: 'auto',
  },
  searchInputContainer: {
    margin: '0.5em 0.5em 0 0.5em',
  },
  floatingMenu: {
    padding: 0,
    gap: 0,
  },
})

interface ISubMenuSelectWithSearch {
  trigger: React.ReactNode
  onChange: (newValue: string, category: string) => void
  categories: SubMenuCategory[]
  onCategoryChange?: (category: string | null) => void
  disabled?: boolean
  matchTriggerWidth?: boolean
  a8?: string
}

const SubMenuSelectWithSearch: React.FC<ISubMenuSelectWithSearch> = ({
  onChange, categories, trigger, onCategoryChange, disabled, matchTriggerWidth, a8,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages'])
  const studioClasses = useStudioMenuStyles()
  const [rawSearchText, setSearchText] = React.useState('')
  const searchText = rawSearchText.toLowerCase()
  const [searchFocused, setSearchFocused] = React.useState(false)

  const searchResults = searchText
    ? categories.map(category => (
      category.options
        .filter((option) => {
          const optionContent = !option.ns ? option.content : t(option.content, {ns: option.ns})
          return optionContent.toLowerCase()
            .includes(searchText.toLowerCase())
        }).map(option => ({...option, category: category.value})))).flat()
    : null

  return (
    <StandardFieldContainer>
      <SelectMenu
        id={React.useId()}
        trigger={trigger}
        matchTriggerWidth={matchTriggerWidth}
        minTriggerWidth={!matchTriggerWidth}
        menuWrapperClassName={combine(studioClasses.studioMenu, classes.floatingMenu)}
        placement='bottom-end'
        margin={5}
        onOpenChange={open => (!open && setSearchText(''))}
        disabled={disabled}
      >
        {collapse => (

          <>
            <div className={classes.searchInputContainer}>
              <SearchBar
                searchText={searchText}
                setSearchText={setSearchText}
                a8={a8}
                focusOnOpen
                onFocus={() => setSearchFocused(true)}
                onBlur={() => setSearchFocused(false)}
                onSubmit={() => {
                  const firstResult = searchResults?.[0]
                  if (firstResult) {
                    collapse()
                    setSearchText('')
                    onChange(firstResult.value, firstResult.category)
                  }
                }}
              />
            </div>
            {searchResults &&
              <div className={classes.searchResultsContainer}>
                <SubMenuOptionsListing
                  options={searchResults}
                  onChange={onChange}
                  firstActive={searchFocused}
                  onCollapse={() => {
                    setSearchText('')
                    collapse()
                  }}
                />
              </div>
            }
            {!searchResults &&
              <SubMenuView
                onOptionChange={onChange}
                onCollapse={() => {
                  setSearchText('')
                  collapse()
                }}
                optionCategories={categories}
                onCategoryChange={onCategoryChange}
              />
            }
          </>
        )}
      </SelectMenu>
    </StandardFieldContainer>
  )
}

export {SubMenuSelectWithSearch}

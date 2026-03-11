import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'

import {FloatingMenuButton} from '../../ui/components/floating-menu-button'
import {Icon, IconStroke} from '../../ui/components/icon'
import {SubMenuHeading} from './submenu-heading'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  subMenu: {
    padding: '0 0.5em',
  },
  subMenuContainer: {
    padding: '0.5em',
    maxHeight: '200px',
    overflow: 'auto',
  },
  categoryButton: {
    width: '100%',
    display: 'flex',
    justifyContent: 'space-between',
  },
  categoryChevron: {
    '& svg': {
      width: '0.75em',
      height: '0.75em',
    },
  },
  subMenuOption: {
    'width': '100%',
    'display': 'flex',
    'justifyContent': 'space-between',
    '& svg': {
      color: theme.fgMain,
    },
  },
}))

type SubMenuItem = {
  value: string
  content: string
  icon?: IconStroke
  ns?: string
}

type SubMenuCategory = {
  parent: string | null
  value: string
  options?: DeepReadonly<SubMenuItem[]>
}

type SubMenuOption = SubMenuItem & {
  category: string | undefined  // When in a flat list, we need the category included
}

interface ISubMenuOptionsListing {
  options: DeepReadonly<SubMenuOption[]>
  onChange: (newValue: string, category: string | undefined) => void
  onCollapse: () => void
  firstActive?: boolean
}

type OptionType = 'option' | 'category'

const make8Attribute = (option: {value?: string | null}, optionType: OptionType) => {
  if (!option?.value || typeof option.value !== 'string') {
    return ''
  }

  return `click;studio-right-panel;${optionType}-${option.value
    .split('.')
    .pop()
    .toLowerCase()}-click`
}

const SubMenuOptionsListing: React.FC<ISubMenuOptionsListing> = ({
  options, onChange, onCollapse, firstActive,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages'])

  return (
    <>
      {options.map((option, i) => {
        const a8 = make8Attribute(option, 'option')
        return (
          <FloatingMenuButton
            key={option.value}
            a8={a8}
            isActive={firstActive && i === 0}
            onClick={(e) => {
              e.stopPropagation()
              onChange(option.value, option.category)
              onCollapse()
            }}
          >
            <div className={classes.subMenuOption}>
              {option.ns ? t(option.content.toString(), {ns: option.ns}) : option.content}
              {option.icon && <Icon stroke={option.icon} />}
            </div>
          </FloatingMenuButton>
        )
      })}
    </>
  )
}

interface ISubMenuView {
  onOptionChange: (newValue: string, category: string) => void
  onCategoryChange?: (category: string | null) => void
  onCollapse: () => void
  optionCategories: SubMenuCategory[]
}

const SubMenuView: React.FC<ISubMenuView> = (
  {onOptionChange, onCategoryChange, onCollapse, optionCategories}
) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages'])
  const [currentCategoryValue, setCurrentCategoryValue] = React.useState<string | null>(null)
  const currentCategory = optionCategories.find(category => category.value === currentCategoryValue)
  const categories = optionCategories
    .filter(category => category.parent === (currentCategoryValue || null) &&
      category.value !== null)

  return (
    <div>
      {currentCategoryValue &&
        <div className={classes.subMenu}>
          <SubMenuHeading
            title={t(currentCategoryValue)}
            onBackClick={() => {
              setCurrentCategoryValue(currentCategory.parent)
              if (onCategoryChange) { onCategoryChange(currentCategory.parent) }
            }}
            compact
          />
        </div>
      }
      <div className={classes.subMenuContainer}>
        {categories.map((category) => {
          const a8 = make8Attribute(category, 'category')
          return (
            <FloatingMenuButton
              key={category.value}
              a8={a8}
              onClick={(e) => {
                e.stopPropagation()
                setCurrentCategoryValue(category.value)
                if (onCategoryChange) {
                  onCategoryChange(category.value)
                }
              }}
            >
              <div className={classes.categoryButton}>
                {t(category.value)}
                <div className={classes.categoryChevron}>
                  <Icon stroke='chevronRight' />
                </div>
              </div>
            </FloatingMenuButton>
          )
        })}
        {currentCategory?.options &&
          <SubMenuOptionsListing
            options={currentCategory.options.map(option => ({
              ...option,
              category: currentCategory.value,
            }))}
            onChange={(newValue, category) => {
              onOptionChange(newValue, category)
              setCurrentCategoryValue(null)
            }}
            onCollapse={onCollapse}
          />
        }
      </div>
    </div>
  )
}

export {
  SubMenuItem,
  SubMenuCategory,
  SubMenuView,
  SubMenuOptionsListing,
}

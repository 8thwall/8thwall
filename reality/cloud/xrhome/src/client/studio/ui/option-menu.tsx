import React from 'react'

import {createUseStyles} from 'react-jss'

import {
  FloatingMenuButton,
  useFloatingMenuButtonStyles,
} from '../../ui/components/floating-menu-button'
import {SelectMenu} from './select-menu'
import {combine} from '../../common/styles'
import {Icon} from '../../ui/components/icon'
import {useStudioMenuStyles} from './studio-menu-styles'

const useStyles = createUseStyles({
  menuOpen: {
    display: 'flex',
    width: 'fit-content',
  },
  categoryButton: {
    width: '100%',
    display: 'flex',
    justifyContent: 'space-between',
  },
  categoryChevron: {
    'marginLeft': '0.5em',
    '& svg': {
      width: '0.75em',
      height: '0.75em',
    },
  },
  nowrap: {
    whiteSpace: 'nowrap',
    display: 'flex',
    width: '100%',
  },
})

interface MenuItem {
  type?: 'item' | undefined
  content: React.ReactNode
  onClick: () => void
  isDisabled?: boolean
  a8?: string
}

interface MenuDivider {
  type: 'divider'
}

interface MenuCategory {
  type: 'menu'
  content: React.ReactNode
  options: MenuOption[]
  isDisabled?: boolean
  a8?: string
}

type MenuOption = MenuItem | MenuDivider | MenuCategory

interface IOptionMenu {
  options: MenuOption[]
  collapse: () => void
  returnFocus?: boolean
}

const MenuOptions: React.FC<IOptionMenu> = ({
  options, collapse: outerCollapse, returnFocus = true,
}) => {
  const classes = useStyles()
  const buttonStyles = useFloatingMenuButtonStyles()
  const studioClasses = useStudioMenuStyles()

  const optionKey = (option: MenuOption, idx: number) => (
    'content' in option && typeof option.content === 'string' ? option.content : idx)

  const renderMenuItem = (option: MenuItem, idx: number, onClick: () => void) => (
    <FloatingMenuButton
      key={optionKey(option, idx)}
      onClick={() => {
        if (option.onClick) {
          option.onClick()
        }
        onClick()
      }}
      isDisabled={option.isDisabled}
    >
      <div className={classes.nowrap}>{option.content}</div>
    </FloatingMenuButton>
  )

  const renderMenuOption = (option: MenuOption, idx: number, collapse: () => void) => {
    if (option.type === 'menu') {
      if (!option.options?.length) {
        return null
      }
      return (
        <SelectMenu
          key={optionKey(option, idx)}
          id={`${option.content}-select-menu`}
          minTriggerWidth
          menuWrapperClassName={classes.menuOpen}
          trigger={(
            <button
              type='button'
              className={combine('style-reset', buttonStyles.floatingMenuButton)}
            >
              <div className={combine(classes.categoryButton, classes.nowrap)}>
                {option.content}
                <div className={classes.categoryChevron}>
                  <Icon stroke='chevronRight' />
                </div>
              </div>
            </button>
          )}
          placement='right-start'
          margin={12}
          returnFocus={returnFocus}
        >
          {collapseSubmenu => (
            <div className={studioClasses.studioMenu}>
              {option.options.map((component, subIdx) => renderMenuOption(component, subIdx, () => {
                collapseSubmenu()
                collapse()
              })) }
            </div>
          )}
        </SelectMenu>
      )
    } else if (option.type === 'divider') {
      // eslint-disable-next-line react/no-array-index-key
      return <div className={studioClasses.divider} key={optionKey(option, idx)} />
    } else {
      return renderMenuItem(option, idx, collapse)
    }
  }
  return (
    <>
      {options.filter(Boolean).map((option, idx) => renderMenuOption(option, idx, outerCollapse))}
    </>
  )
}

export {
  MenuOptions,
  MenuOption,
}

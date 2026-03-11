import React from 'react'

import {createThemedStyles} from '../../ui/theme'
import {Icon} from '../../ui/components/icon'
import {SelectMenu} from './select-menu'
import {useStudioMenuStyles} from './studio-menu-styles'

const useStyles = createThemedStyles(theme => ({
  cornerMenuContainer: {
    color: theme.fgMain,
    position: 'relative',
    display: 'flex',
    alignItems: 'center',
  },
  cornerMenuButton: {
    'padding': '0 0.5em',
    'display': 'flex',
    'color': theme.fgMuted,
    'background': 'transparent',
    'border': 'none',
    'cursor': 'pointer',
    '&:hover': {
      color: theme.fgMain,
    },
  },
}))

interface ICornerDropdownMenu {
  children: (collapse: () => void) => React.ReactNode
  disabled?: boolean
}

const CornerDropdownMenu: React.FC<ICornerDropdownMenu> = ({
  children, disabled,
}) => {
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()

  return (
    <div className={classes.cornerMenuContainer}>
      <SelectMenu
        id='corner-dropdown-menu'
        menuWrapperClassName={menuStyles.studioMenu}
        placement='right-start'
        margin={-3}
        trigger={(
          <button
            aria-haspopup='menu'
            aria-disabled={disabled}
            className={classes.cornerMenuButton}
            type='button'
          >
            <Icon stroke='kebab' />
          </button>
        )}
      >
        {children}
      </SelectMenu>
    </div>
  )
}

export {
  CornerDropdownMenu,
}

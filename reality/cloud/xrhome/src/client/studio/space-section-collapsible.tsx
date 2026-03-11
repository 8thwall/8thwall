import React from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../ui/theme'
import {MenuOptions, type MenuOption} from './ui/option-menu'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {combine} from '../common/styles'
import {CornerDropdownMenu} from './ui/corner-dropdown-menu'
import {useStudioMenuStyles} from './ui/studio-menu-styles'
import {useSceneContext} from './scene-context'
import {useActiveSpace} from './hooks/active-space'
import {updateIncludedSpaces} from './configuration/included-spaces'

const useStyles = createThemedStyles(theme => ({
  collapseContainer: {
    'display': 'flex',
    'flexDirection': 'column',
    '&:last-child': {
      flexGrow: 1,
    },
  },
  collapseHeader: {
    display: 'flex',
    padding: '0 0.25rem',
  },
  collapseButton: {
    'padding': '0.25rem 0.5rem',
    'display': 'flex',
    'cursor': 'pointer',
    'justifyContent': 'space-between',
    'userSelect': 'none',
    'color': theme.fgMain,
    '&:hover': {
      color: theme.fgMuted,
    },
    'flex': 1,
    'gap': '0.5em',
    'alignItems': 'center',
    'fontSize': '12px',
    'border': theme.studioSectionBorder,
    'borderRadius': '0.5rem',
  },
  contextMenu: {
    paddingTop: '0.4em',
  },
}))

interface ISpaceSectionCollapsible {
  children: React.ReactNode
  title: React.ReactNode
  spaceId: string
  contextMenuOptions?: MenuOption[]
}

const SpaceSectionCollapsible: React.FC<ISpaceSectionCollapsible> = ({
  children, title, contextMenuOptions, spaceId,
}) => {
  const {t} = useTranslation(['common'])
  const classes = useStyles()
  const styles = useStudioMenuStyles()
  const [isCollapsed, setCollapsed] = React.useState(false)
  const ctx = useSceneContext()
  const menuState = useContextMenuState()
  const {spaces} = ctx.scene
  const activeSpace = useActiveSpace()

  const includedSpaces = activeSpace?.id ? spaces[activeSpace.id].includedSpaces : []

  const collapseContextMenu = () => {
    menuState.setContextMenuOpen(false)
  }

  const onToggleCollapse = () => {
    setCollapsed(!isCollapsed)
    collapseContextMenu()
  }

  return (
    <div
      className={classes.collapseContainer}
      ref={menuState.refs.setReference}
      {...menuState.getReferenceProps()}
    >
      <div className={combine(classes.collapseHeader, styles.menuItemContainer)}>
        <button
          type='button'
          className={combine(classes.collapseButton, 'style-reset')}
          onClick={onToggleCollapse}
          onContextMenu={contextMenuOptions ? menuState.handleContextMenu : undefined}
        >
          {title}
        </button>
        {spaceId &&
          <div className={combine(styles.iconContainer, styles.iconButtons, classes.contextMenu)}>
            <CornerDropdownMenu>
              {collapse => (
                <MenuOptions
                  options={[
                    {
                      content: t('button.remove', {ns: 'common'}),
                      onClick: () => (updateIncludedSpaces(
                        spaceId,
                        ctx,
                        includedSpaces,
                        activeSpace
                      )),
                    },
                  ]}
                  collapse={collapse}
                />
              )}
            </CornerDropdownMenu>
          </div>
        }
      </div>
      {!isCollapsed && children}
      {contextMenuOptions &&
        <ContextMenu
          menuState={menuState}
          options={contextMenuOptions}
        />
      }
    </div>
  )
}

export {
  SpaceSectionCollapsible,
}

import React from 'react'
import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'
import type {Space} from '@ecs/shared/scene-graph'

import {useStudioMenuStyles} from './ui/studio-menu-styles'
import {useSceneContext} from './scene-context'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {useSpaceMenuOptions} from './space-menu-options'
import {createThemedStyles} from '../ui/theme'
import {combine} from '../common/styles'
import {RenameSpaceForm, useRenameSpaceState} from './rename-space'
import {Popup} from '../ui/components/popup'
import {Icon} from '../ui/components/icon'
import {CornerDropdownMenu} from './ui/corner-dropdown-menu'
import {MenuOptions} from './ui/option-menu'
import {SpaceChip} from './configuration/diff-chip'

const useStyles = createThemedStyles(theme => ({
  entrySpaceIcon: {
    'color': theme.fgMuted,
    'margin': '0 1.5em 0 auto',
    'padding': '0 0.25em',
  },
  contextMenu: {
    paddingTop: '4px',
  },
  rowWrapper: {
    position: 'relative',
  },
}))

interface ISpaceItem {
  space: DeepReadonly<Space>
  spaces: DeepReadonly<Space>[]
  active: boolean
  onClick: () => void
  onRename: (newName: string) => void
  setDuplicateName: (duplicate: boolean) => void
  duplicateName: boolean
}

const SpaceItem: React.FC<ISpaceItem> = ({
  space, spaces, onClick, active, onRename, setDuplicateName, duplicateName,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStudioMenuStyles()
  const styles = useStyles()
  const ctx = useSceneContext()
  const menuState = useContextMenuState()
  const [isHovering, setIsHovering] = React.useState(false)

  const renameState = useRenameSpaceState(space)
  const spacesMenuOptions = useSpaceMenuOptions(space, renameState.startRename)

  const handleRename = () => {
    if (duplicateName) {
      return
    }
    onRename(renameState.spaceName)
    renameState.setIsRenaming(false)
  }

  const handleSetSpaceName = (name: string) => {
    if (spaces.some(s => s.name === name && s.id !== space.id)) {
      setDuplicateName(true)
    } else if (duplicateName) {
      setDuplicateName(false)
    }
  }

  const isCurrentEntrySpace = ctx.scene.entrySpaceId === space.id

  return renameState.isRenaming
    ? (
      <RenameSpaceForm
        value={renameState.spaceName}
        renameState={renameState}
        onRename={handleRename}
        onBlur={() => setDuplicateName(false)}
        onChange={value => handleSetSpaceName(value)}
        onEscapeKey={() => {
          setDuplicateName(false)
        }}
      />
    )
    : (
      <div
        ref={menuState.refs.setReference}
        {...menuState.getReferenceProps()}
        className={styles.rowWrapper}
      >
        <SpaceChip spaceId={space.id} />
        <div
          className={classes.menuItemContainer}
          onMouseEnter={() => setIsHovering(true)}
          onMouseLeave={() => setIsHovering(false)}
        >
          <button
            type='button'
            className={combine('style-reset', classes.menuItem)}
            onClick={onClick}
            onContextMenu={menuState.handleContextMenu}
          >
            {space.name}
            {isCurrentEntrySpace &&
              <div className={styles.entrySpaceIcon}>
                <Popup
                  content={t('spaces_menu.popup.entry_space')}
                  position='bottom'
                  alignment='right'
                  size='tiny'
                  delay={250}
                >
                  <Icon stroke='entrySpace' block />
                </Popup>
              </div>
          }
          </button>
          {active && !isHovering &&
            <div className={combine(classes.iconContainer, classes.checkmark)}>
              <Icon stroke='checkmark' color='highlight' block />
            </div>
        }
          <div
            className={combine(classes.iconContainer,
              (active && !isHovering) ? classes.activeIconButton : classes.iconButtons,
              styles.contextMenu)}
          >
            <CornerDropdownMenu>
              {collapse => (
                <MenuOptions options={spacesMenuOptions} collapse={collapse} />
              )}
            </CornerDropdownMenu>
          </div>
          <ContextMenu
            menuState={menuState}
            options={spacesMenuOptions}
          />
        </div>
      </div>
    )
}

export {
  SpaceItem,
}

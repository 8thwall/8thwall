import React from 'react'
import {v4 as uuid} from 'uuid'
import type {DeepReadonly} from 'ts-essentials'
import type {Space} from '@ecs/shared/scene-graph'

import {useTranslation} from 'react-i18next'

import {SelectMenu} from './ui/select-menu'
import {FloatingMenuButton} from '../ui/components/floating-menu-button'
import {combine} from '../common/styles'
import {Icon} from '../ui/components/icon'
import {StandardTextField} from '../ui/components/standard-text-field'
import {SrOnly} from '../ui/components/sr-only'
import {useStudioMenuStyles} from './ui/studio-menu-styles'
import {useSceneContext} from './scene-context'
import {useStudioStateContext} from './studio-state-context'
import {useActiveSpace} from './hooks/active-space'
import {StaticBanner} from '../ui/components/banner'
import {Keys} from './common/keys'
import {createThemedStyles} from '../ui/theme'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {useSpaceMenuOptions} from './space-menu-options'
import {RenameSpaceForm, useRenameSpaceState} from './rename-space'
import {SpaceItem} from './space-menu-item'
import {useDerivedScene} from './derived-scene-context'
import {addSpace, switchSpace, renameSpace} from './spaces'

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const useStyles = createThemedStyles(theme => ({
  menuTitle: {
    'display': 'flex',
    'width': '100%',
    'gap': '1em',
    'alignItems': 'center',
    'justifyContent': 'space-between',
    'overflow': 'hidden',
    'fontSize': '12px',
    'padding': '0.5rem 1rem',
    'color': theme.fgMain,
    'cursor': 'pointer',
    'user-select': 'none',
    '&:hover': {
      color: theme.fgMuted,
    },
    'borderBottom': theme.studioSectionBorder,
  },
  spaceName: {
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
    overflow: 'hidden',
  },
  icon: {
    margin: '0.325em 0',
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
  menuSpaceRenameForm: {
    width: '100%',
  },
}))

interface ISpaceTitle {
  isOpen: boolean
  onContextMenu?: React.MouseEventHandler<HTMLDivElement>
  children: React.ReactNode
}

const SpaceTitle: React.FC<ISpaceTitle> = ({isOpen, onContextMenu, children}) => {
  const classes = useStudioMenuStyles()
  const styles = useStyles()

  return (
    <div className={styles.menuTitle} onContextMenu={onContextMenu}>
      {children}
      <div
        className={
          combine(styles.icon, classes.chevronIcon, isOpen && classes.openIcon)
        }
      >
        <Icon block color='gray4' stroke='chevronDown' />
      </div>
    </div>
  )
}

const SpacesMenu: React.FC = () => {
  const classes = useStudioMenuStyles()
  const styles = useStyles()
  const {t} = useTranslation(['cloud-studio-pages'])
  const menuState = useContextMenuState()
  const [isOpen, setIsOpen] = React.useState(false)
  const [newSpaceName, setNewSpaceName] = React.useState('')
  const [isAdding, setIsAdding] = React.useState(false)
  const [duplicateName, setDuplicateName] = React.useState(false)

  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const spaces = ctx.scene.spaces ? Object.values(ctx.scene.spaces) : []
  const activeSpace = useActiveSpace()
  const renameState = useRenameSpaceState(activeSpace)

  React.useEffect(() => {
    if (activeSpace?.id !== stateCtx.state.activeSpace) {
      stateCtx.update(s => ({...s, activeSpace: activeSpace?.id ?? undefined}))
    }
  }, [activeSpace])

  const handleSelectSpace = (id: string) => {
    switchSpace(stateCtx, derivedScene, id)
    setDuplicateName(false)
    setIsOpen(false)
  }

  const startAddSpace = () => {
    setIsAdding(true)
    setNewSpaceName('')
  }

  const baseSpaceContextMenu = useSpaceMenuOptions(activeSpace, renameState.startRename)
  const activeSpaceContextMenuOptions = [
    ...baseSpaceContextMenu,
    {
      content: t('spaces_menu.button.create_new_space'),
      onClick: () => {
        setIsOpen(true)
        startAddSpace()
      },
    },
  ]

  const handleAddSpace = () => {
    const id = uuid()
    ctx.updateScene(scene => addSpace(scene, newSpaceName, t, id))
    handleSelectSpace(id)
  }

  const handleSetSpaceName = (name: string, spaceRename?: DeepReadonly<Space>) => {
    if (spaces.some(s => s.name === name && (s.id !== spaceRename?.id))) {
      setDuplicateName(true)
    } else if (duplicateName) {
      setDuplicateName(false)
    }
    if (spaceRename) {
      renameState.setSpaceName(name)
    } else {
      setNewSpaceName(name)
    }
  }

  const cancelAddSpace = () => {
    setDuplicateName(false)
    setIsAdding(false)
  }

  return (
    <>
      {renameState.isRenaming
        ? (
          <SpaceTitle isOpen={isOpen}>
            <RenameSpaceForm
              className={styles.menuSpaceRenameForm}
              value={renameState.spaceName}
              renameState={renameState}
              onRename={() => {
                if (duplicateName) {
                  return
                }
                ctx.updateScene(scene => renameSpace(scene, activeSpace.id, renameState.spaceName))
                renameState.setIsRenaming(false)
              }}
              onBlur={() => setDuplicateName(false)}
              onChange={value => handleSetSpaceName(value, activeSpace)}
              onEscapeKey={() => {
                setDuplicateName(false)
              }}
            />
          </SpaceTitle>
        )
        : (
          <SelectMenu
            id='spaces-menu'
            menuWrapperClassName={classes.studioMenu}
            trigger={(
              <SpaceTitle isOpen={isOpen} onContextMenu={menuState.handleContextMenu}>
                <div className={styles.spaceName}>
                  {activeSpace?.name ?? t('spaces_menu.title')}
                </div>
              </SpaceTitle>
            )}
            minTriggerWidth
            onOpenChange={(open) => {
              cancelAddSpace()
              setIsOpen(open)
            }}
            isOpen={isOpen}
          >
            {collapse => (
              <>
                {spaces.map(space => (
                  <SpaceItem
                    key={space.id}
                    space={space}
                    spaces={spaces}
                    onRename={newName => ctx.updateScene(
                      scene => renameSpace(scene, space.id, newName)
                    )}
                    active={space.id === activeSpace.id}
                    onClick={() => {
                      handleSelectSpace(space.id)
                      collapse()
                    }}
                    setDuplicateName={setDuplicateName}
                    duplicateName={duplicateName}
                  />
                ))}
                {isAdding && (
                  <form
                    onSubmit={(e) => {
                      e.preventDefault()
                      if (duplicateName) {
                        return
                      }
                      handleAddSpace()
                      collapse()
                    }}
                  >
                    <StandardTextField
                      id='create-new-space'
                      label={<SrOnly>{t('spaces_menu.input.space_name')}</SrOnly>}
                      height='tiny'
                      value={newSpaceName}
                      onChange={e => handleSetSpaceName(e.target.value)}
                      onBlur={() => {
                        cancelAddSpace()
                      }}
                      ref={element => element && element.focus()}
                      onKeyDown={(e) => {
                        if (e.key === Keys.ESCAPE) {
                          cancelAddSpace()
                          collapse()
                        }
                      }}
                    />
                  </form>
                )}
                {duplicateName &&
                  <StaticBanner
                    message={t('spaces_menu.warning.duplicate_space_name')}
                    type='warning'
                  />
              }
                {(spaces.length > 0 || isAdding) && <div className={classes.divider} />}
                <FloatingMenuButton onClick={startAddSpace}>
                  {t('spaces_menu.button.create_new_space')}
                </FloatingMenuButton>
              </>
            )}
          </SelectMenu>
        )}
      {!isOpen && (
        <ContextMenu
          menuState={menuState}
          options={activeSpaceContextMenuOptions}
        />
      )}
      {duplicateName && !isOpen && (
        <StaticBanner
          message={t('spaces_menu.warning.duplicate_space_name')}
          type='warning'
        />
      )}
    </>
  )
}

export {SpacesMenu}

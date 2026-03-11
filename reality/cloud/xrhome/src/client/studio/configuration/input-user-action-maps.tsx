import React from 'react'
import {IgnoreKeys} from 'react-hotkeys'
import {useTranslation} from 'react-i18next'
import {DEFAULT_ACTION_MAP} from '@ecs/shared/default-action-maps'

import {FloatingMenuButton} from '../../ui/components/floating-menu-button'
import {Icon} from '../../ui/components/icon'
import {CornerDropdownMenu} from '../ui/corner-dropdown-menu'
import {combine} from '../../common/styles'
import {createThemedStyles} from '../../ui/theme'
import InlineTextInput from '../../common/inline-text-input'
// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const DEFAULT_ACTION_MAP_LABEL = 'Default Action Map'

const useStyles = createThemedStyles(theme => ({
  actionMapButton: {
    'display': 'flex',
    'position': 'relative',
    '&:hover $checkmark': {
      display: 'none',
    },
    '&:hover $actionMapContextContainer': {
      visibility: 'visible',
    },
  },
  actionMapContextContainer: {
    position: 'absolute',
    right: 0,
    top: '0.25em',
    visibility: 'hidden',
  },
  checkmark: {
    position: 'absolute',
    right: '0.5em',
    top: '0.25em',
  },
  renamingContainer: {
    flex: 1,
  },
  renaming: {
    border: `1px solid ${theme.sfcBorderFocus}`,
    borderRadius: '4px',
    color: theme.fgMain,
    padding: '0 0.25em',
    width: '100%',
  },
}))

interface IInputUserActionMaps {
  currentMap: string
  actionMapList: string[]
  onAddMap: (name: string, sourceMap: string | null, isPreset: boolean) => void
  onDeleteMap: (name: string) => void
  onRenameMap: (oldName: string, newName: string) => void
  onChangeCurrentMap: (name: string) => void
  renamingMap: string | null
  setRenamingMap: (map: string | null) => void
  newActionMap: string
  setNewActionMap?: (name: string) => void
  collapse: () => void
}

const InputUserActionMaps: React.FC<IInputUserActionMaps> = ({
  currentMap, actionMapList, onAddMap, onDeleteMap, onRenameMap, onChangeCurrentMap, renamingMap,
  setRenamingMap, newActionMap, setNewActionMap, collapse,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()

  return (
    <>
      {
        actionMapList.map((name: string) => (
          <div key={name} className={classes.actionMapButton}>
            {(renamingMap !== name) &&
              <FloatingMenuButton
                onClick={(e) => {
                  e.stopPropagation()
                  onChangeCurrentMap(name)
                  collapse()
                }}
              >
                {name === DEFAULT_ACTION_MAP ? DEFAULT_ACTION_MAP_LABEL : name}
              </FloatingMenuButton>
            }
            {(renamingMap === name) &&
              <IgnoreKeys className={classes.renamingContainer}>
                <InlineTextInput
                  value={newActionMap}
                  onChange={e => setNewActionMap(e.target.value)}
                  onCancel={() => {
                    setNewActionMap('')
                    setRenamingMap(null)
                  }}
                  onSubmit={() => {
                    onRenameMap(renamingMap, newActionMap)
                    setNewActionMap('')
                  }}
                  inputClassName={combine('style-reset', classes.renaming)}
                  aria-label={t('input_user_action_maps.input.rename_action_map')}
                />
              </IgnoreKeys>
            }
            {(renamingMap !== name) &&
              <div className={classes.actionMapContextContainer}>
                <CornerDropdownMenu>
                  {collapse2 => (
                    <>
                      {name !== DEFAULT_ACTION_MAP &&
                        <FloatingMenuButton
                          onClick={(e) => {
                            e.stopPropagation()
                            collapse2()
                            setRenamingMap(name)
                            setNewActionMap(name)
                          }}
                        >
                          {t('button.rename', {ns: 'common'})}
                        </FloatingMenuButton>
                    }
                      <FloatingMenuButton
                        onClick={(e) => {
                          e.stopPropagation()
                          onAddMap(name, name, false)
                          collapse2()
                          collapse()
                        }}
                      >
                        {t('button.duplicate', {ns: 'common'})}
                      </FloatingMenuButton>
                      {name !== DEFAULT_ACTION_MAP &&
                        <FloatingMenuButton
                          onClick={(e) => {
                            e.stopPropagation()
                            onDeleteMap(name)
                            collapse2()
                          }}
                        >
                          {t('button.delete', {ns: 'common'})}
                        </FloatingMenuButton>
                    }
                    </>
                  )}
                </CornerDropdownMenu>
              </div>
            }
            {currentMap === name && renamingMap !== name &&
              <div
                className={classes.checkmark}
              >
                <Icon stroke='checkmark' color='highlight' />
              </div>
            }
          </div>
        ))
      }
    </>
  )
}

export {
  InputUserActionMaps,
}

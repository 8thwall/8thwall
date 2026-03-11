import React from 'react'
import {createUseStyles} from 'react-jss'
import type {DeepReadonly} from 'ts-essentials'

import type {InputMap} from '@ecs/shared/scene-graph'
import {DEFAULT_ACTION_MAPS, DEFAULT_ACTION_MAP} from '@ecs/shared/default-action-maps'

import {SelectMenu} from '../ui/select-menu'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {useSceneContext, type MutateCallback} from '../scene-context'
import {combine} from '../../common/styles'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {useStyles as useRowFieldStyles} from './row-fields'
import {InputUserActionMaps} from './input-user-action-maps'
import {InputPresetActionMaps} from './input-preset-action-maps'
import {InputCreateActionMapForm} from './input-create-action-map-form'

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const DEFAULT_ACTION_MAP_LABEL = 'Default Action Map'

const useStyles = createUseStyles({
  selectMapMenu: {
    width: '100%',
    padding: '0',
    gap: '0',
  },
  selectMapMenuButton: {
    fontSize: '12px',
  },
  currentActionMap: {
    whiteSpace: 'nowrap',
    overflow: 'hidden',
    textOverflow: 'ellipsis',
  },
  userMapsContainer: {
    padding: '0.5em 0.5em 0 0.5em',
    display: 'flex',
    flexDirection: 'column',
    gap: '0.25em',
  },
})

const suggestNewName = (name: string, actionMap: DeepReadonly<InputMap>): string => {
  if (actionMap[name]) {
    const parts = name.split(' ')
    const suffix = parts[parts.length - 1]
    if (Number.isNaN(parseInt(suffix, 10))) {
      return `${name} 1`
    }
    return `${name.substring(0, name.length - suffix.length)} ${Number(suffix) + 1}`
  }
  return name
}

const getInputMap = (inputs: DeepReadonly<InputMap>) => (
  (inputs && Object.keys(inputs).length !== 0) ? inputs : {[DEFAULT_ACTION_MAP]: []})

interface IInputActionMapDropdown {
  currentMap: string
  setCurrentMap: (map: string) => void
  onUpdateSceneInputMap: (cb: MutateCallback<InputMap>) => void
}

const InputActionMapDropdown: React.FC<IInputActionMapDropdown> = ({
  currentMap, setCurrentMap, onUpdateSceneInputMap,
}) => {
  const classes = useStyles()
  const studioClasses = useStudioMenuStyles()
  const rowFieldStyles = useRowFieldStyles()
  const ctx = useSceneContext()
  const actionMap = getInputMap(ctx.scene.inputs)
  const [renamingMap, setRenamingMap] = React.useState<string | null>(null)
  const [newActionMap, setNewActionMap] = React.useState<string>('')
  const [isCreatingNewMap, setCreatingNewMap] = React.useState<boolean>(false)
  const [isPresetsOpen, setPresetsOpen] = React.useState<boolean>(false)
  const [presetMap, setPresetMap] = React.useState<string | null>(null)

  const handleAddMap = (name: string, sourceMap: string | null, isPreset: boolean) => {
    let newName = name
    if (actionMap[name] || name === DEFAULT_ACTION_MAP_LABEL) {
      newName = suggestNewName(name, actionMap)
    }

    onUpdateSceneInputMap((oldInputs) => {
      const newInputs = {...oldInputs}
      if (sourceMap) {
        newInputs[newName] = isPreset ? DEFAULT_ACTION_MAPS[sourceMap] : [...newInputs[sourceMap]]
      } else {
        newInputs[newName] = []
      }
      return newInputs
    })
    setCurrentMap(newName)
    setPresetMap(null)
  }

  const handleDeleteMap = (name: string) => {
    if (!actionMap[name]) {
      throw new Error(`Action map ${name} does not exist`)
    } else if (currentMap === name) {
      setCurrentMap(DEFAULT_ACTION_MAP)
    }

    onUpdateSceneInputMap((oldInputs) => {
      const newInputs = {...oldInputs}
      delete newInputs[name]
      return newInputs
    })
  }

  const handleRenameMap = (oldName: string, newName: string) => {
    if (actionMap[oldName] && !actionMap[newName]) {
      onUpdateSceneInputMap((oldInputs) => {
        const newInputs = {...oldInputs}
        newInputs[newName] = [...newInputs[oldName]]
        delete newInputs[oldName]
        return newInputs
      })
      setCurrentMap(newName)
    } else {
      throw new Error(`Action map ${oldName} does not exist or ${newName} already exists`)
    }
  }

  const handleChangeCurrentMap = (name: string) => {
    if (actionMap[name]) {
      setCurrentMap(name)
    } else {
      throw new Error(`Action map ${name} does not exist`)
    }
  }

  return (
    <StandardFieldContainer>
      <SelectMenu
        id='action-map-select-menu'
        trigger={(
          <button
            className={combine('style-reset', rowFieldStyles.select, classes.selectMapMenuButton)}
            type='button'
          >
            <div className={classes.currentActionMap}>
              {currentMap === DEFAULT_ACTION_MAP ? DEFAULT_ACTION_MAP_LABEL : currentMap}
            </div>
            <div className={rowFieldStyles.chevron} />
          </button>
        )}
        menuWrapperClassName={combine(studioClasses.studioMenu, classes.selectMapMenu)}
        onOpenChange={(open) => {
          if (!open) {
            setRenamingMap(null)
            setPresetsOpen(false)
            setPresetMap(null)
          }
        }}
        placement='bottom-end'
        margin={5}
        matchTriggerWidth
      >
        {collapse => (
          <>
            {!isPresetsOpen &&
              <div className={classes.userMapsContainer}>
                <InputUserActionMaps
                  currentMap={currentMap}
                  actionMapList={Object.keys(actionMap)}
                  onAddMap={handleAddMap}
                  onDeleteMap={handleDeleteMap}
                  onRenameMap={handleRenameMap}
                  onChangeCurrentMap={handleChangeCurrentMap}
                  renamingMap={renamingMap}
                  setRenamingMap={setRenamingMap}
                  newActionMap={newActionMap}
                  setNewActionMap={setNewActionMap}
                  collapse={collapse}
                />
                {isCreatingNewMap &&
                  <InputCreateActionMapForm
                    onAddMap={handleAddMap}
                    setCreatingNewMap={setCreatingNewMap}
                    newActionMap={newActionMap}
                    setNewActionMap={setNewActionMap}
                    presetMap={presetMap}
                    collapse={collapse}
                  />
                }
                <div className={studioClasses.divider} />
              </div>
            }
            <InputPresetActionMaps
              setPresetsOpen={setPresetsOpen}
              setNewActionMap={setNewActionMap}
              setCreatingNewMap={setCreatingNewMap}
              setPresetMap={setPresetMap}
            />
          </>
        )}
      </SelectMenu>
    </StandardFieldContainer>
  )
}

export {
  InputActionMapDropdown,
  getInputMap,
}

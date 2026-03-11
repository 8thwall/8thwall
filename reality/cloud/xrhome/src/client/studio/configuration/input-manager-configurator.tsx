import React from 'react'
import type {InputMap} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'
import {DEFAULT_ACTION_MAP} from '@ecs/shared/default-action-maps'

import {MutateCallback, useSceneContext} from '../scene-context'
import {ActionConfigurator} from './action-configurator'
import {StaticBanner} from '../../ui/components/banner'
import {createThemedStyles} from '../../ui/theme'
import {InputNewActionButton} from './input-new-action-button'
import {InputActionMapDropdown} from './input-action-map-dropdown'

const useStyles = createThemedStyles(theme => ({
  selectMapMenuContainer: {
    alignContent: 'center',
    justifyContent: 'center',
    padding: '.75em',
  },
  actionConfigurator: {
    padding: '1em 0',
  },
  saveContainer: {
    display: 'flex',
    justifyContent: 'space-between',
    marginTop: '1em',
  },
  noActions: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    minHeight: '8em',
    fontSize: '12px',
    color: theme.fgMuted,
  },
}))

const getInputMap = (inputs: DeepReadonly<InputMap>) => (
  (inputs && Object.keys(inputs).length !== 0) ? inputs : {[DEFAULT_ACTION_MAP]: []})

const InputManagerConfigurator = () => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const ctx = useSceneContext()
  const classes = useStyles()
  const currentMap = ctx.scene.activeMap || DEFAULT_ACTION_MAP
  const actionMap = getInputMap(ctx.scene.inputs)

  const handleChangeCurrentMap = (newMap: string) => {
    ctx.updateScene(scene => ({
      ...scene,
      activeMap: newMap,
    }))
  }

  const updateSceneInputMap = (cb: MutateCallback<InputMap>) => {
    ctx.updateScene((scene) => {
      const oldInputs = getInputMap(scene.inputs)
      const newInputs = cb(oldInputs)
      return {
        ...scene,
        inputs: newInputs,
      }
    })
  }

  const handleNewAction = (actionName: string) => {
    if (!actionName || actionMap[currentMap].find(action => action.name === actionName)) {
      return
    }
    updateSceneInputMap((oldInputs) => {
      const newInputs = {...oldInputs}
      newInputs[currentMap] = [...newInputs[currentMap], {name: actionName, bindings: []}]
      return newInputs
    })
  }

  const handleDeleteAction = (actionName: string) => {
    updateSceneInputMap((oldInputs) => {
      const newInputs = {...oldInputs}
      newInputs[currentMap] = newInputs[currentMap].filter(action => action.name !== actionName)
      return newInputs
    })
  }

  const handleAddBinding = (actionName: string, input: string, modifiers: string[]) => {
    updateSceneInputMap((oldInputs) => {
      const {name, bindings} = oldInputs[currentMap].find(action => action.name === actionName)
      const newBindings = [...bindings, {input, modifiers: [...modifiers]}]
      const newAction = {name, bindings: newBindings}
      const newMap = [
        ...oldInputs[currentMap].filter(action => action.name !== actionName), newAction,
      ]
      return {...oldInputs, [currentMap]: newMap}
    })
  }

  const handleChangeBinding = (
    actionName: string, input: string, index: number, modifiers: string[]
  ) => {
    updateSceneInputMap((oldInputs) => {
      const {name, bindings} = oldInputs[currentMap].find(action => action.name === actionName)
      const newBindings = [...bindings]
      newBindings[index] = {input, modifiers: [...modifiers]}
      const newMap = [
        ...oldInputs[currentMap].filter(action => action.name !== actionName),
        {name, bindings: newBindings},
      ]
      return {...oldInputs, [currentMap]: newMap}
    })
  }

  const handleDeleteBinding = (actionName: string, index: number) => {
    updateSceneInputMap((oldInputs) => {
      const {name, bindings} = oldInputs[currentMap].find(action => action.name === actionName)
      const newBindings = bindings.filter((_, currIndex) => currIndex !== index)
      const newMap = [
        ...oldInputs[currentMap].filter(action => action.name !== actionName),
        {name, bindings: newBindings},
      ]
      return {...oldInputs, [currentMap]: newMap}
    })
  }

  return (
    <div className={classes.selectMapMenuContainer}>
      <InputActionMapDropdown
        currentMap={currentMap}
        setCurrentMap={handleChangeCurrentMap}
        onUpdateSceneInputMap={updateSceneInputMap}
      />
      {!actionMap[currentMap]
        ? <StaticBanner
            message={t('input_manager_configurator.error.invalid_action_map')}
            type='danger'
        />
        : [...actionMap[currentMap]].sort((a, b) => a.name.localeCompare(b.name)).map(action => (
          <div className={classes.actionConfigurator} key={action.name}>
            <ActionConfigurator
              action={action}
              onDeleteAction={handleDeleteAction}
              onAddBinding={handleAddBinding}
              onChangeBinding={handleChangeBinding}
              onDeleteBinding={handleDeleteBinding}
            />
          </div>
        ))}
      {actionMap[currentMap]?.length === 0 &&
        <div className={classes.noActions}>
          {t('input_manager_configurator.no_actions.label')}
        </div>
      }
      <InputNewActionButton onNewAction={handleNewAction} />
    </div>
  )
}

export {
  InputManagerConfigurator,
}

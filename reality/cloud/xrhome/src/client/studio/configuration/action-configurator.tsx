import React from 'react'
import type {Action, Binding} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'

import {FloatingMenuButton} from '../../ui/components/floating-menu-button'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {CornerDropdownMenu} from '../ui/corner-dropdown-menu'
import {BindingConfigurator} from './binding-configurator'
import {gray4} from '../../static/styles/settings'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  floatingMenu: {
    maxHeight: '200px',
  },
  mainContainer: {
    border: theme.publishModalTableBorder,
    borderRadius: '0.5em',
    display: 'flex',
    flexDirection: 'column',
    gap: '1em',
    padding: '0.5em',
  },
  actionHeader: {
    display: 'flex',
    color: theme.fgMuted,
    alignContent: 'center',
    justifyContent: 'space-between',
    fontSize: '12px',
  },
  addContainer: {
    display: 'flex',
    justifyContent: 'right',
    alignItems: 'center',
    gap: '1em',
  },
  noBindings: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    color: theme.fgMuted,
    fontSize: '12px',
    minHeight: '2em',
  },
  bindingList: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5em',
  },
}))

interface IActionConfigurator {
  action: DeepReadonly<Action>
  onDeleteAction: (actionName: string) => void
  onAddBinding: (actionName: string, input: string, modifiers: string[]) => void
  onChangeBinding: (actionName: string, input: string, index: number, modifiers: string[]) => void
  onDeleteBinding: (actionName: string, index: Number) => void
}

const ActionConfigurator: React.FC<IActionConfigurator> = (
  {action, onAddBinding, onDeleteAction, onChangeBinding, onDeleteBinding}
) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()

  return (
    <div className={classes.mainContainer}>
      <div className={classes.actionHeader}>
        <span color={gray4}> {action.name} </span>
        <CornerDropdownMenu>
          {collapse2 => (
            <FloatingMenuButton
              onClick={(e) => {
                e.stopPropagation()
                onDeleteAction(action.name)
                collapse2()
              }}
            >
              {t('button.delete', {ns: 'common'})}
            </FloatingMenuButton>
          )}
        </CornerDropdownMenu>
      </div>
      <div className={classes.bindingList}>
        {action.bindings.length === 0 &&
          <div className={classes.noBindings}>
            {t('action_configurator.no_bindings.label')}
          </div>
        }
        {action.bindings.map((binding: Binding, bindingIndex: number) => {
          const id = binding.input + bindingIndex
          return (
            <BindingConfigurator
              key={id}
              binding={binding}
              onChangeBinding={(newBinding) => {
                onChangeBinding(action.name, newBinding, bindingIndex, binding.modifiers)
              }}
              onChangeModifier={
                (mods) => { onChangeBinding(action.name, binding.input, bindingIndex, mods) }
              }
              onDelete={() => onDeleteBinding(action.name, bindingIndex)}
            />
          )
        })}

      </div>
      <div className={classes.addContainer}>
        <FloatingPanelButton
          spacing='normal'
          height='tiny'
          onClick={(e) => {
            e.stopPropagation()
            onAddBinding(action.name, 'none', [])
          }}
        >
          {t('action_configurator.button.add_binding')}
        </FloatingPanelButton>
        <FloatingPanelButton
          spacing='normal'
          height='tiny'
          onClick={(e) => {
            e.stopPropagation()
            onAddBinding(action.name, 'none', ['empty-modifier'])
          }}
        >
          {t('action_configurator.button.add_with_modifier')}
        </FloatingPanelButton>
      </div>
    </div>
  )
}

export {
  ActionConfigurator,
}

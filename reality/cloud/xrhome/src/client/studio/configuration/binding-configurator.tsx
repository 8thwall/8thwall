import React from 'react'
import type {Binding} from '@ecs/shared/scene-graph'
import {createUseStyles} from 'react-jss'

import {useTranslation} from 'react-i18next'

import {Icon} from '../../ui/components/icon'
import {IconButton} from '../../ui/components/icon-button'
import {BindingConfiguratorMenu} from './binding-configurator-menu'

const useStyles = createUseStyles({
  bindingContainer: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  modifierContainer: {
    display: 'flex',
    justifyContent: 'left',
    alignItems: 'center',
    paddingLeft: '20px',
  },
})

interface IBindingConfigurator {
  binding: Binding
  onChangeBinding: (newBinding: string) => void
  onChangeModifier: (modifier: string[]) => void
  onDelete: () => void
}

const BindingConfigurator: React.FC<IBindingConfigurator> = (
  {binding, onChangeModifier, onChangeBinding, onDelete}
) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-studio-pages'])
  const hasModifier = binding.modifiers.length > 0

  return (
    <>
      <div className={classes.bindingContainer}>
        <BindingConfiguratorMenu
          name={hasModifier ? binding.modifiers[0] : binding.input}
          onChange={newBinding => (
            hasModifier ? onChangeModifier([newBinding]) : onChangeBinding(newBinding)
          )}
          placeholder={hasModifier
            ? t('binding_configurator.placeholder.select_modifier_input')
            : t('binding_configurator.placeholder.select_binding')}
        />
        <IconButton
          onClick={(e) => {
            e.stopPropagation()
            onDelete()
          }}
          text=''
          stroke='delete12'
        />
      </div>

      {hasModifier &&
        <div className={classes.modifierContainer}>
          <Icon stroke='arrowDownRight' />
          <BindingConfiguratorMenu
            name={binding.input}
            onChange={newBinding => onChangeBinding(newBinding)}
            placeholder={t('binding_configurator.placeholder.select_binding_input')}
          />
        </div>
      }
    </>
  )
}

export {
  BindingConfigurator,
}

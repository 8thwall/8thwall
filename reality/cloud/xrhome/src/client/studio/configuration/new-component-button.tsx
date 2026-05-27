import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import type {GraphObject} from '@ecs/shared/scene-graph'

import {
  useAvailableComponents, Component, MESH_COMPONENT, LIGHT_COMPONENT, COLLIDER_COMPONENT,
  PARTICLE_EMITTER_COMPONENT, UI_COMPONENT,
} from '../hooks/available-components'
import {Icon} from '../../ui/components/icon'
import {SubMenuSelectWithSearch} from '../ui/submenu-select-with-search'
import {makeMaterial, makeParticles} from '../make-object'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {setSectionCollapsed} from '../hooks/collapsed-section'
import {useStudioStateContext} from '../studio-state-context'
import {MutateCallback, useSceneContext} from '../scene-context'
import {useSelectedObjects} from '../hooks/selected-objects'
import {useDerivedScene} from '../derived-scene-context'
import {createIdSeed} from '../id-generation'
import {CreateComponentOption} from './create-component-option'

const useStyles = createUseStyles({
  addComponent: {
    padding: '1rem',
    fontSize: '12px',
  },
  button: {
    display: 'flex',
    gap: '1em',
    alignItems: 'center',
    justifyContent: 'space-between',
    overflow: 'hidden',
    whiteSpace: 'nowrap',
    padding: '0.325em 0.5em',
  },
})

interface INewComponentButton {
}

const NewComponentButton: React.FC<INewComponentButton> = () => {
  const classes = useStyles()
  const {t} = useTranslation('cloud-studio-pages')
  const stateCtx = useStudioStateContext()
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()

  const objects = useSelectedObjects()

  const handleAddComponent = (value: string, isDirectProperty: boolean) => {
    let sectionId = value
    const idSeed = createIdSeed()

    objects.forEach((object) => {
      const onChange = (u: MutateCallback<GraphObject>) => ctx.updateObject(object.id, u)
      switch (value) {
        case MESH_COMPONENT:
          onChange(o => ({
            ...o,
            material: o.material || makeMaterial(),
          }))
          break
        case LIGHT_COMPONENT:
          onChange(o => ({...o, light: {type: 'directional'}}))
          break
        case COLLIDER_COMPONENT:
          // TODO(christoph): Technically if the object doesn't have a valid collider shape
          // as its geometry, we should default to something else
          onChange(o => ({...o, collider: {geometry: {type: 'auto'}}}))
          break
        case UI_COMPONENT:
          onChange(o => ({...o, ui: {type: '3d'}}))
          break
        default:
          if (isDirectProperty) {
            onChange(o => ({...o, [value]: {}}))
          } else {
            const id = idSeed.fromId(object.id)
            sectionId = id

            let parameters = {}
            switch (value) {
              case PARTICLE_EMITTER_COMPONENT:
                parameters = makeParticles()
                break
              default:
                break
            }

            onChange(o => ({
              ...o,
              components: {
                ...o.components,
                [id]: {
                  id,
                  name: value,
                  parameters,
                },
              },
            }))
          }
      }

      setSectionCollapsed(stateCtx, object.id, sectionId, false)
    })
  }

  // need to make this use the available components across all objects, using first one for now
  const sortedComponents = useAvailableComponents(
    derivedScene.getObject(objects[0].id)?.id,
    o => <CreateComponentOption {...o} onCreate={name => handleAddComponent(name, false)} />
  )

  const handleSelectOption = (option: string) => {
    const component = sortedComponents.flatMap(e => e.options).find(c => c.value === option)
    if (component) {
      handleAddComponent(
        component.value,
        (component as Component).isDirectProperty
      )
    }
  }

  return (
    <div className={classes.addComponent}>
      <SubMenuSelectWithSearch
        a8='click;studio;new-component-search-click'
        trigger={(
          <FloatingPanelButton
            a8='click;studio;new-component-button'
            spacing='full'
          >
            <Icon stroke='plus' inline />
            {t('new_component_button.button.add_component')}
          </FloatingPanelButton>
        )}
        onChange={handleSelectOption}
        categories={sortedComponents}
      />
    </div>
  )
}

export {
  NewComponentButton,
}

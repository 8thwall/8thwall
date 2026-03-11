import React from 'react'
import {v4 as uuid} from 'uuid'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

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
import {useSceneContext} from '../scene-context'
import {useSelectedObjects} from '../hooks/selected-objects'
import {useDerivedScene} from '../derived-scene-context'

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
  // need to make this use the available components across all objects, using first one for now
  const sortedComponents = useAvailableComponents(
    derivedScene.getObject(objects[0].id)?.id
  )
  const flatComponents = sortedComponents.reduce((acc, section) => acc.concat(section.options), [])

  const handleAddComponent = (component: Component) => {
    const {value} = component
    const stringValue = value as string
    let sectionId = stringValue

    objects.forEach((object) => {
      const onChange = u => ctx.updateObject(object.id, u)
      switch (stringValue) {
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
          if (component.isDirectProperty) {
            onChange(o => ({...o, [value]: {}}))
          } else {
            const id = uuid()
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
  const handleSelectOption = (option: string) => {
    const component = flatComponents.find(c => c.value === option)
    handleAddComponent(component)
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

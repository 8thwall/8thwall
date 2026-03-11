import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {createUseStyles} from 'react-jss'
import type {GraphObject} from '@ecs/shared/scene-graph'
import {useTranslation} from 'react-i18next'

import {SchemaConfigurator} from './schema-configurator'
import {useComponentMetadata} from '../hooks/component-schema'
import {COMPONENT_DESCRIPTION} from './component-description'
import {useFileActionsContext} from '../../editor/files/file-actions-context'
import {useStudioComponentsContext} from '../studio-components-context'
import {RowTextField} from './row-fields'
import {useStudioStateContext} from '../studio-state-context'
import {copyComponent} from './copy-component'
import {ComponentConfiguratorTray} from './component-configurator-tray'
import {ALL_NEW_COMPONENT_OPTIONS} from './new-component-strings'

type Components = DeepReadonly<GraphObject['components']>
type Parameters = Components[string]['parameters']

const NO_INSPECT_COMPONENTS = new Set(['location-anchor'])
const NO_REMOVE_COMPONENTS = new Set(['face-attachment'])
const CUSTOM_COMPONENT_CONFIG = new Set(['face-attachment'])

const useStyles = createUseStyles({
  textArea: {
    width: '100%',
    height: '500px',
    maxHeight: '50vh',
  },
})

interface IComponentConfigurators {
  components: Components
  onChange: (updater: (current: Components) => Components) => void
  resetToPrefab: (componentIds: string[], nonDirect: boolean) => void
  applyOverridesToPrefab: (componentIds: string[], nonDirect: boolean) => void
}

interface IRawJsonEditor {
  values: Parameters
  onChange: (newValues: Parameters) => void
}

const RawJsonEditor: React.FC<IRawJsonEditor> = ({values, onChange}) => {
  const [pendingJson, setPendingJson] = React.useState(JSON.stringify(values, null, 2))
  const id = React.useId()
  const classes = useStyles()
  const {t} = useTranslation('cloud-studio-pages')

  React.useLayoutEffect(() => {
    setPendingJson(JSON.stringify(values, null, 2))
  }, [values])

  const handleChange = (e: React.ChangeEvent<HTMLTextAreaElement>) => {
    const newValue = e.target.value
    setPendingJson(newValue)
    try {
      onChange(JSON.parse(newValue))
    } catch (err) {
      // ignore
    }
  }

  return (
    <label htmlFor={id}>
      {t('component_configurator.raw_json_editor.parameters.label')}
      <textarea className={classes.textArea} id={id} value={pendingJson} onChange={handleChange} />
    </label>
  )
}

interface IComponentParameterEditor {
  name: string
  values: Parameters
  onChange: (callback: (oldValues: Parameters) => Parameters) => void
}

const ComponentParameterEditor: React.FC<IComponentParameterEditor> = ({
  name, values, onChange,
}) => {
  const metadata = useComponentMetadata(name)
  const {t} = useTranslation('cloud-studio-pages')

  if (metadata) {
    return (
      <SchemaConfigurator
        metadata={metadata}
        values={values as any || {}}
        onChange={onChange as any}
      />
    )
  } else {
    return (
      <>
        <b>{t('component_configurator.error.invalid_component')}</b> <br />
        <RawJsonEditor values={values} onChange={p => onChange(() => p)} />
      </>
    )
  }
}

interface IXRAttachmentConfigurator {
  attachment: string
}

const XrAttachmentConfigurator: React.FC<IXRAttachmentConfigurator> = (
  {attachment}
) => {
  const {t} = useTranslation('cloud-studio-pages')

  return (
    <RowTextField
      id='attachmentId'
      label={t('component_configurator.attachment_id.label')}
      value={attachment}
      disabled
      onChange={() => {}}
    />
  )
}

const ComponentConfigurators: React.FC<IComponentConfigurators> = ({
  components, onChange, resetToPrefab, applyOverridesToPrefab,
}) => {
  const componentList = components && Object.values(components)
  const actionsContext = useFileActionsContext()
  const {getComponentLocation, getComponentSchema} = useStudioComponentsContext()
  const stateCtx = useStudioStateContext()
  const {t} = useTranslation('cloud-studio-pages')

  return (
    <>
      {componentList.filter(component => !NO_INSPECT_COMPONENTS.has(component.name))
        .map((component) => {
          const {name} = component
          const fileLocation = getComponentLocation(name)
          const schema = getComponentSchema(name)
          const scrollTo = {
            line: schema?.location?.startLine,
            column: schema?.location?.startColumn,
          }

          const getComponentTitle = () => {
            const foundOption = ALL_NEW_COMPONENT_OPTIONS.find(option => option.value === name)
            if (!foundOption) {
              return name
            }
            return foundOption.ns
              ? t(foundOption.content, {ns: foundOption.ns})
              : foundOption.content
          }

          return (
            <ComponentConfiguratorTray
              key={component.id}
              title={getComponentTitle()}
              description={COMPONENT_DESCRIPTION[name] && t(COMPONENT_DESCRIPTION[name])}
              onEdit={fileLocation &&
                (() => actionsContext.onSelect(fileLocation, scrollTo))}
              onRemove={!(NO_REMOVE_COMPONENTS.has(name)) && (
                () => onChange((current) => {
                  const newComponents = {...current}
                  delete newComponents[component.id]
                  return newComponents
                })
              )}
              onCopy={!(NO_REMOVE_COMPONENTS.has(name)) && (
                () => copyComponent(stateCtx, component)
              )}
              onResetToPrefab={() => resetToPrefab([component.id], true)}
              onApplyOverridesToPrefab={() => applyOverridesToPrefab([component.id], true)}
              sectionId={component.id}
              componentData={[{type: 'nonDirect', name}]}
            >
              {name === 'face-attachment' &&
                <XrAttachmentConfigurator
                  attachment={component.parameters.point as string}
                  // TODO (lynn): add this back when we enable attachment point selection.
                  // options={faceAttachmentNames
                  //   .filter(attachmentName => (isEarEnabled ||
                  //   !earAttachmentNames.includes(attachmentName)))
                  //   .map(attachmentName => ({
                  //     value: attachmentName,
                  //     content: attachmentName,
                  //   }))}
                />
              }

              {!CUSTOM_COMPONENT_CONFIG.has(name) &&
                <ComponentParameterEditor
                  name={name}
                  values={component.parameters}
                  onChange={callback => onChange((current) => {
                    const currentComponent = current[component.id]
                    if (!currentComponent) {
                      return current
                    }
                    return {
                      ...current,
                      [component.id]: {
                        ...currentComponent,
                        parameters: callback(currentComponent.parameters),
                      },
                    }
                  })}
                />
              }
            </ComponentConfiguratorTray>
          )
        })}
    </>
  )
}

export {
  ComponentConfigurators,
}

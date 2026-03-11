import React from 'react'
import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'
import type {FieldPresentation, PropertyType, ReadData, Schema, Type} from '@ecs/shared/schema'
import type {EntityReference, GraphObject} from '@ecs/shared/scene-graph'
import type {StudioComponentMetadata} from '@ecs/shared/studio-component'

import {extractResourceUrl} from '@ecs/shared/resource'

import {createUseStyles} from 'react-jss'

import {useSceneContext} from '../scene-context'
import {displayNameForObject} from '../display'
import {deriveFieldLabel} from '../../editor/module-config/derive-field-label'
import {
  RowNumberField, RowSelectField, RowBooleanField, RowTextField, useStyles as useRowStyles,
  RowColorField,
} from './row-fields'
import {AttributeField, PropertyField} from './attribute-property-fields'
import {DropTarget} from '../ui/drop-target'
import {buildDisplayDataTree, FieldDisplayData} from './schema-presentation'
import {GroupField} from './group-field'
import {AssetSelector} from '../ui/asset-selector'
import {CollapsibleSection} from './collapsible-section'
import {useActiveSpace} from '../hooks/active-space'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {combine} from '../../common/styles'
import {SubMenuSelectWithSearch} from '../ui/submenu-select-with-search'
import {useSelectedObjects} from '../hooks/selected-objects'
import {TextNotification} from '../../ui/components/text-notification'
import {useDerivedScene} from '../derived-scene-context'

const useStyles = createUseStyles({
  selectContainer: {
    flex: 1,
    fontSize: '12px',
  },
})

interface FieldProps<T> {
  label: React.ReactNode
  // Type is unknown here as it can change if this field is used on a custom component
  value: unknown
  onChange: (newValue: T) => void
}

const NumberField: React.FC<FieldProps<number> & {min?: number, max?: number}> = ({
  label, value, onChange, min, max,
}) => {
  const id = React.useId()

  return (
    <RowNumberField
      id={id}
      onChange={onChange}
      value={Number(value) ?? 0}
      step={1}
      label={label}
      min={min}
      max={max}
    />
  )
}

const StringField: React.FC<FieldProps<string>> = ({label, value, onChange}) => {
  const id = React.useId()
  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    onChange(e.target.value)
  }

  return (
    <RowTextField
      id={id}
      onChange={handleChange}
      value={String(value ?? '')}
      label={label}
    />
  )
}

const SelectField: React.FC<FieldProps<string> & {options: readonly string[]}> = ({
  label, value, options, onChange,
}) => {
  const id = React.useId()

  return (
    <RowSelectField
      id={id}
      options={options.map(option => ({value: option, content: option}))}
      onChange={onChange}
      value={String(value ?? '')}
      label={label}
    />
  )
}

const BooleanField: React.FC<FieldProps<boolean>> = ({label, value, onChange}) => {
  const id = React.useId()
  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    onChange(e.target.checked)
  }

  return (
    <RowBooleanField
      id={id}
      onChange={handleChange}
      checked={!!value}
      label={label}
    />
  )
}

const StringColorField: React.FC<FieldProps<string>> = ({label, value, onChange}) => {
  const id = React.useId()

  return (
    <RowColorField
      id={id}
      onChange={onChange}
      value={String(value)}
      label={label}
    />
  )
}

interface IEidFieldEditor {
  label: string
  value: EntityReference | null
  onChange: (newValue: EntityReference) => void
  required: boolean | undefined
}

// Create the component for selecting an entity from the scene.
const EidFieldEditor: React.FC<IEidFieldEditor> = ({label, value, onChange, required}) => {
  const classes = useStyles()
  const rowClasses = useRowStyles()
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const {t} = useTranslation('cloud-studio-pages')
  const activeSpaceId = useActiveSpace()?.id
  const selectedObjects = useSelectedObjects()
  const currentObjectId = selectedObjects.length && selectedObjects[0].id

  const selectedId = derivedScene.resolveObjectReference(currentObjectId, value?.id)
  const valueObject = derivedScene.getObject(selectedId)

  const prefabId = derivedScene.getParentPrefabId(currentObjectId)

  const handleChange = (newValue: string) => {
    onChange(newValue === '' ? undefined : {type: 'entity', id: newValue})
  }

  const handleDrop = (e: React.DragEvent) => {
    const objectId = e.dataTransfer.getData('objectId')
    const isFound = !!derivedScene.getObject(objectId)
    if (objectId && isFound) {
      onChange({type: 'entity', id: objectId})
    }
  }

  const currentPrefabOptions = () => [
    {
      parent: null,
      value: null,
      options: [
        {value: '', content: t('schema_configurator.option.none')},
        {
          value: prefabId,
          content: displayNameForObject(derivedScene.getObject(prefabId)),
        },
        ...derivedScene.getDescendantObjectIds(prefabId).map((id) => {
          const object = derivedScene.getObject(id)
          return {
            value: id,
            content: displayNameForObject(object),
          }
        })],
    },
  ]

  const getCategory = () => {
    if (prefabId) {
      return currentPrefabOptions()
    }

    const allIncludedSpaces = derivedScene.getAllIncludedSpaces(activeSpaceId)

    const getOptionsForObjects = (objects: DeepReadonly<GraphObject>[]) => [
      {value: '', content: t('schema_configurator.option.none')},
      ...objects.map(o => ({value: o?.id, content: displayNameForObject(o)})),
    ]

    const getSpaceObjectOptions = (spaceId: string) => {
      const spaceObjects = derivedScene.getSpaceObjects(spaceId)
      return getOptionsForObjects(spaceObjects)
    }

    const getPrefabIds = () => {
      const prefabRoots = derivedScene.getPrefabs()
      const prefabObjectIds: string[] = []
      prefabRoots.forEach((p) => {
        prefabObjectIds.push(p.id, ...derivedScene.getDescendantObjectIds(p.id))
      })
      return prefabObjectIds
    }

    const prefabOptions = {
      parent: null,
      value: t('schema_configurator.category.prefabs'),
      options: getOptionsForObjects(getPrefabIds().map(
        id => derivedScene.getObject(id)
      )),
    }

    if (allIncludedSpaces.length < 2) {
      const objects = activeSpaceId
        ? derivedScene.getSpaceObjects(activeSpaceId)
        : Object.values(ctx.scene.objects)
      return [
        {
          parent: null,
          value: null,
          options: getOptionsForObjects(objects),
        },
        prefabOptions,
      ]
    } else {
      return [
        ...allIncludedSpaces.map(spaceId => ({
          parent: null,
          value: ctx.scene.spaces[spaceId].name,
          options: getSpaceObjectOptions(spaceId),
        })),
        prefabOptions,
      ]
    }
  }

  return (
    <DropTarget onDrop={handleDrop}>
      <div className={rowClasses.row}>
        <div className={rowClasses.flexItem}>
          <StandardFieldLabel
            label={label}
            mutedColor
            starred={required}
          />
        </div>
        <div className={classes.selectContainer}>
          <SubMenuSelectWithSearch
            onChange={handleChange}
            categories={getCategory()}
            trigger={(
              <button
                className={combine('style-reset', rowClasses.select,
                  rowClasses.preventOverflow)}
                type='button'
              >
                <div className={rowClasses.selectText}>
                  {(valueObject && displayNameForObject(valueObject)) ||
                    t('schema_configurator.option.none')}
                </div>
                <div className={rowClasses.chevron} />
              </button>
            )}
          />
        </div>
      </div>
      {required && !valueObject &&
        <TextNotification type='warning'>
          {t('schema_configurator.eid_select.required_warning', {label})}
        </TextNotification>}
    </DropTarget>
  )
}

interface IField {
  label: string
  type: Type
  value: any
  defaultValue?: any
  presentation: DeepReadonly<FieldPresentation> | undefined
  onChange: (newValue: any) => void
}

const Field: React.FC<IField> = ({label, type, value, defaultValue, presentation, onChange}) => {
  const {t} = useTranslation('cloud-studio-pages')
  switch (type) {
    case 'f32':
    case 'i32':
    case 'f64':
    case 'ui32':
    case 'ui8':
      return (
        <NumberField
          label={label}
          value={value ?? defaultValue ?? 0}
          onChange={onChange}
          min={presentation?.min}
          max={presentation?.max}
        />
      )
    case 'string':
      if (presentation?.enumValues) {
        return (
          <SelectField
            label={label}
            value={value ?? defaultValue}
            options={presentation.enumValues}
            onChange={onChange}
          />
        )
      } else if (presentation?.mode === 'asset') {
        // TODO(christoph): Follow up after asset manifest
        return (
          <AssetSelector
            label={label}
            assetKind={presentation.assetKind}
            resource={value ?? defaultValue}
            onChange={
              newResource => onChange(newResource ? extractResourceUrl(newResource) : undefined)
            }
          />
        )
      } else if (presentation?.mode === 'color') {
        return (
          <StringColorField
            label={label}
            value={value ?? defaultValue ?? ''}
            onChange={onChange}
          />
        )
      }
      return <StringField label={label} value={value ?? defaultValue} onChange={onChange} />
    case 'boolean':
      return <BooleanField label={label} value={value ?? defaultValue} onChange={onChange} />
    case 'eid':
      return (
        <EidFieldEditor
          label={label}
          value={value}
          onChange={onChange}
          required={presentation?.required}
        />
      )
    // TODO(christoph): Add support for other types
    default:
      return <div>{t('scheme_configurator.error.unsupported_type', {type})}</div>
  }
}

interface ISchemaSingleField<T extends Schema> {
  name: string
  presentation: DeepReadonly<FieldPresentation>
  type: Type
  metadata: DeepReadonly<StudioComponentMetadata<T>>
  values: ReadData<T>
  onChange: (updater: (current: ReadData<T>) => ReadData<T>) => void
}

// eslint-disable-next-line arrow-parens
const SchemaSingleField = <T extends Schema>({
  name, presentation, type, metadata, values, onChange,
}: ISchemaSingleField<T>) => {
  const key = name
  const label = presentation?.label || deriveFieldLabel(key)
  const {schemaDefaults} = metadata
  const defaultValue = schemaDefaults?.[key]
  const {schemaPresentation} = metadata
  const handleChange = (newValue: any) => onChange(current => ({...current, [key]: newValue}))

  switch (presentation?.mode) {
    case 'attribute':
      return (
        <AttributeField
          key={key}
          label={label}
          value={values[key]}
          defaultValue={defaultValue as string}
          attributeType={presentation.attribute}
          onChange={handleChange}
        />
      )

    case 'property': {
      // Make sure that the attribute is a string
      let attribute: PropertyType = values[presentation.propertyOf]
      if (typeof attribute !== 'string') {
        attribute = String(schemaDefaults?.[presentation.propertyOf])
      }

      return (
        <PropertyField
          key={key}
          label={label}
          value={values[key]}
          defaultValue={defaultValue as string}
          attributeType={schemaPresentation.fields[presentation.propertyOf]?.attribute}
          selectedAttribute={attribute}
          onChange={handleChange}
        />
      ) }

    default:
      return (
        <Field
          key={key}
          label={label}
          type={type}
          value={values[key]}
          defaultValue={defaultValue}
          presentation={presentation}
          onChange={handleChange}
        />
      )
  }
}

interface ISchemaFields<T extends Schema> {
  fields: Record<string, FieldDisplayData>
  metadata: DeepReadonly<StudioComponentMetadata<T>>
  values: ReadData<T>
  onChange: (updater: (current: ReadData<T>) => ReadData<T>) => void
}

const SchemaFields = <T extends Schema>({fields, metadata, values, onChange}: ISchemaFields<T>) => (
  <div>
    {Object.entries(fields).map(([key, displayData]) => {
      switch (displayData.mode) {
        case 'group': {
          const handleChange = (
            newValues: Record<string, PropertyType>
          ) => onChange(current => ({...current, ...newValues}))

          return (
            <GroupField
              key={key}
              label={<span>{displayData.presentation?.label || deriveFieldLabel(key)}</span>}
              data={displayData}
              presentation={displayData.presentation}
              onChange={handleChange}
            />
          )
        }
        case 'single':
          return (
            <SchemaSingleField
              key={key}
              name={key}
              presentation={displayData.presentation}
              type={displayData.type}
              metadata={metadata}
              values={values}
              onChange={onChange}
            />
          )
        case 'section':
          return (
            <CollapsibleSection
              title={displayData.presentation?.label || deriveFieldLabel(key)}
              defaultClosed={displayData.presentation?.defaultClosed}
              sectionId={`schema-fields/${key}`}
            >
              <SchemaFields
                fields={displayData.subFields}
                metadata={metadata}
                values={values}
                onChange={onChange}
              />
            </CollapsibleSection>
          )
        default:
          return null
      }
    })}
  </div>
)

interface ISchemaConfigurator<T extends Schema> {
  metadata: DeepReadonly<StudioComponentMetadata<T>>
  values: ReadData<T>
  onChange: (updater: (current: ReadData<T>) => ReadData<T>) => void
}

const SchemaConfigurator = <T extends Schema>({
  metadata, values, onChange,
}: ISchemaConfigurator<T>) => (
  <div>
    {metadata.schema &&
      <SchemaFields
        fields={buildDisplayDataTree(metadata, values)}
        metadata={metadata}
        values={values}
        onChange={onChange}
      />}
  </div>
  )

export {
  SchemaConfigurator,
}

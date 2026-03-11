import type {AttributeTypeName, Schema} from '@ecs/shared/schema'
import {
  getBasicType, isBasicTypeNumber, hasAttributesWithPropertiesOfType,
} from '@ecs/shared/schema-utility'

import type {DeepReadonly} from 'ts-essentials'

import React from 'react'

import {useTranslation} from 'react-i18next'

import {useStudioComponentsContext} from '../studio-components-context'

import {RowSelectField} from './row-fields'

// Build a list of all properties on the attribute that match the type
const usePropertiesOfType = (attributeName: string, typeName: AttributeTypeName): string[] => {
  const context = useStudioComponentsContext()
  const attributeSchema = context.getComponentSchema(attributeName)?.schema
  if (!attributeSchema) {
    return []
  }

  const values: string[] = []
  Object.entries(attributeSchema).forEach((property) => {
    if (getBasicType(property[1]) === typeName) {
      values.push(property[0])
    }
  })

  return values
}

const hasVector3Attributes = (schema: DeepReadonly<Schema>): boolean => schema &&
  isBasicTypeNumber(schema.x) && isBasicTypeNumber(schema.y) && isBasicTypeNumber(schema.z)

// Build the list attributes that can be used for a given type
const useAttributeList = (typeName: AttributeTypeName): string[] => {
  const context = useStudioComponentsContext()
  const attributes = context.listComponents()
  return attributes.filter((name) => {
    const schema = context.getComponentSchema(name)?.schema
    if (!schema) {
      return false
    }

    return typeName === 'vector3'
      ? hasVector3Attributes(schema)
      : hasAttributesWithPropertiesOfType(schema, typeName)
  })
}

interface IAttributeField {
  label: React.ReactNode
  value: any
  defaultValue?: string
  attributeType: AttributeTypeName
  onChange: (newValue: any) => void
}

const AttributeField: React.FC<IAttributeField> = (
  {label, value, defaultValue, attributeType, onChange}
) => {
  const id = React.useId()
  const {t} = useTranslation('cloud-studio-pages')

  return (
    <RowSelectField
      id={id}
      label={label}
      value={value ?? defaultValue ?? ''}
      options={[
        {value: '', content: t('attribute_property_field.option.no_attribute')},
        ...useAttributeList(attributeType).map(attr => ({value: attr, content: attr})),
      ]}
      onChange={onChange}
    />
  )
}
interface IPropertyField {
  label: React.ReactNode
  value: any
  defaultValue?: string
  attributeType: AttributeTypeName
  selectedAttribute: string | undefined
  onChange: (newValue: string) => void
}

const PropertyField: React.FC<IPropertyField> = (
  {label, value, defaultValue, attributeType, selectedAttribute, onChange}
) => {
  const {t} = useTranslation('cloud-studio-pages')
  const id = React.useId()
  const options = usePropertiesOfType(selectedAttribute, attributeType)

  return (
    <RowSelectField
      id={id}
      label={label}
      value={value ?? defaultValue ?? ''}
      options={[
        {value: '', content: t('attribute_property_field.option.no_property')},
        ...options.map(option => ({value: option, content: option})),
      ]}
      onChange={onChange}
    />
  )
}

export {
  AttributeField,
  PropertyField,
}

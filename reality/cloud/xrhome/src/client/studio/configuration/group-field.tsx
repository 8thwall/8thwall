import React from 'react'
import {parseColor} from '@ecs/shared/color'
import {useTranslation} from 'react-i18next'

import type {GroupPresentation, PropertyType} from '@ecs/shared/schema'

import {RowColorField, RowGroupFields} from './row-fields'
import {SliderInputAxisField} from '../../ui/components/slider-input-axis-field'
import type {FieldDisplayData} from './schema-presentation'

interface IDisplayNumberField {
  fieldName: string
  data: FieldDisplayData
  defaultLabel: string
  onChange: (newValues: Record<string, PropertyType>) => void
}

const DisplayNumberField: React.FC<IDisplayNumberField> = (
  {fieldName, data, defaultLabel, onChange}
) => {
  const {t} = useTranslation('cloud-studio-pages')
  const id = React.useId()
  if (data.mode !== 'single') {
    return <div>{t('group_fields.invalid_data')}</div>
  }
  return (
    <SliderInputAxisField
      id={id}
      step={1}
      label={data.presentation?.label || defaultLabel}
      // We do checks when parsing the schema to ensure that the field type is a number
      value={data.value as number}
      onChange={newValue => onChange({[fieldName]: newValue})}
      fullWidth
    />
  )
}

interface IVector3GroupField {
  label: React.ReactNode
  data: FieldDisplayData
  onChange: (newValues: Record<string, PropertyType>) => void
}

const Vector3GroupField: React.FC<IVector3GroupField> = ({label, data, onChange}) => {
  const {t} = useTranslation('cloud-studio-pages')
  if (data.mode !== 'group') {
    return <div>{t('group_fields.invalid_data')}</div>
  }
  const fields = Object.entries(data.subFields)
  if (fields.length !== 3) {
    return <div>{t('group_fields.invalid_data')}</div>
  }

  return (
    <RowGroupFields label={label}>
      <DisplayNumberField
        fieldName={fields[0][0]}
        data={fields[0][1]}
        defaultLabel='X'
        onChange={onChange}
      />
      <DisplayNumberField
        fieldName={fields[1][0]}
        data={fields[1][1]}
        defaultLabel='Y'
        onChange={onChange}
      />
      <DisplayNumberField
        fieldName={fields[2][0]}
        data={fields[2][1]}
        defaultLabel='Z'
        onChange={onChange}
      />
    </RowGroupFields>
  )
}

interface IColorGroupField {
  label: React.ReactNode
  data: FieldDisplayData
  onChange: (newValues: Record<string, PropertyType>) => void
}

// Convert a single color component value to a two-digit hexadecimal string
const colorComponentToString = (
  data: FieldDisplayData
) => (data.mode === 'single' ? data.value.toString(16).padStart(2, '0') : '').toUpperCase()

const ColorGroupField: React.FC<IColorGroupField> = ({label, data, onChange}) => {
  const id = React.useId()
  const {t} = useTranslation('cloud-studio-pages')
  if (data.mode !== 'group') {
    return <div>{t('group_fields.invalid_data')}</div>
  }
  const fields = Object.entries(data.subFields)
  if (fields.length !== 3) {
    return <div>{t('group_fields.invalid_data')}</div>
  }

  const getColor = (): string => {
    // Otherwise, build the color from the stored field values
    const redString = colorComponentToString(fields[0][1])
    const greenString = colorComponentToString(fields[1][1])
    const blueString = colorComponentToString(fields[2][1])
    return `#${redString}${greenString}${blueString}`
  }

  const handleChange = (newColor: string) => {
    const rgb = parseColor(newColor)
    // Color is checked in RowColorField before calling this function, but early out just in case
    if (!rgb) {
      return
    }

    const redFieldName = fields[0][0]
    const greenFieldName = fields[1][0]
    const blueFieldName = fields[2][0]

    onChange({[redFieldName]: rgb.r, [greenFieldName]: rgb.g, [blueFieldName]: rgb.b})
  }

  return (
    <RowColorField
      id={id}
      label={label}
      value={getColor()}
      onChange={handleChange}
    />
  )
}

interface IGroupField {
  label: React.ReactNode
  data: FieldDisplayData
  presentation: GroupPresentation
  onChange: (newValues: Record<string, PropertyType>) => void
}

const GroupField: React.FC<IGroupField> = ({label, data, presentation, onChange}) => {
  const {t} = useTranslation('cloud-studio-pages')
  switch (presentation.type) {
    case 'vector3':
      return (
        <Vector3GroupField
          label={label}
          data={data}
          onChange={onChange}
        />
      )
    case 'color':
      return (
        <ColorGroupField
          label={label}
          data={data}
          onChange={onChange}
        />
      )
    default:
      // TODO(jperrins): J8W-2405 Implement a generic group field
      return <div>{t('group_fields.field_not_implemented')}</div>
  }
}

export {
  GroupField,
}

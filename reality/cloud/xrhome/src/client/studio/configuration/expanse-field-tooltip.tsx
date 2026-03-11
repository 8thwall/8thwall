import React from 'react'
import {useTranslation} from 'react-i18next'

import type {EditorTooltipData, PropertyEntry} from '@ecs/shared/tooltip-types'

import type {ExpanseField} from './expanse-field-types'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import * as RAW_TOOLTIP_DATA from '../tooltip-data.json'
import {StandardLink} from '../../ui/components/standard-link'
import {useScenePathContext} from '../scene-path-context'
import {Tooltip} from '../../ui/components/tooltip'

const TOOLTIP_DATA: EditorTooltipData = RAW_TOOLTIP_DATA

type TooltipData = {
  accessor: string
  link: string
  textKey: string
  properties: Array<PropertyEntry>
}

const makeExampleValue = (property: PropertyEntry) => {
  if (property.exampleValue) {
    return property.exampleValue
  }

  if (property.defaultValue) {
    return property.defaultValue
  }

  switch (property.type) {
    case 'string':
      return '\'\''
    case 'number':
      return '0'
    case 'boolean':
      return 'true'
    default:
      return '<unknown>'
  }
}

const getExampleCode = (data: TooltipData) => {
  const propertyLines = data.properties.map(property => (
    `  ${property.name}: ${makeExampleValue(property)},`
  )).join('\n')

  return `ecs.${data.accessor}.set(world, component.eid, {\n${propertyLines}\n})`
}

type TooltipExpanseField = ExpanseField<DefaultableConfigDiffInfo<unknown, string[][]>>

const useFullPaths = (field: TooltipExpanseField): string[][] | null => {
  const previousPath = useScenePathContext()

  if (!field) {
    return null
  }

  if (!previousPath) {
    return null
  }

  const fullPaths = (typeof field === 'object' && field?.overridePaths)
    ? [...field.overridePaths]
    : []

  const leafPaths: string[][] = []
  if (typeof field === 'string') {
    leafPaths.push([field])
  } else {
    const {leafPaths: rawLeafPaths} = field
    if (Array.isArray(rawLeafPaths)) {
      leafPaths.push(...rawLeafPaths)
    } else {
      leafPaths.push([rawLeafPaths])
    }
  }

  leafPaths.forEach((leaf) => {
    fullPaths.push([...previousPath, ...leaf])
  })

  return fullPaths
}

const convertToTooltipKey = (fullPath: string[]): string | null => {
  if (fullPath[0] !== 'objects') {
    return null
  }
  if (fullPath[2] === 'components') {
    if (fullPath[4] === 'parameters') {
      // TODO(christoph): This is actually wrong because the component object is keyed
      // by random ID, not the component name. To make it work, I think we'll need to
      // read from the scene graph.
      return `component.${fullPath[3]}.${fullPath[5]}`
    } else {
      return null
    }
  }
  // TODO(christoph): Add logic for scoped prefixes for material/geometry
  if (fullPath.length >= 4) {
    return `object.${fullPath.slice(2).join('.')}`
  }

  return null
}

const useTooltipData = (expanseField: TooltipExpanseField): TooltipData => {
  const fullPaths = useFullPaths(expanseField)
  const tooltipKeys = fullPaths
    ?.map(convertToTooltipKey).filter(Boolean)

  const properties = tooltipKeys
    ?.map(e => TOOLTIP_DATA.propertiesBySceneGraph[e]).flat().filter(Boolean)

  if (!properties?.length) {
    return null
  }
  const {accessor} = properties[0]
  const componentData = TOOLTIP_DATA.components[accessor]
  if (!componentData) {
    return null
  }

  return {
    accessor,
    link: componentData.link,
    textKey: componentData.textKey,
    properties,
  }
}

interface IExpanseFieldTooltipContent {
  data: TooltipData
}

const ExpanseFieldTooltipContent: React.FC<IExpanseFieldTooltipContent> = ({data}) => {
  const {t} = useTranslation('studio-tooltips')

  return (
    <div>
      {data.link
        ? <StandardLink newTab href={data.link}>ecs.{data.accessor}</StandardLink>
        : <>ecs.{data.accessor}</>}
      <br />
      {data.textKey && t(data.textKey)}
      <hr />
      {data.properties.map(tooltip => (
        <div key={tooltip.name}>
          {tooltip.name} {'{'}{tooltip.type}{'}'}
          <br />
          {t(tooltip.textKey)}
        </div>
      ))}
      <code>
        <pre>
          {getExampleCode(data)}
        </pre>
      </code>
    </div>
  )
}

interface IExpanseFieldTooltip {
  data: TooltipData
  children: React.ReactNode
}

const ExpanseFieldTooltip: React.FC<IExpanseFieldTooltip> = ({data, children}) => (
  <Tooltip
    position='left'
    content={(
      <React.Suspense fallback={null}>
        <ExpanseFieldTooltipContent data={data} />
      </React.Suspense>
    )}
  >
    {children}
  </Tooltip>
)

export {
  useTooltipData,
  ExpanseFieldTooltip,
}

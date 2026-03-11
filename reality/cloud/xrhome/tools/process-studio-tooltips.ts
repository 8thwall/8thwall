import fs from 'fs'
import path from 'path'

import type {
  ProcessedTooltipData, ParsedRow, PropertyEntry, EditorTooltipData,
  RuntimeTooltipData,
} from '@ecs/shared/tooltip-types'

// Usage:
// - Go to http://go.8thwall.com/cstooltips
// - Download the spreadsheet as a tab-separated values (TSV) file
// - Open a terminal in the `reality/cloud/xrhome` directory
// - Run the command: `cat ~/Desktop/tooltip-data.tsv | npx ts-node process-tooltips.ts

/* eslint-disable no-console */

const root = path.join(__dirname, '../../../..')
const xrhomeDir = path.join(root, 'reality/cloud/xrhome')
const ecsDir = path.join(root, 'c8/ecs')

const runtimeTooltipDataPath = path.join(ecsDir, 'gen/runtime-tooltip-data.json')
const editorTooltipDataPath = path.join(xrhomeDir, 'src/client/studio/tooltip-data.json')

const makeTooltipStringsPath = (language: string) => (
  path.join(xrhomeDir, 'src', 'client', 'i18n', language, 'studio-tooltips.json')
)

// eslint-disable-next-line arrow-parens
const sortJson = <T>(value: T): T => {
  if (Array.isArray(value)) {
    return value.map(sortJson) as T
  } else if (typeof value === 'object' && value !== null) {
    const sortedObj: Record<string, any> = {}
    Object.keys(value).sort().forEach((key) => {
      sortedObj[key] = sortJson((value as Record<string, any>)[key])
    })
    return sortedObj as T
  }
  return value
}

const writeJson = (filePath: string, content: {}) => {
  fs.mkdirSync(path.dirname(filePath), {recursive: true})
  fs.writeFileSync(filePath, `${JSON.stringify(content, null, 2)}\n`, 'utf8')
}

const toSnakeCase = (str: string): string => (
  str.replace(/.?[A-Z]+/g, match => (match.length === 1
    ? match.toLowerCase()
    : `${match[0]}_${match.slice(1).split('').join('_').toLowerCase()}`))
)

const makeKey = (accessor: string, name?: string): string => {
  if (!name) {
    return `tooltip.${toSnakeCase(accessor)}.text`
  }
  return `tooltip.${toSnakeCase(accessor)}.${toSnakeCase(name)}.text`
}

const asString = (v: any) => {
  if (typeof v !== 'string') {
    throw new Error(`Expected string, got ${typeof v}`)
  }
  return v.trim()
}

const asUrl = (v: any) => {
  try {
    return new URL(asString(v)).toString()
  } catch (e) {
    throw new Error(`Expected URL, got ${v}`)
  }
}

// eslint-disable-next-line arrow-parens
const optionalOr = <T>(v: any, fn: (v: any) => T) => {
  if (!v) {
    return undefined
  }

  return fn(v)
}

const fixBooleans = (v: string) => {
  switch (v) {
    case 'TRUE':
      return 'true'
    case 'FALSE':
      return 'false'
    default:
      return v
  }
}

const storeComponentData = (
  res: ProcessedTooltipData,
  row: ParsedRow
): void => {
  if (!row.accessor) {
    throw new Error(`Missing accessor in row: ${JSON.stringify(row)}`)
  }

  if (res.components[row.accessor]) {
    throw new Error(`Duplicate component headerKey found: ${row.accessor}`)
  }

  if (!row.description) {
    throw new Error(`Missing description for component: ${row.accessor}`)
  }

  if (!row.headerLink) {
    console.warn(`Missing headerLink for component: ${row.accessor}`)
  }

  let textKey: string
  if (row.description === 'TODO') {
    console.warn(`TODO description for component: ${row.accessor}`)
  } else if (row.description !== 'X') {
    textKey = makeKey(row.accessor)
    if (res.strings[textKey]) {
      throw new Error(`Duplicate string key found: ${textKey}`)
    }
    res.strings[textKey] = row.description
  }

  res.components[row.accessor] = {
    textKey,
    link: row.headerLink,
  }
}

const storeScopedComponentData = (
  res: ProcessedTooltipData,
  sceneGraphKey: string,
  scope: string,
  entry: PropertyEntry
): void => {
  if (!res.scopedPropertiesBySceneGraph[sceneGraphKey]) {
    res.scopedPropertiesBySceneGraph[sceneGraphKey] = {}
  }

  if (!res.scopedPropertiesBySceneGraph[sceneGraphKey][scope]) {
    res.scopedPropertiesBySceneGraph[sceneGraphKey][scope] = []
  }

  res.scopedPropertiesBySceneGraph[sceneGraphKey][scope].push(entry)
}

const storePropertyData = (
  res: ProcessedTooltipData,
  row: ParsedRow
): void => {
  if (!row.accessor) {
    throw new Error(`Missing accessor in row: ${JSON.stringify(row)}`)
  }

  if (!row.name) {
    throw new Error(`Missing name in row: ${JSON.stringify(row)}`)
  }

  const runtimeKey = `${row.accessor}.${row.name}`

  let textKey: string
  if (row.description === 'TODO') {
    console.warn(`TODO description for: ${row.accessor}.${row.name}`)
  } else if (row.description !== 'X') {
    textKey = makeKey(row.accessor, row.name)
    res.strings[textKey] = row.description
  }

  const entry: PropertyEntry = {
    accessor: row.accessor,
    name: row.name,
    textKey,
    defaultValue: row.defaultValue,
    exampleValue: row.exampleValue,
    type: row.type,
  }

  if (row.status !== 'REMOVED') {
    if (res.propertiesByRuntime[runtimeKey]) {
      throw new Error(`Duplicate property runtime key found: ${runtimeKey}`)
    }
    res.propertiesByRuntime[runtimeKey] = entry

    if (!res.groupedPropertiesByRuntime[row.accessor]) {
      res.groupedPropertiesByRuntime[row.accessor] = []
    }
    res.groupedPropertiesByRuntime[row.accessor].push(res.propertiesByRuntime[runtimeKey])
  }

  if (!textKey) {
    // Don't generate scene graph keys for X properties
    return
  }

  if (!row.sceneGraphKey) {
    console.warn(`Missing sceneGraphKey in row: ${row.accessor}.${row.name}`)
    return
  }

  const parts = row.sceneGraphKey.split('(')

  if (parts.length === 1) {
    if (!res.propertiesBySceneGraph[row.sceneGraphKey]) {
      res.propertiesBySceneGraph[row.sceneGraphKey] = []
    }

    res.propertiesBySceneGraph[row.sceneGraphKey].push(entry)
  } else if (parts.length === 2) {
    const [prefix, rest] = parts
    const [scope, suffix] = rest.split(')')
    const sceneGraphKey = `${prefix.trim()}${suffix.trim()}`
    storeScopedComponentData(res, sceneGraphKey, scope, entry)
  } else {
    throw new Error(`Unexpected sceneGraphKey format: ${row.sceneGraphKey}`)
  }
}

// eslint-disable-next-line arrow-parens
const asOneOf = <T>(values: T[], v: any): T => {
  if (!values.includes(v)) {
    throw new Error(`Expected one of ${values.join(', ')}, got ${v}`)
  }
  return v
}

const run = () => {
  const data: string = fs.readFileSync(0, 'utf8')

  const [headerLine, ...lines] = data.split('\n').filter(line => line.trim().length > 0)

  const headers = headerLine.split('\t').map(h => h.trim())

  const findRow = (header: string) => {
    const index = headers.findIndex(h => h.includes(header))
    if (index === -1) {
      throw new Error(`Header "${header}" not found in the input data.`)
    }
    return index
  }

  const RUNTIME_ACCESSOR_INDEX = findRow('Runtime Accessor')
  const RUNTIME_PROPERTY_NAME_INDEX = findRow('Runtime Property Name')
  const SCENE_GRAPH_KEY_INDEX = findRow('Scene Graph Key')
  const DESCRIPTION_INDEX = findRow('Description')
  const TYPE_INDEX = findRow('Type')
  const DEFAULT_VALUE_INDEX = findRow('Default')
  const EXAMPLE_VALUE_INDEX = findRow('Example Value')
  const HEADER_LINK_INDEX = findRow('Header Link')
  const STATUS_INDEX = findRow('Status')

  const rows = lines.map(line => line.split('\t'))

  const parsedRows = rows.map((row): ParsedRow => ({
    accessor: optionalOr(row[RUNTIME_ACCESSOR_INDEX], asString),
    name: optionalOr(row[RUNTIME_PROPERTY_NAME_INDEX], asString),
    sceneGraphKey: optionalOr(row[SCENE_GRAPH_KEY_INDEX], asString),
    description: optionalOr(row[DESCRIPTION_INDEX], asString),
    type: optionalOr(row[TYPE_INDEX], asString),
    defaultValue: fixBooleans(optionalOr(row[DEFAULT_VALUE_INDEX], asString)),
    exampleValue: fixBooleans(optionalOr(row[EXAMPLE_VALUE_INDEX], asString)),
    headerLink: optionalOr(row[HEADER_LINK_INDEX], asString),
    status: optionalOr(row[STATUS_INDEX], v => asOneOf(['REMOVED'], v)),
  }))

  const res: ProcessedTooltipData = {
    components: {},
    propertiesBySceneGraph: {},
    scopedPropertiesBySceneGraph: {},
    propertiesByRuntime: {},
    groupedPropertiesByRuntime: {},
    strings: {},
  }

  for (const row of parsedRows) {
    if (row.name) {
      storePropertyData(res, row)
    } else {
      storeComponentData(res, row)
    }
  }

  const fixLink = (link: string | undefined) => {
    if (!link || link === 'X') {
      return undefined
    }
    return asUrl(link)
  }

  const editorTooltipData: EditorTooltipData = {
    components: Object.fromEntries(Object.entries(res.components)
      .filter(([, component]) => component.textKey)
      .map(([key, component]) => [key, {...component, link: fixLink(component.link)}])),
    propertiesBySceneGraph: res.propertiesBySceneGraph,
    scopedPropertiesBySceneGraph: res.scopedPropertiesBySceneGraph,
  }

  const runtimeTooltipData: RuntimeTooltipData = {
    components: res.components,
    propertiesByRuntime: res.propertiesByRuntime,
  }

  writeJson(runtimeTooltipDataPath, runtimeTooltipData)
  writeJson(editorTooltipDataPath, sortJson(editorTooltipData))
  writeJson(makeTooltipStringsPath('en-US'), sortJson(res.strings))
}

run()

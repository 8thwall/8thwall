type ParsedRow = {
  accessor: string
  name: string
  sceneGraphKey: string
  description: string
  type: string
  defaultValue: string
  exampleValue: string
  headerLink: string
  status: string
}

type PropertyEntry = {
  accessor: string
  name: string
  textKey: string
  defaultValue?: string | undefined
  exampleValue?: string | undefined
  type?: string | undefined
}

type ComponentEntry = {
  textKey?: string | undefined
  link?: string | undefined
}

type ScopedComponentEntry = Record<string, PropertyEntry[]>

type ProcessedTooltipData = {
  components: Record<string, ComponentEntry>
  propertiesByRuntime: Record<string, PropertyEntry>
  propertiesBySceneGraph: Record<string, PropertyEntry[]>
  scopedPropertiesBySceneGraph: Record<string, ScopedComponentEntry>
  groupedPropertiesByRuntime: Record<string, PropertyEntry[]>
  strings: Record<string, string>
}

type RuntimeTooltipData = Pick<ProcessedTooltipData,
  'components' | 'propertiesByRuntime'
>

type EditorTooltipData = Pick<ProcessedTooltipData,
'components' | 'propertiesBySceneGraph' | 'scopedPropertiesBySceneGraph'
>

export type {
  ParsedRow,
  PropertyEntry,
  ComponentEntry,
  ScopedComponentEntry,
  ProcessedTooltipData,
  RuntimeTooltipData,
  EditorTooltipData,
}

import type {ReadData, Schema, SchemaPresentation} from './schema'

interface Location {
  startLine: number
  startColumn: number
  endLine: number
  endColumn: number
}

type StudioComponentMetadata<T extends Schema = Schema> = {
  schema: T
  schemaPresentation?: SchemaPresentation
  schemaDefaults?: Partial<ReadData<T>>
  location?: Location
}

type StudioComponentError = {
  message: string
  severity: 'warning' | 'error'
  location?: Location
}

type StudioComponentData = {
  name: string
} & StudioComponentMetadata

type ParsedComponents = {
  componentData: StudioComponentData[]
  errors: StudioComponentError[]
}

export type {
  StudioComponentData,
  StudioComponentMetadata,
  Location,
  StudioComponentError,
  ParsedComponents,
}

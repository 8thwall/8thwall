import type {ALL_ATTRIBUTE_TYPES, GROUP_TYPES} from './component-constants'
import type {ASSET_KINDS} from './asset-constants'

type Eid = bigint

type TypeToValue = {
  'eid': Eid
  'f32': number
  'f64': number
  'i32': number
  'ui8': number
  'ui32': number
  'string': string
  'boolean': boolean
}

type Type = keyof TypeToValue

type ElementOf<T extends Type> = TypeToValue[T]

type PropertyType = TypeToValue[keyof TypeToValue]

interface Schema {
  [key: string]: Type
}

type SchemaDefaultType = number | string | boolean
interface SchemaDefaults {
  [key: string]: SchemaDefaultType
}

type AssetKind = typeof ASSET_KINDS[number]

type FieldPresentationMode = 'enum' | 'attribute' | 'property' | 'asset' | 'color'
type AttributeTypeName = typeof ALL_ATTRIBUTE_TYPES[number]
type GroupType = typeof GROUP_TYPES[number]

type Presentation = {
  label?: string
  condition?: string
}

interface FieldPresentation extends Presentation {
  mode?: FieldPresentationMode
  enumValues?: readonly string[]
  // If set, this field is an attribute of the entity.
  // The value is the type of attribute (this indicates what properties are valid)
  attribute?: AttributeTypeName
  // If set, this field is a property of an attribute on the entity.
  // The value is the name of the attribute field.
  propertyOf?: string
  group?: string
  section?: string
  min?: number
  max?: number
  assetKind?: AssetKind
  required?: boolean
}

interface GroupPresentation extends Presentation {
  type?: GroupType
}

interface SectionPresentation extends Presentation {
  defaultClosed?: boolean
}

type SchemaPresentation = {
  fields: Record<string, FieldPresentation>
  groups: Record<string, GroupPresentation>
  sections: Record<string, SectionPresentation>
}

type ReadData<T extends Schema> = {readonly [key in keyof T]: ElementOf<T[key]>}
type WriteData<T extends Schema> = {-readonly [key in keyof T]: ElementOf<T[key]>}

type OrderedSchema = Array<[string, Type, number]>

export type {
  Type,
  Schema,
  SchemaDefaults,
  SchemaDefaultType,
  FieldPresentationMode,
  Presentation,
  GroupPresentation,
  FieldPresentation,
  SectionPresentation,
  SchemaPresentation,
  ElementOf,
  ReadData,
  WriteData,
  TypeToValue,
  PropertyType,
  OrderedSchema,
  AttributeTypeName,
  GroupType,
  Eid,
  AssetKind,
}

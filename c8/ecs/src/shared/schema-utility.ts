import type {DeepReadonly} from 'ts-essentials'

import type {AttributeTypeName, Schema, Type} from './schema'
import {NUMERIC_SCHEMA_TYPES} from './component-constants'

const isBasicTypeNumber = (
  schemaType: Type
): boolean => NUMERIC_SCHEMA_TYPES.includes(schemaType as any)

const getBasicType = (schemaType: Type): string => (
  isBasicTypeNumber(schemaType) ? 'number' : schemaType)

const hasAttributesWithPropertiesOfType = (
  schema: DeepReadonly<Schema>, typeName: AttributeTypeName
): boolean => Object.values(schema).some(property => getBasicType(property) === typeName, false)

export {
  isBasicTypeNumber,
  getBasicType,
  hasAttributesWithPropertiesOfType,
}

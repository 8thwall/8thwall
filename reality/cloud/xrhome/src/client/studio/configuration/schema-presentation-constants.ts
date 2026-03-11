import type {Type, PropertyType} from '@ecs/shared/schema'
import type {DeepReadonly} from 'ts-essentials'

const TYPE_TO_FIELD_DEFAULT: DeepReadonly<Record<Type, PropertyType>> = {
  // Default entity is null because this object is used by the scene graph (not the runtime).
  // A valid entity reference here is an object and we don't have access to the BigInt type.
  eid: null,
  f32: 0,
  f64: 0,
  i32: 0,
  ui8: 0,
  ui32: 0,
  string: '',
  boolean: false,
}

export {
  TYPE_TO_FIELD_DEFAULT,
}

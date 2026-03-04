// Names of all types that are valid for attributes (components of an entity that
// can be modified/animated by other components via the attribute presentation field)
const ALL_ATTRIBUTE_TYPES = ['number', 'string', 'boolean', 'vector3'] as const

const NUMERIC_SCHEMA_TYPES = ['f32', 'f64', 'i32', 'ui8', 'ui32'] as const

const GROUP_TYPES = ['vector3', 'color'] as const

export {
  ALL_ATTRIBUTE_TYPES,
  NUMERIC_SCHEMA_TYPES,
  GROUP_TYPES,
}

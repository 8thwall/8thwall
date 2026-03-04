import type {Type, Schema, ReadData} from '../shared/schema'

const getTypeBounds = (type: Type) => {
  switch (type) {
    case 'f32':
    case 'f64':
      return [Number.NEGATIVE_INFINITY, Number.POSITIVE_INFINITY]
    case 'i32':
      return [-0x80000000, 0x7fffffff]
    case 'ui8':
      return [0, 0xff]
    case 'ui32':
      return [0, 0xffffffff]
    default:
      throw new Error(`Boundaries not found for type: '${type}'`)
  }
}

const getInvalidValueError = (key: string, value: any, schemaType: Type) => (
  `Invalid value '${value}' for key '${key}' with type '${schemaType}'`
)

const validateNumberType = (value: any, type: Type) => {
  if (typeof value !== 'number') {
    return false
  }

  if (!(type === 'f32' || type === 'f64') && !Number.isInteger(value)) {
    return false
  }

  const [min, max] = getTypeBounds(type)
  if (value < min || value > max) {
    return false
  }

  return true
}

const validateCustomDefaults = <T extends Schema>(
  schema?: T, customDefaults?: Partial<ReadData<T>>
) => {
  if (!customDefaults) {
    return
  }

  const errors: string[] = []

  Object.entries(customDefaults).forEach(([key, value]) => {
    if (!schema?.[key]) {
      errors.push(`Key '${key}' not in schema`)
      return
    }

    switch (schema[key]) {
      case 'eid':
        if (typeof value !== 'bigint') {
          errors.push(getInvalidValueError(key, value, schema[key]))
        }
        break
      case 'f32':
      case 'f64':
      case 'i32':
      case 'ui8':
      case 'ui32': {
        if (!validateNumberType(value, schema[key])) {
          errors.push(getInvalidValueError(key, value, schema[key]))
        }
        break
      }
      case 'string':
        if (typeof value !== 'string') {
          errors.push(getInvalidValueError(key, value, schema[key]))
        }
        break
      case 'boolean':
        if (typeof value !== 'boolean') {
          errors.push(getInvalidValueError(key, value, schema[key]))
        }
        break
      default:
        errors.push(`Unsupported type in schema: ${schema[key]}`)
    }
  })

  if (errors.length > 0) {
    throw new Error(errors.join('\n'))
  }
}

export {
  validateNumberType,
  validateCustomDefaults,
}

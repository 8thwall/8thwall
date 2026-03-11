import type {ReadData, Schema, PropertyType} from '@ecs/shared/schema'
import type {StudioComponentMetadata} from '@ecs/shared/studio-component'
import type {DeepReadonly} from 'ts-essentials'

import {TYPE_TO_FIELD_DEFAULT} from './configuration/schema-presentation-constants'

// Combine the sparse current values, schema and defaults into a single map for ease of
// lookup of whatever the current value is for a given key
const aggregateValues = <T extends Schema>(
  // eslint-disable-next-line arrow-parens
  metadata: DeepReadonly<StudioComponentMetadata<T>>, values: ReadData<T>
) => {
  const allValues: Map<string, PropertyType> = new Map()

  Object.entries(metadata.schema).forEach(([key, value]) => {
    if (values[key] !== undefined) {
      allValues.set(key, values[key])
    } else if (metadata.schemaDefaults && metadata.schemaDefaults[key] !== undefined) {
      allValues.set(key, metadata.schemaDefaults[key])
    } else {
      allValues.set(key, TYPE_TO_FIELD_DEFAULT[value])
    }
  })

  return allValues
}

// Get the value from the token stream and return the index of the next token to read
const getValueFromTokenStream = (
  allValues: Map<string, PropertyType>, tokens: string[], streamIndex: number
): [PropertyType, number] => {
  if (tokens.length <= streamIndex) {
    throw new Error('condition ended unexpectedly')
  }
  const token = tokens[streamIndex]

  // TODO (jonathan) handle complex cases of more than one token
  return [allValues.has(token) ? allValues.get(token) : token, streamIndex + 1]
}

const getEqualityResult = (
  allValues: Map<string, PropertyType>, tokens: string[], streamIndex: number
): [boolean, number] => {
  // Get the left hand side of the equality comparison
  const [lhs, comparatorIndex] = getValueFromTokenStream(allValues, tokens, streamIndex)

  if (tokens.length < comparatorIndex + 2 || tokens[comparatorIndex] !== '=') {
    // no comparator found in condition. Just skip to the end of the tokens
    return [false, tokens.length]
  }

  // Get the right hand side of the equality comparison
  const [rhs, nextIndex] = getValueFromTokenStream(allValues, tokens, comparatorIndex + 1)

  // Boolean and strings don't work with javascript truthiness so do boolean explicitly
  if (typeof lhs === 'boolean' && typeof rhs === 'string') {
    return [lhs === (rhs === 'true'), nextIndex]
  }

  // Ditto null values
  if (lhs === null) {
    return [rhs === 'null', nextIndex]
  }

  // allow javascript equality comparison to attempt to convert the rhs to the type of the lhs
  // eslint-disable-next-line eqeqeq
  return [lhs == rhs, nextIndex]
}

// Evaluate a true/false condition for the current values of the component
const evaluateCondition = <T extends Schema>(
  // eslint-disable-next-line arrow-parens
  metadata: DeepReadonly<StudioComponentMetadata<T>>, values: ReadData<T>, condition: string
): boolean => {
  // Condition is optional, and evaluates to true if not present
  if (!condition) {
    return true
  }

  const allValues = aggregateValues(metadata, values)

  // Split the string into tokens while removing (and splitting on) whitespace
  // TODO (jonathan) add tokens (,), !, etc when we make this smarter
  const tokens = condition.split(/(?=[&|=])|(?<=[&|=])|\s+/)

  // Evaluate the first equality comparison
  let [result, index] = getEqualityResult(allValues, tokens, 0)

  // Evaluate subsequent equality comparisons if present and combine the results
  while (index < tokens.length && (tokens[index] === '&' || tokens[index] === '|')) {
    const [nextResult, nextIndex] = getEqualityResult(allValues, tokens, index + 1)
    index = nextIndex
    if (tokens[index - 1] === '&') {
      result = result && nextResult
    } else {
      result = result || nextResult
    }
  }

  return result
}

export {
  evaluateCondition,
}

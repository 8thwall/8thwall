import type {World} from './world'
import {createInstanced} from '../shared/instanced'

const makeStringStorage = () => {
  const idToString = new Map<number, string>()
  const stringToId = new Map<string, number>()
  let nextId = 1
  let warnedOnNonString = false

  const get = (id: number) => {
    if (id === 0) {
      return ''
    }
    if (!idToString.has(id)) {
      throw new Error(`String storage does not contain id ${id}`)
    }
    return idToString.get(id)!
  }

  const store = (rawValue: unknown): number => {
    if (rawValue === '') {
      return 0
    }

    let value: string
    if (typeof rawValue === 'string') {
      value = rawValue
    } else {
      if (!warnedOnNonString) {
        warnedOnNonString = true
        // eslint-disable-next-line no-console
        console.warn(
          'Non-string value passed to string field, values will be converted to string.'
        )
      }
      if (rawValue === undefined || rawValue === null) {
        return 0
      }
      value = String(rawValue)
    }

    if (stringToId.has(value)) {
      return stringToId.get(value)!
    }
    const id = nextId++
    idToString.set(id, value)
    stringToId.set(value, id)
    return id
  }

  return {get, store}
}

type StringStorage = ReturnType<typeof makeStringStorage>

const globalStringMap = createInstanced<World, StringStorage>(makeStringStorage)

export {
  globalStringMap,
}

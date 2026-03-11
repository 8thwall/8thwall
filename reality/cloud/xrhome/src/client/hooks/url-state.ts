import React from 'react'
import {useHistory, useLocation} from 'react-router-dom'
import type {DeepReadonly} from 'ts-essentials'

import type {MemoryHistory, History} from 'history'

import {useEvent} from './use-event'

type Setter<T> = [T] extends [Function] ? ((s: T) => T) : (T | ((s: T) => T))

const isMemoryHistory = (history: History): history is MemoryHistory<unknown> => (
  history.location !== undefined
)

const getCurrentLocation = (history: History) => (
  isMemoryHistory(history) ? history.location : window.location
)

const setValue = (history: History, key: string, value: string | undefined) => {
  const {search, pathname} = getCurrentLocation(history)
  const params = new URLSearchParams(search)
  if (value === undefined) {
    params.delete(key)
  } else {
    params.set(key, value)
  }
  const newSearch = params.toString()
  if (search === newSearch) {
    return
  }
  const newPath = newSearch ? `${pathname}?${newSearch}` : pathname
  history.replace(newPath)
}

const createUseUrlState = <T>(
  serialize: (value: T) => string | undefined,
  deserialize: (value: string) => T
) => (key: string, defaultValue: T) => {
    const {search} = useLocation()
    const history = useHistory()

    const value = React.useMemo(() => {
      const params = new URLSearchParams(search)
      if (params.has(key)) {
        return deserialize(params.get(key)!)
      } else {
        return defaultValue
      }
    }, [search, key, defaultValue])

    const setValueEvent = useEvent((setter: Setter<T>) => {
      const newValue = typeof setter === 'function' ? setter(value) : setter
      const serializedNewValue = serialize(newValue)
      const isDefault = serializedNewValue === serialize(defaultValue)
      setValue(history, key, isDefault ? undefined : serializedNewValue)
    })

    return [value, setValueEvent] as const
  }

const useStringUrlState = createUseUrlState<string | undefined>(
  value => value,
  value => value
)

const useBooleanUrlState = createUseUrlState<boolean>(
  value => (value ? '1' : '0'),
  value => value === '1'
)

const useStringArrayUrlState = createUseUrlState<DeepReadonly<string[]>>(
  value => value.join(','),
  value => (value.length ? value.split(',') : [])
)

const useNumberUrlState = createUseUrlState<number>(
  value => value.toString(),
  value => Number(value)
)

export {
  setValue,
  createUseUrlState,
  useStringUrlState,
  useBooleanUrlState,
  useStringArrayUrlState,
  useNumberUrlState,
}

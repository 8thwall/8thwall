import React, {useEffect, useRef} from 'react'
import type {DeepReadonly} from 'ts-essentials'

type EffectCallback<Deps> = (prevDeps: Deps) => (void | (() => void))

type ChangeEffect = <T extends DeepReadonly<any[]>>(callback: EffectCallback<T>, deps: T) => void

// Same as useEffect but you have access to the previous values of deps
const useChangeEffect: ChangeEffect = (callback, deps) => {
  const ref = React.useRef([] as any as typeof deps)
  React.useEffect(() => {
    const res = callback(ref.current)
    ref.current = deps
    return res
  }, deps)
}

// Taken from react-monaco. Same as useEffect but only runs when deps change, and not
// on first mount
const useUpdateEffect = (effect, deps) => {
  const isInitialMount = useRef(true)

  useEffect(
    isInitialMount.current
      ? () => { isInitialMount.current = false }
      : effect,
    deps
  )
}

// Same as useLayoutEffect but you have access to the previous values of deps
const useLayoutChangeEffect: ChangeEffect = (callback, deps) => {
  const ref = React.useRef([] as any as typeof deps)
  React.useLayoutEffect(() => {
    const res = callback(ref.current)
    ref.current = deps
    return res
  }, deps)
}

export {
  useChangeEffect,
  useLayoutChangeEffect,
  useUpdateEffect,
}

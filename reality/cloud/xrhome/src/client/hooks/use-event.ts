import {
  useLayoutEffect,
  useRef,
  useInsertionEffect,
} from 'react'

type AnyFunction = (...args: any[]) => any;

/**
 * Suppress the warning when using useLayoutEffect with SSR.
 * (https://reactjs.org/link/uselayouteffect-ssr)
 * Make use of useInsertionEffect if available.
 */
const useBrowserEffect =
  typeof window !== 'undefined' ? useInsertionEffect || useLayoutEffect : () => {}

/**
   * Render methods should be pure, especially when concurrency is used,
   * so we will throw this error if the callback is called while rendering.
   */
function shouldNotBeInvokedBeforeMount() {
  throw new Error(
    `INVALID_USEEVENT_INVOCATION:
      the callback from useEvent cannot be invoked before the component has mounted.`
  )
}

/**
 * Similar to useCallback, with a few subtle differences:
 * - The returned function is a stable reference, and will always be the same between renders
 * - No dependency lists required
 * - Properties or state accessed within the callback will always be "current"
 */
// eslint-disable-next-line arrow-parens
const useEvent = <TCallback extends AnyFunction>(callback: TCallback): TCallback => {
  // Keep track of the latest callback:
  const latestRef = useRef<TCallback>(shouldNotBeInvokedBeforeMount as any)
  useBrowserEffect(() => {
    latestRef.current = callback
  }, [callback])

  // Create a stable callback that always calls the latest callback:
  // using useRef instead of useCallback avoids creating and empty array on every render
  const stableRef = useRef<TCallback>(null as any)
  if (!stableRef.current) {
    stableRef.current = ((...rest) => latestRef.current(...rest)) as TCallback
  }

  return stableRef.current
}

export {
  useEvent,
}

// eslint-disable-next-line no-restricted-imports
import {
  TypedUseSelectorHook,
  useSelector as useBaseSelector,
} from 'react-redux'

import type {RootState} from './reducer'

// We want types in all the places.
// Reference:
// https://redux.js.org/usage/usage-with-typescript

/**
 * Use this EVERYWHERE instead of useSelector() because it gives you a typed state!
 */
const useSelector: TypedUseSelectorHook<RootState> = useBaseSelector

export {
  useSelector,
}

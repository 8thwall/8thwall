import React from 'react'
import {useDispatch} from 'react-redux'

import type {RawActions, Actions, Dispatch} from './types/actions'

/* eslint-disable-next-line arrow-parens */
const useActions = <T extends RawActions>(actions: Actions<T>) => {
  const dispatch = useDispatch<Dispatch>()

  return React.useMemo(() => actions(dispatch), [dispatch, actions])
}

export default useActions

import React from 'react'
import {useDispatch} from 'react-redux'

import type {RawActions, Actions, Dispatch} from './types/actions'

const useActions = <T extends RawActions>(actions: Actions<T>) => {
  const dispatch = useDispatch<Dispatch>()

  return React.useMemo(() => actions(dispatch), [dispatch, actions])
}

export default useActions

import {dispatchify} from 'bzl/examples/web/lib/dispatchify'
import type {DispatchifiedActions} from 'bzl/examples/web/lib/dispatchify-types'

const setTime = newTime => dispatch => (
  dispatch({type: 'TIME/SET', currentTime: newTime})
)

const setTimezone = tz => dispatch => (
  dispatch({type: 'TIME/SET_TZ', tz})
)

const rawActions = {
  setTime,
  setTimezone,
}

type TimeActions = DispatchifiedActions<typeof rawActions>
const timeActions = dispatchify(rawActions)

export {
  TimeActions,
  timeActions,
}

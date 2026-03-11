import {dispatchify} from '../common'

const acknowledgeError = () => dispatch => dispatch({type: 'ACKNOWLEDGE_ERROR'})

const acknowledgeMessage = () => dispatch => dispatch({type: 'ACKNOWLEDGE_MESSAGE'})

const acknowledgeSuccess = () => dispatch => dispatch({type: 'ACKNOWLEDGE_SUCCESS'})

export const rawActions = {
  acknowledgeError,
  acknowledgeMessage,
  acknowledgeSuccess,
}

export default dispatchify(rawActions)

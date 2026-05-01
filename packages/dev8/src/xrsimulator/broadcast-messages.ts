import {postMessage} from '../shared/utils/post-message'

const broadcastLoadingSequence = (status) => {
  postMessage('SIMULATOR_SEQUENCE_LOADING', {status})
}

const broadcastSequenceProgress = (simulatorId, currentProgress) => {
  postMessage('SEQUENCE_PROGRESS', {
    simulatorId,
    currentProgress,
  })
}

const broadcastReloadConfirmation = (simulatorId) => {
  postMessage('SIMULATOR_RELOAD_ACKNOWLEDGE', {
    simulatorId,
  })
}

const broadcastConfigUpdateConfirmation = (simulatorId) => {
  postMessage('SIMULATOR_CONFIG_UPDATE_ACKNOWLEDGE', {
    simulatorId,
  })
}

const broadcastInitializeConfirmation = (simulatorId) => {
  postMessage('SIMULATOR_INITIALIZE_ACKNOWLEDGE', {
    simulatorId,
  })
}

export {
  broadcastLoadingSequence,
  broadcastSequenceProgress,
  broadcastReloadConfirmation,
  broadcastConfigUpdateConfirmation,
  broadcastInitializeConfirmation,
}

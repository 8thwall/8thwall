import {postMessage} from '../shared/utils/post-message'
import {SimulatorHMDLinkActionType} from '../shared/webrtc/constants/remoteActions'

const sendWebRtcAnswerToXrHome = (webRTCData: unknown) => {
  const data = {webRTCData}

  postMessage(SimulatorHMDLinkActionType.WEBRTC_ANSWER, data)
}

const sendWebRtcConnectionClosedEventToXrHome = () => {
  postMessage(SimulatorHMDLinkActionType.WEBRTC_CONNECTION_CLOSED, {})
}

const sendWebRtcConnectionErrorEventToXrHome = () => {
  postMessage(SimulatorHMDLinkActionType.WEBRTC_CONNECTION_ERROR, {})
}

const sendWebRtcConnectionSuccessEventToXrHome = () => {
  postMessage(SimulatorHMDLinkActionType.WEBRTC_CONNECTION_SUCCESS, {})
}

const sendUnloadEventToXrHome = () => {
  postMessage(SimulatorHMDLinkActionType.UNLOAD, {})
}

const sendReadyForNewConnectionEventToXrHome = () => {
  postMessage(SimulatorHMDLinkActionType.READY_FOR_NEW_CONNECTION, {})
}

export {
  sendWebRtcConnectionClosedEventToXrHome,
  sendWebRtcConnectionErrorEventToXrHome,
  sendWebRtcConnectionSuccessEventToXrHome,
  sendWebRtcAnswerToXrHome,
  sendUnloadEventToXrHome,
  sendReadyForNewConnectionEventToXrHome,
}

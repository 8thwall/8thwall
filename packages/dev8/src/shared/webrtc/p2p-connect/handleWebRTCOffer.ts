import type SimplePeer from 'simple-peer'

import {SimulatorHMDLinkActionType} from '../constants/remoteActions'

/**
 * Triggered via window postMessage from XRHome.
 *
 * When XRHome wants to connect a simulator with a remote HMD, the HMD will make an offer.
 * This handler consumes that offer.
 */
const handleWebRtcOffer = (msg: MessageEvent, peerReceiver: SimplePeer): void => {
  const isOfferType = msg?.data?.action === SimulatorHMDLinkActionType.WEBRTC_OFFER

  if (isOfferType) {
    if (peerReceiver) {
      const payload = msg?.data?.data
      peerReceiver.signal(payload.webRTCData)
    }
  }
}

export {
  handleWebRtcOffer,
}

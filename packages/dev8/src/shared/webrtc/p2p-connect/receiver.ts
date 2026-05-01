import SimplePeer from 'simple-peer'

import {DEBUG} from '../constants/isDebug'

/**
 * Creates a new P2P receiver to get events from.
 */
const createNewP2PReceiver = ({
  onData,
  onSignal,
  onConnectionSuccess,
  onConnectionClosed,
  onConnectionError,
}) => {
  // Create a peer connection
  const peerReceiver = new SimplePeer({
    objectMode: true,
  })

  peerReceiver.on('signal', (data) => {
    if (DEBUG) {
      // eslint-disable-next-line no-console
      console.log('got offer, will answer')
    }
    onSignal(data)
  })

  peerReceiver.on('closed', () => {
    onConnectionClosed()
  })

  peerReceiver.on('error', () => {
    onConnectionError()
  })

  // On connect success, attach callback to run on data event
  peerReceiver.on('connect', () => {
    if (DEBUG) {
      // eslint-disable-next-line no-console
      console.log('connection successful')
    }
    onConnectionSuccess()
    peerReceiver.on('data', (data) => {
      onData(data)
    })
  })

  return peerReceiver
}

export {
  createNewP2PReceiver,
}

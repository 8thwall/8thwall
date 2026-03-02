import {XrDeviceFactory} from '../js-device'
import {createNativeRecorder} from './native-recorder'

// The direct recorder records directly to MP4 using the browser API, not needing a worker
// to transcode the video.

const createDirectRecorder = (audioMixer, compositor) => (
  createNativeRecorder(audioMixer, compositor, {
    mimeType: 'video/mp4',
    // We can't specify mimeType because if we do, Safari will not return data (iOS 14.4.1)
    specifyMimeType: false,
    // We must specify a "timeslice" because if we don't, Safari won't return data (iOS 14.4.1)
    timeSlice: 1000,
  })
)

// We only support the direct recorder on iOS due to quirks of the implementation.
const deviceSupportsDirectRecorder = () => (
  window.MediaRecorder &&
  window.MediaRecorder.isTypeSupported &&
  window.MediaRecorder.isTypeSupported('video/mp4') &&
  window.MediaRecorder.isTypeSupported('audio/mp4') &&
  XrDeviceFactory().deviceEstimate().os === 'iOS'
)

export {
  createDirectRecorder,
  deviceSupportsDirectRecorder,
}

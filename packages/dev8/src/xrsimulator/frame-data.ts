import {List, Message} from 'capnp-ts'

import {LogRecord} from '../capnp/c8/protolog/api/log-request.capnp'
import type {RawPositionalSensorValue} from '../capnp/reality/engine/api/request/sensor.capnp'

import {getMessageOffsets} from './message-parser'
import type {Rotation} from './motion-orientation-events'

type GpsData = {
  // time from the start of the recording
  timeNanos: number
  latitude: number
  longitude: number
  accuracy: number
}

type MotionOrientationData = {
  // time from the start of the recording
  timeNanos: number
  orientation: Rotation
  motionDataList: List<RawPositionalSensorValue>
}

const getMotionOrientation = (logRecord: LogRecord): MotionOrientationData => {
  const pose = logRecord.getRealityEngine()
    .getRequest()
    .getSensors()
    .getPose()

  const deviceWebOrientation = pose.getDeviceWebOrientation()

  const frame = logRecord.getRealityEngine()
    .getRequest()
    .getSensors()
    .getCamera()
    .getCurrentFrame()
  const timeNanos = frame.getTimestampNanos().toNumber()
  return {
    timeNanos,
    orientation: {
      alpha: deviceWebOrientation.getAlpha(),
      beta: deviceWebOrientation.getBeta(),
      gamma: deviceWebOrientation.getGamma(),
    },
    motionDataList: pose.getEventQueue(),
  }
}

const getFrameMotionOrientationData = (
  recordingData: ArrayBuffer, start: number, end: number
): MotionOrientationData => {
  const message = new Message(recordingData.slice(start, end), false)
  const logRecord = message.getRoot(LogRecord)
  return getMotionOrientation(logRecord)
}

const getGpsData = (logRecord: LogRecord): GpsData => {
  const frame = logRecord.getRealityEngine()
    .getRequest()
    .getSensors()
    .getCamera()
    .getCurrentFrame()

  const timeNanos = frame.getTimestampNanos().toNumber()

  const gps = logRecord.getRealityEngine()
    .getRequest()
    .getSensors()
    .getGps()

  return {
    timeNanos,
    latitude: gps.getLatitude(),
    longitude: gps.getLongitude(),
    accuracy: gps.getHorizontalAccuracy(),
  }
}

const getFrameGpsData = (recordingData: ArrayBuffer, start: number, end: number): GpsData => {
  const message = new Message(recordingData.slice(start, end), false)
  const logRecord = message.getRoot(LogRecord)

  return getGpsData(logRecord)
}

const fetchSequenceData = async (srcUrl: string) => {
  if (!srcUrl) {
    return null
  }

  try {
    const res = await fetch(srcUrl)
    return res.arrayBuffer()
  } catch (error) {
    // eslint-disable-next-line no-console
    console.error('FAILED TO FETCH', srcUrl, error)
  }

  return null
}

const makeVideoElement = (srcUrl: string): HTMLVideoElement => {
  const el = document.createElement('video')
  el.crossOrigin = 'anonymous'
  el.src = srcUrl
  el.setAttribute('width', '1')
  el.setAttribute('height', '1')
  el.setAttribute('autoplay', 'true')
  // Currently need playsInline for mobile Safari to not open the video in fullscreen by default
  el.playsInline = true
  el.muted = true
  el.style.display = 'none'
  return el
}

type LoadedFrameData = {
  videoEl: HTMLVideoElement | null

  // Motion and orientation data in increasing time order
  motionOrientation: MotionOrientationData[] | null

  // GPS data in increasing time order
  gpsData: GpsData[] | null
}

// We support three types of data
//  - only camera data via an mp4 (face effects not needing sensor)
//  - both camera data via an mp4 and sensor data via a LogRecord capnp file
//  - only sensor data via a LogRecord capnp file for rendering in simulator
const loadFrameData = async (
  cameraUrl?: string,
  sequenceUrl?: string
): Promise<LoadedFrameData> => {
  const sequenceData = sequenceUrl ? await fetchSequenceData(sequenceUrl) : null

  let motionOrientation: MotionOrientationData[] | null = null
  let gpsData: GpsData[] | null = null

  if (sequenceData) {
    // We have sensor data
    const messageBoundaries = getMessageOffsets(sequenceData)
    motionOrientation = messageBoundaries.map(
      ({start, end}) => getFrameMotionOrientationData(sequenceData, start, end)
    )
    gpsData = messageBoundaries.map(
      ({start, end}) => getFrameGpsData(sequenceData, start, end)
    )
  }

  const videoEl = cameraUrl ? makeVideoElement(cameraUrl) : null
  if (videoEl) {
    // Wait until we have frame data
    await new Promise<void>(
      resolve => videoEl.addEventListener('canplaythrough', () => (resolve()), {once: true})
    )
  }

  return {
    videoEl,
    motionOrientation,
    gpsData,
  }
}

export {
  loadFrameData,
}

export type {
  LoadedFrameData,
  MotionOrientationData,
  GpsData,
}

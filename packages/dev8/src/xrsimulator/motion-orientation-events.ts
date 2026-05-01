import {RawPositionalSensorValue} from '../capnp/reality/engine/api/request/sensor.capnp'
import type {MotionOrientationData} from './frame-data'

type Position = {
  x: number
  y: number
  z: number
}

type Rotation = {
  alpha: number
  beta: number
  gamma: number
}

type MotionDetails = {
  timeStamp: number
  interval: number
  acceleration?: Position
  accelerationIncludingGravity?: Position
  rotationRate?: Rotation
}

const emitOrientationData = (orientation: Rotation, timeStampNanos: number) => {
  if (!orientation) {
    return
  }

  const detail = {
    timeStamp: timeStampNanos / 1000000,
    ...orientation,
  }
  window.dispatchEvent(new CustomEvent('deviceorientation8w', {detail}))
}

// SLAM behavior is odd when a sequence does not have orientation data, so we emit fake data to
// prevent this. Values were chosen to be close to the orientation of a phone when held for filming
// in portrait mode, stationary, the screen facing the user, and top of the phone above the bottom
// Alpha and gamma don't have much effect since the phone is "stationary", so they're set to 0
// beta: 75 - motion around x-axis, device is mostly upright, slightly tilted forward away from user
const emitFakeOrientationData = (timeStampNanos: number) => {
  emitOrientationData({alpha: 0, beta: 75, gamma: 0}, timeStampNanos)
}

const emitMotionData = (positionalData: RawPositionalSensorValue, baseTimeOffset: number) => {
  const eventTimeNanos = positionalData.getEventTimestampNanos().toNumber()
  const shiftedEventTime = (eventTimeNanos + baseTimeOffset) / 1000000

  const kind = positionalData.getKind()
  const value = positionalData.getValue()
  const interval = positionalData.getIntervalNanos().toNumber() / 1000000

  const x = value.getX()
  const y = value.getY()
  const z = value.getZ()

  const {PositionalSensorKind} = RawPositionalSensorValue

  const detail: MotionDetails = {timeStamp: shiftedEventTime, interval}
  switch (kind) {
    case PositionalSensorKind.LINEAR_ACCELERATION:
      detail.acceleration = {x, y, z}
      break
    case PositionalSensorKind.ACCELEROMETER:
      detail.accelerationIncludingGravity = {x, y, z}
      break
    case PositionalSensorKind.GYROSCOPE:
      detail.rotationRate = {alpha: x, beta: y, gamma: z}
      break
    default:
      // eslint-disable-next-line no-console
      console.log('Incompatible positional sensor kind')
      return
  }
  const deviceMotionEvent = new CustomEvent('devicemotion8w', {detail})
  window.dispatchEvent(deviceMotionEvent)
}

const emitMotionQueueData = (motionData: RawPositionalSensorValue[], baseTimeOffset: number) => {
  for (let i = 0; i < motionData.length; i++) {
    emitMotionData(motionData[i], baseTimeOffset)
  }
}

// result is used to normalize the time of the sequence relative to the video time which assumes
// to starts at 0
const getBaseTimeOffset = (frame: MotionOrientationData): number => {
  if (!frame || frame.motionDataList.getLength() === 0) {
    throw new Error('Frame does not exist or has no motion data')
  }

  const positionalData = frame.motionDataList.get(0)
  const baseEventTimeNanos = positionalData.getEventTimestampNanos().toNumber()
  return -baseEventTimeNanos
}

// Find the frame closest to the loopStartFrame_ that has motion data
// to use as the base to calculate the baseTimeOffset_
// @return the frame number containing the base time. -1 if there is no motion data.
const getBaseTimeFrame = (
  frameDataList: MotionOrientationData[], startFrame: number, endFrame: number
): number => {
  if (!frameDataList || frameDataList.length === 0) {
    throw new Error('Frame data list does not exist or is empty')
  }

  for (let i = startFrame; i <= endFrame; i++) {
    if (frameDataList[i].motionDataList?.getLength()) {
      return i
    }
  }
  return -1  // If there is no motion data at all nothing will be emitted
}

type FoundMotionOrientationData = {
  timeNanos: number
  orientation: Rotation
  motionDataList: RawPositionalSensorValue[]
}

const makeMotionDataCursor = (data: MotionOrientationData[]) => {
  let currentIdx_ = data.length === 0 ? -1 : 0
  return {
    seek: (timeNanos: number): FoundMotionOrientationData | null => {
      if (currentIdx_ < 0 || currentIdx_ >= data.length) {
        // No data
        return null
      }

      if (timeNanos < data[currentIdx_].timeNanos) {
        // No data available yet
        return null
      }

      // Collect all the motion data that we have passed over
      const motionDataToSend: MotionOrientationData[] = []
      while (currentIdx_ < data.length && timeNanos >= data[currentIdx_].timeNanos) {
        motionDataToSend.push(data[currentIdx_])
        currentIdx_++
      }
      if (motionDataToSend.length > 0) {
        const motionDataList = ([] as RawPositionalSensorValue[]).concat(
          ...motionDataToSend.map(motionData => motionData.motionDataList.toArray())
        )
        return {
          timeNanos: motionDataToSend[motionDataToSend.length - 1].timeNanos,
          orientation: motionDataToSend[motionDataToSend.length - 1].orientation,
          motionDataList,
        }
      }
      return null
    },
    reset: () => {
      currentIdx_ = 0
    },
  }
}

type MotionDataCursor = ReturnType<typeof makeMotionDataCursor>

export {
  emitMotionQueueData,
  emitOrientationData,
  getBaseTimeOffset,
  getBaseTimeFrame,
  makeMotionDataCursor,
  emitFakeOrientationData,
  emitMotionData,
}

export type {
  MotionDataCursor,
  MotionDetails,
  Rotation,
}

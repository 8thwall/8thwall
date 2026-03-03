// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

import * as capnp from 'capnp-ts'

import type {XrTrackingccModule} from 'reality/app/xr/js/src/types/xrtrackingcc'

import {RawPositionalSensorValue, RequestPose} from 'reality/engine/api/request/sensor.capnp'

declare global {
  interface Window {
    XR8: any
  }
}

function Sensors(xrccPromise: Promise<XrTrackingccModule>) {
  const eventQueue = []
  let xrcc = null
  xrccPromise.then((xrcc_) => { xrcc = xrcc_ })

  const invalid = () => (!xrcc || !window.XR8 || window.XR8.isPaused())

  const setEventQueue = () => {
    if (invalid()) {
      return
    }

    const message = new capnp.Message()
    const builder = message.initRoot(RequestPose).initEventQueue(eventQueue.length)
    eventQueue.forEach((e, i) => {
      const b = builder.get(i)
      b.setKind(e.kind)
      b.setTimestampNanos(capnp.Int64.fromNumber(e.timestampNanos))
      b.setEventTimestampNanos(capnp.Int64.fromNumber(e.eventTimestampNanos))
      b.setIntervalNanos(capnp.Int64.fromNumber(e.intervalNanos))
      b.getValue().setX(e.x)
      b.getValue().setY(e.y)
      b.getValue().setZ(e.z)
    })
    eventQueue.length = 0

    const buffer = message.toArrayBuffer()
    const ptr = xrcc._malloc(buffer.byteLength)
    xrcc.writeArrayToMemory(new Uint8Array(buffer), ptr)
    xrcc._c8EmAsm_setEventQueue(ptr, buffer.byteLength)
    xrcc._free(ptr)
  }

  const updateDeviceMotion = ({acceleration, accelerationIncludingGravity, rotationRate, timeStamp, interval}) => {
    if (invalid()) {
      return
    }

    const {PositionalSensorKind} = RawPositionalSensorValue
    const timestampNanos = window.performance.now() * 1000 * 1000
    const eventTimestampNanos = timeStamp * 1000 * 1000
    const intervalNanos = interval * 1000 * 1000
    const addEvent = (kind, {x, y, z}) => eventQueue.push(
      {kind, timestampNanos, eventTimestampNanos, intervalNanos, x, y, z}
    )

    if (eventQueue.length && (timestampNanos - eventQueue[0].timestampNanos) > 5e9) {
      // clear the queue for large lag times
      eventQueue.length = 0
    }

    if (acceleration) {
      addEvent(PositionalSensorKind.LINEAR_ACCELERATION, acceleration)
    }
    if (accelerationIncludingGravity) {
      addEvent(PositionalSensorKind.ACCELEROMETER, accelerationIncludingGravity)
    }
    if (rotationRate) {
      addEvent(PositionalSensorKind.GYROSCOPE, {
        x: rotationRate.alpha,
        y: rotationRate.beta,
        z: rotationRate.gamma,
      })
    }
  }

  return {
    updateDeviceMotion,
    setEventQueue,
  }
}

export {
  Sensors,
}

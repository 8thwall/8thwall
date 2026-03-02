// eslint-disable-next-line import/no-unresolved
import {Message} from 'capnp-ts'
import {SemanticsOptions} from 'reality/engine/api/semantics.capnp'
import {CameraCoordinates} from 'reality/engine/api/base/camera-intrinsics.capnp'

import type {Quaternion} from '../quaternion'
import type {Coordinates, Pose, Position} from '../types/common'
import {
  DEFAULT_INVERT_LAYER_MASK,
  DEFAULT_EDGE_SMOOTHNESS,
  LayersControllerOptions,
} from '../types/layers'

// Gets the origin from coordinates, or sets defaults if position / rotation are missing.
// Set the default height to 2 to match engine.js which doesn't use 0 b/c otherwise it would
// lead to divide by 0 due to how responsive scale works (ground is at 0).
const getOrigin = (coordinates?: Coordinates): Pose => {
  const position: Position = {x: 0, y: 2, z: 0}
  const rotation: Quaternion = {w: 1, x: 0, y: 0, z: 0}
  if (coordinates?.origin?.position) {
    position.x = coordinates.origin.position.x
    position.y = coordinates.origin.position.y
    position.z = coordinates.origin.position.z
  }
  if (coordinates?.origin?.rotation) {
    rotation.w = coordinates.origin.rotation.w
    rotation.x = coordinates.origin.rotation.x
    rotation.y = coordinates.origin.rotation.y
    rotation.z = coordinates.origin.rotation.z
  }
  return {position, rotation}
}

const createSemanticsOptionsMsg = (
  outputWidth: number,
  outputHeight: number,
  haveSeenSky: boolean,
  options: LayersControllerOptions
) => {
  const msg = new Message()
  const opts = msg.initRoot(SemanticsOptions)
  opts.setOutputWidth(outputWidth)
  opts.setOutputHeight(outputHeight)
  opts.setNearClip(options.nearClip && options.nearClip > 0 ? options.nearClip : 0.01)
  opts.setFarClip(options.farClip && options.farClip > 0 ? options.farClip : 1000.0)
  const {axes, mirroredDisplay} = options.coordinates || {}
  const o = opts.getCoordinates().getOrigin()
  const origin = getOrigin(options.coordinates)
  o.getPosition().setX(origin.position.x)
  o.getPosition().setY(origin.position.y)
  o.getPosition().setZ(origin.position.z)
  o.getRotation().setW(origin.rotation.w)
  o.getRotation().setX(origin.rotation.x)
  o.getRotation().setY(origin.rotation.y)
  o.getRotation().setZ(origin.rotation.z)

  // Default mirrored display to false if not specified.
  opts.getCoordinates().setMirroredDisplay(!!mirroredDisplay)

  // We are in 'responsive' mode so set the scale to the height of the camera origin.
  opts.getCoordinates().setScale(o.getPosition().getY() > 0 ? o.getPosition().getY() : 1.0)

  opts.getCoordinates().setAxes(axes === 'LEFT_HANDED'
    ? CameraCoordinates.Axes.LEFT_HANDED
    : CameraCoordinates.Axes.RIGHT_HANDED)

  let invertLayerMask = DEFAULT_INVERT_LAYER_MASK
  let edgeSmoothness = DEFAULT_EDGE_SMOOTHNESS
  if (options && options.layers && options.layers.sky) {
    if (options.layers.sky.invertLayerMask !== null) {
      invertLayerMask = options.layers.sky.invertLayerMask
    }
    if (options.layers.sky.edgeSmoothness !== null) {
      edgeSmoothness = options.layers.sky.edgeSmoothness
    }
  }

  opts.getSkyOpts().setInvertAlphaMask(invertLayerMask)
  opts.getSkyOpts().setEdgeSmoothness(edgeSmoothness)
  opts.getSkyOpts().setHaveSeenSky(haveSeenSky)

  return msg
}

export {getOrigin, createSemanticsOptionsMsg}

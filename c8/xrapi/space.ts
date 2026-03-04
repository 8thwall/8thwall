import type {
  DOMPointReadOnly,
  XrPose,
  XrReferenceSpace,
  XrReferenceSpaceType,
  XrRigidTransform,
  XrSpace,
  XrSession,
  XrJointSpace,
  XrHandJoint,
  XrJointPose,
} from './xrapi-types'

import {
  XrRigidTransformImpl,
  multiplyTransform,
} from './transform'

type InternalXrSpace = XrSpace & {
  _session: XrSession
  _nativeOrigin: XrRigidTransform
  _originOffset: XrRigidTransform
  _effectiveOrigin: XrRigidTransform
}

type InternalXrReferenceSpace = XrReferenceSpace & InternalXrSpace & {
  _type: XrReferenceSpaceType
}

type InternalXrJointSpace = XrJointSpace & InternalXrSpace & {
  _radius: number
}

const updateSpaceOrigin = (space: XrSpace, origin: XrRigidTransform): void => {
  const spaceInternal = space as InternalXrSpace

  spaceInternal._nativeOrigin = origin

  // The effective origin can be obtained by multiplying origin offset and the native origin.
  spaceInternal._effectiveOrigin = multiplyTransform(
    spaceInternal._nativeOrigin, spaceInternal._originOffset
  )
}

const updateSpaceOriginPose = (space: XrSpace, pose: XrPose): void => {
  updateSpaceOrigin(space, pose.transform)
}

const updateSpaceOriginOffset = (
  space: XrSpace,
  position: DOMPointReadOnly,
  rotation: DOMPointReadOnly
): void => {
  const spaceInternal = space as InternalXrSpace

  spaceInternal._originOffset = new XrRigidTransformImpl(position, rotation)
}

const updateJointSpace = (space: XrJointSpace, pose: XrPose, radius: number): void => {
  updateSpaceOrigin(space, pose.transform)
  const spaceInternal = space as InternalXrJointSpace
  spaceInternal._radius = radius
}

const getRelativeTransform = (transform: XrRigidTransform, space: XrSpace): XrRigidTransform => {
  const spaceInternal = space as InternalXrSpace

  return multiplyTransform(spaceInternal._effectiveOrigin.inverse, transform)
}

const getEffectiveOrigin = (space: XrSpace): XrRigidTransform => {
  const spaceInternal = space as InternalXrSpace

  return spaceInternal._effectiveOrigin
}

// https://immersive-web.github.io/webxr/#populate-the-pose
const getPose = (space: XrSpace, baseSpace: XrSpace): XrPose => {
  const spaceInternal = space as InternalXrSpace
  const baseSpaceInternal = baseSpace as InternalXrSpace
  const outTransform = multiplyTransform(
    baseSpaceInternal._effectiveOrigin.inverse, spaceInternal._effectiveOrigin
  )

  return {
    transform: outTransform,
    linearVelocity: {x: 0, y: 0, z: 0, w: 0},  // TODO: Implement velocity here and in native
    angularVelocity: {x: 0, y: 0, z: 0, w: 0},
  }
}

const getJointPose = (joint: XrJointSpace, baseSpace: XrSpace): XrJointPose => {
  const jointInternal = joint as InternalXrJointSpace
  const baseSpaceInternal = baseSpace as InternalXrSpace
  const outTransform = multiplyTransform(
    baseSpaceInternal._effectiveOrigin.inverse, jointInternal._effectiveOrigin
  )

  return {
    transform: outTransform,
    linearVelocity: {x: 0, y: 0, z: 0, w: 0},
    angularVelocity: {x: 0, y: 0, z: 0, w: 0},
    radius: jointInternal._radius,
  }
}

// IMPLEMENTATION NOTE:
// This doesn't use a nativeOrigin, store an XrPose in a WeakMap using the XrSpace as the key.
const applyPropertiesToXrSpace = (
  session: XrSession
): XrSpace => ({
  _session: session,

  _nativeOrigin: new XrRigidTransformImpl(),

  _originOffset: new XrRigidTransformImpl(),

  _effectiveOrigin: new XrRigidTransformImpl(),
})

const applyPropertiesToXrJointSpace = (
  session: XrSession, jointName: XrHandJoint
): InternalXrJointSpace => ({
  jointName,

  _radius: 0,

  _session: session,

  _nativeOrigin: new XrRigidTransformImpl(),

  _originOffset: new XrRigidTransformImpl(),

  _effectiveOrigin: new XrRigidTransformImpl(),
})

const createXrSpace = (
  session: XrSession
): XrSpace => applyPropertiesToXrSpace(session)

const createXrJointSpace = (
  session: XrSession, jointName: XrHandJoint
): XrJointSpace => applyPropertiesToXrJointSpace(session, jointName)

const createXrReferenceSpace = (
  session: XrSession,
  type: XrReferenceSpaceType
): XrReferenceSpace => {
  const baseSpace = applyPropertiesToXrSpace(session) as InternalXrSpace

  return {
    _session: baseSpace._session,

    _nativeOrigin: baseSpace._nativeOrigin,

    _originOffset: baseSpace._originOffset,

    _effectiveOrigin: baseSpace._effectiveOrigin,

    _type: type,

    // https://immersive-web.github.io/webxr/#dom-xrreferencespace-getoffsetreferencespace
    getOffsetReferenceSpace: (originOffset: XrRigidTransform): XrReferenceSpace => {
      // 1. Let base be the XRReferenceSpace the method was called on.
      // [IMPLEMENTATION NOTE] 'this' is baseSpace

      // 2. Initialize offsetSpace as follows:
      // 2.1 If base is an instance of XRBoundedReferenceSpace:
      // [NOT IMPLEMENTED]

      // 2.2 Otherwise: Let offsetSpace be a new XRReferenceSpace in the relevant realm of base.
      const offsetSpace = createXrReferenceSpace(session, type) as InternalXrReferenceSpace

      // 3. Set offsetSpace’s type to base’s type.
      // [IMPLEMENTATION NOTE] Already set in createXrReferenceSpace

      // 4. Set offsetSpace’s origin offset to the result of multiplying base’s origin offset
      // by originOffset in the relevant realm of base.
      offsetSpace._originOffset = multiplyTransform(baseSpace._originOffset, originOffset)

      // [IMPLEMENTATION NOTE] Update the effective origin of the reference space
      // Assuming native origin is identity
      offsetSpace._effectiveOrigin = offsetSpace._originOffset

      // 5. Return offsetSpace.
      return offsetSpace
    },
  } as InternalXrReferenceSpace
}

export {
  createXrSpace,
  createXrReferenceSpace,
  updateSpaceOriginPose,
  updateSpaceOriginOffset,
  getRelativeTransform,
  getPose,
  getEffectiveOrigin,
  createXrJointSpace,
  updateJointSpace,
  getJointPose,
}

// Helper for extracting a rotation from a TRS matrix with scale. This reuses the same vector
// objects for translation and scale across calls so that only quaternion is reallocated on each
// call.
const rotationForMatrixGen = () => {
  const unusedT = new window.THREE.Vector3()
  const unusedS = new window.THREE.Vector3()
  return (m) => {
    const q = new window.THREE.Quaternion()
    m.decompose(unusedT, q, unusedS)
    return q
  }
}

// Helper for extracting a rotation from a vector. This reuses the same vector objects for
// the forward vector across calls.
const rotationForUnitVectorGen = () => {
  const forward = new window.THREE.Vector3(0, 0, -1)
  return v => new window.THREE.Quaternion().setFromUnitVectors(forward, v)
}

// Helper for setting the position of an object in world space.
const setWorldTransformGen = () => {
  const {THREE} = window
  const parentMatrixWorldInverse_ = new THREE.Matrix4()
  return (obj, {position, quaternion, scale}) => {
    obj.matrixWorld.compose(position, quaternion, scale)
    if (obj.parent) {
      if (parentMatrixWorldInverse_.invert) {
        // THREE 123 preferred version
        parentMatrixWorldInverse_.copy(obj.parent.matrixWorld).invert()
      } else {
        // Backwards compatible version
        parentMatrixWorldInverse_.getInverse(obj.parent.matrixWorld)
      }
      obj.matrix.copy(obj.matrixWorld).premultiply(parentMatrixWorldInverse_)
      obj.matrix.decompose(obj.position, obj.quaternion, obj.scale)
      obj.updateMatrixWorld(true)
    } else {
      obj.matrix.copy(obj.matrixWorld)
      obj.matrix.decompose(obj.position, obj.quaternion, obj.scale)
      obj.updateMatrixWorld(true)
    }
  }
}

export {
  rotationForMatrixGen,
  rotationForUnitVectorGen,
  setWorldTransformGen,
}

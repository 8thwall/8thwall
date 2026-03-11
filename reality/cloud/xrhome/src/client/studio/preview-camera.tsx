import React from 'react'
import {OrbitControls, PerspectiveCamera} from '@react-three/drei'
import {degreesToRadians} from '@ecs/shared/angle-conversion'
import {Vector3, Box3, Object3D} from 'three'
import {useThree} from '@react-three/fiber'

const FOV_HEIGHT_UNIT = 2 * Math.tan(degreesToRadians(50) / 2)
const VIEW_ASPECT_RATIO = 1
const ZOOM_SCALE = 1.1
const POV_DIRECTION = new Vector3(0, 0.2, 1)
const DEFAULT_CAMERA_STATE = {position: new Vector3(0, 0, 5), target: new Vector3(0, 0, 0)}

const getInitialCameraPosition = (object: Object3D) => {
  if (!object) {
    return DEFAULT_CAMERA_STATE
  }

  // NOTE(owen): setFromObject calls updateWorldMatrix on the object, which should be the same as
  // calling updateMatrixWorld, except for SkinnedMesh objects. They override updateMatrixWorld
  // to update some internals, but do not override updateWorldMatrix for some reason.
  object.updateMatrixWorld(true)
  // Compute the bounding box of the object and its children
  const boundingBox = new Box3().setFromObject(object)
  const objectCenter = new Vector3()
  boundingBox.getCenter(objectCenter)

  const size = boundingBox.getSize(new Vector3())
  const scale = size.length() || 1

  const distance = (scale * ZOOM_SCALE) / (FOV_HEIGHT_UNIT * VIEW_ASPECT_RATIO)
  const cameraPosition = POV_DIRECTION.clone().multiplyScalar(distance).add(objectCenter)

  return {
    position: new Vector3(cameraPosition.x, cameraPosition.y, cameraPosition.z),
    target: new Vector3(objectCenter.x, objectCenter.y, objectCenter.z),
  }
}

interface IPreviewCameraComponent {
  model?: Object3D
  sticky?: boolean
}

const PreviewCameraComponent: React.FC<IPreviewCameraComponent> = ({model, sticky}) => {
  const scene = useThree(state => state.scene)

  // use memo could not be used here because the models in the studio scene
  // would load after the camera is set
  const [cameraState, setCameraState] = React.useState(DEFAULT_CAMERA_STATE)

  // If camera is sticky, don't change if the scene/model change
  const [cameraStateSet, setCameraStateSet] = React.useState(false)

  React.useLayoutEffect(() => {
    if (sticky && cameraStateSet) {
      return
    }

    // Some scenes contain gizmos and indicators that are not part of the main object
    // in these cases passing the scene (object of interest) directly is needed
    const obj3d = model || scene
    setCameraState(getInitialCameraPosition(obj3d))
    // Only set when sticky is true
    setCameraStateSet(sticky && !!obj3d)
  }, [scene, model])

  return (
    <>
      <PerspectiveCamera
        makeDefault
        position={cameraState.position}
        near={0.1}
        far={2500}
      />
      <OrbitControls
        target={cameraState.target}
        enableDamping
        rotateSpeed={0.5}
        minPolarAngle={degreesToRadians(10)}
        maxPolarAngle={degreesToRadians(170)}
      />
    </>
  )
}

export {
  getInitialCameraPosition,
  PreviewCameraComponent,
}

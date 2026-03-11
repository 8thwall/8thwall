import React, {useMemo, useRef} from 'react'
import type {Camera as CameraConfig} from '@ecs/shared/scene-graph'
import {
  OrthographicCamera, CameraHelper, PerspectiveCamera,
} from 'three'
import {useHelper} from '@react-three/drei'
import type {DeepReadonly} from 'ts-essentials'
import {CAMERA_DEFAULTS} from '@ecs/shared/camera-constants'

const PerspectiveCameraComponent: React.FC<
{isSelected: boolean, cameraConfig: DeepReadonly<CameraConfig>}
> = ({isSelected, cameraConfig}) => {
  const cameraRef = useRef<PerspectiveCamera>()
  React.useEffect(() => cameraRef.current?.updateProjectionMatrix(), [cameraConfig])
  useHelper(isSelected ? cameraRef : null, CameraHelper)

  return (
    <perspectiveCamera
      ref={cameraRef}
      rotation={[0, Math.PI, 0]}
      fov={cameraConfig.fov > 0 ? cameraConfig.fov : CAMERA_DEFAULTS.fov}
      zoom={cameraConfig.zoom > 0 ? cameraConfig.zoom : 1}
      near={cameraConfig.nearClip ?? CAMERA_DEFAULTS.nearClip}
      far={cameraConfig.farClip ?? CAMERA_DEFAULTS.farClip}
    />
  )
}

const OrthographicCameraComponent: React.FC<
{isSelected: boolean, cameraConfig: DeepReadonly<CameraConfig>}
> = ({isSelected, cameraConfig}) => {
  const cameraRef = useRef<OrthographicCamera>()
  React.useEffect(() => cameraRef.current?.updateProjectionMatrix(), [cameraConfig])
  useHelper(isSelected ? cameraRef : null, CameraHelper)

  return (
    <orthographicCamera
      ref={cameraRef}
      rotation={[0, Math.PI, 0]}
      left={cameraConfig.left}
      right={cameraConfig.right}
      top={cameraConfig.top}
      bottom={cameraConfig.bottom}
      zoom={cameraConfig.zoom > 0 ? cameraConfig.zoom : 1}
      near={cameraConfig.nearClip ?? CAMERA_DEFAULTS.nearClip}
      far={cameraConfig.farClip ?? CAMERA_DEFAULTS.farClip}
    />
  )
}

interface ICamera {
  baseCamera?: DeepReadonly<CameraConfig>
  isSelected: boolean
}

const Camera: React.FC<ICamera> = ({baseCamera, isSelected}) => {
  const cameraConfig = useMemo(() => (
    {...CAMERA_DEFAULTS, ...baseCamera}
  ), [baseCamera])
  switch (cameraConfig.type) {
    case 'perspective':
      return (
        <PerspectiveCameraComponent isSelected={isSelected} cameraConfig={baseCamera} />
      )
    case 'orthographic':
      return (
        <OrthographicCameraComponent isSelected={isSelected} cameraConfig={baseCamera} />
      )
    default:
      // eslint-disable-next-line no-console
      console.error('Unknown Camera', cameraConfig.type)
      return null
  }
}

export {
  Camera,
}

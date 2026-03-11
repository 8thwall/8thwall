import React from 'react'

import {Sprite, TextureLoader, Vector3} from 'three'

import type {GraphObject, SceneGraph} from '@ecs/shared/scene-graph'

import type {DeepReadonly} from 'ts-essentials'

import {useFrame, useThree} from '@react-three/fiber'

import cameraIcon from '../static/camera.png'
import audioIcon from '../static/audio.png'
import pointLightIcon from '../static/point-light.png'
import directionalLightIcon from '../static/directional-light.png'
import ambientLightIcon from '../static/ambient-light.png'
import spotLightIcon from '../static/spot-light.png'
import areaLightIcon from '../static/area-light.png'
import splatIcon from '../static/splat.png'
import locationIcon from '../static/location.png'
import {calculateDescalingFactor} from './scale'

const ICONS = {
  camera: cameraIcon,
  audio: audioIcon,
  pointLight: pointLightIcon,
  directionalLight: directionalLightIcon,
  ambientLight: ambientLightIcon,
  spotLight: spotLightIcon,
  areaLight: areaLightIcon,
  splat: splatIcon,
  location: locationIcon,
}

const getIconUrl = (object: DeepReadonly<GraphObject>) => {
  if (object.camera) {
    return ICONS.camera
  } else if (object.light) {
    if (object.light.type === 'directional') {
      return ICONS.directionalLight
    } else if (object.light.type === 'point') {
      return ICONS.pointLight
    } else if (object.light.type === 'spot') {
      return ICONS.spotLight
    } else if (object.light.type === 'area') {
      return ICONS.areaLight
    } else {
      return ICONS.ambientLight
    }
  } else if (object.splat) {
    return ICONS.splat
  } else if (object.audio && !(object.gltfModel || object.geometry)) {
    return ICONS.audio
  } else if (object.location) {
    return ICONS.location
  }
  return null
}

const ViewportIcon: React.FC<
{ object: DeepReadonly<GraphObject>
  scene: DeepReadonly<SceneGraph> }
> = (
  {object, scene}
) => {
  const url = getIconUrl(object)
  const texture = React.useMemo(() => url && new TextureLoader().load(url), [url])
  const {camera} = useThree()
  const spriteRef = React.useRef<Sprite>()
  const spritePosition = new Vector3()
  const descalingFactor = React.useMemo(() => calculateDescalingFactor(object.id, scene),
    [object, scene])

  useFrame(() => {
    if (spriteRef.current) {
      spriteRef.current.getWorldPosition(spritePosition)
      if (object.splat) {
        // Splats have a opacity that is based on the distance to the camera
        spriteRef.current.material.opacity =
          Math.max(0.15, Math.min(0.6, camera.position.distanceTo(spritePosition) / 10))
      }
      const scaledDistance = Math.min(1, camera.position.distanceTo(spritePosition) / 10)
      spriteRef.current.scale.set(
        (0.5 / descalingFactor[0]) * scaledDistance,
        (0.5 / descalingFactor[1]) * scaledDistance,
        (0.5 / descalingFactor[2]) * scaledDistance
      )
    }
  })

  if (url && texture) {
    return (
      <sprite ref={spriteRef} castShadow={false} renderOrder={Number.MAX_SAFE_INTEGER}>
        <spriteMaterial
          map={texture}
        />
      </sprite>
    )
  }

  return null
}

export default ViewportIcon

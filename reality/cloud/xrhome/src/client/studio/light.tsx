import React, {useLayoutEffect, useMemo, useRef} from 'react'
import type {OrthographicCameraProps} from '@react-three/fiber'
import type {DeepReadonly} from 'ts-essentials'
import type {Light as LightConfig} from '@ecs/shared/scene-graph'
import {
  DirectionalLightHelper, type DirectionalLight, CameraHelper,
  PointLight, PointLightHelper, PerspectiveCamera, type SpotLight, SpotLightHelper, Object3D,
} from 'three'
import {DEFAULT_COLOR, LIGHT_DEFAULTS} from '@ecs/shared/light-constants'
import {Edges, useHelper} from '@react-three/drei'

import {useTexture} from './hooks/use-texture'

const LIGHT_CONFIG_DEFAULTS = {
  castShadow: LIGHT_DEFAULTS.castShadow,
  color: DEFAULT_COLOR,
  intensity: LIGHT_DEFAULTS.intensity,
  target: [LIGHT_DEFAULTS.targetX, LIGHT_DEFAULTS.targetY, LIGHT_DEFAULTS.targetZ],
  shadowBias: LIGHT_DEFAULTS.shadowBias,
  shadowNormalBias: LIGHT_DEFAULTS.shadowNormalBias,
  shadowRadius: LIGHT_DEFAULTS.shadowRadius,
  shadowAutoUpdate: LIGHT_DEFAULTS.shadowAutoUpdate,
  shadowBlurSamples: LIGHT_DEFAULTS.shadowBlurSamples,
  shadowMapSize: [LIGHT_DEFAULTS.shadowMapSizeWidth, LIGHT_DEFAULTS.shadowMapSizeHeight],
  shadowCamera: [LIGHT_DEFAULTS.shadowCameraLeft, LIGHT_DEFAULTS.shadowCameraRight,
    LIGHT_DEFAULTS.shadowCameraTop, LIGHT_DEFAULTS.shadowCameraBottom,
    LIGHT_DEFAULTS.shadowCameraNear, LIGHT_DEFAULTS.shadowCameraFar],
} as const

interface ILightComponent {
  lightConfig: DeepReadonly<LightConfig>
  showHelpers: boolean
  intensity: number
}

const DirectionalLightComponent: React.FC<ILightComponent> = (
  {lightConfig, showHelpers, intensity}
) => {
  const lightRef = useRef<DirectionalLight>()
  useHelper(showHelpers && lightRef, DirectionalLightHelper)

  // NOTE (jeffha): need to manually handle our own camera helper to ensure it gets properly
  //                updated when the frustum size changes from the user end
  const [cameraHelper, setCameraHelper] = React.useState<CameraHelper>(null)

  // default to regular shadow area size if smart shadows are enabled.
  // in future, have smart shadows intelligently choose size
  const shadowFrustumSize: OrthographicCameraProps['args'] = lightConfig.followCamera
    ? [
      LIGHT_DEFAULTS.shadowCameraLeft,
      LIGHT_DEFAULTS.shadowCameraRight,
      LIGHT_DEFAULTS.shadowCameraTop,
      LIGHT_DEFAULTS.shadowCameraBottom,
      LIGHT_DEFAULTS.shadowCameraNear,
      LIGHT_DEFAULTS.shadowCameraFar,
    ]
    : [
      lightConfig.shadowCamera[0],
      lightConfig.shadowCamera[1],
      lightConfig.shadowCamera[2],
      lightConfig.shadowCamera[3],
      lightConfig.shadowCamera[4],
      lightConfig.shadowCamera[5],
    ]

  useLayoutEffect(() => {
    if (lightRef.current) {
      lightRef.current.target.position.set(lightConfig.target[0],
        lightConfig.target[1], lightConfig.target[2])
      lightRef.current.shadow.bias = lightConfig.shadowBias
      lightRef.current.shadow.normalBias = lightConfig.shadowNormalBias
      lightRef.current.shadow.radius = lightConfig.shadowRadius
      lightRef.current.shadow.autoUpdate = lightConfig.shadowAutoUpdate
      lightRef.current.shadow.blurSamples = lightConfig.shadowBlurSamples
      lightRef.current.target.updateMatrixWorld()

      // force a shadow map update (needed for map size change)
      if (
        lightRef.current.shadow.mapSize.width !== lightConfig.shadowMapSize[0] ||
        lightRef.current.shadow.mapSize.height !== lightConfig.shadowMapSize[1]
      ) {
        lightRef.current.shadow.mapSize.set(lightConfig.shadowMapSize[0],
          lightConfig.shadowMapSize[1])
        lightRef.current.shadow.map?.dispose()
        lightRef.current.shadow.map = null
      }

      const [left, right, top, bottom, near, far] = shadowFrustumSize
      lightRef.current.shadow.camera.left = left
      lightRef.current.shadow.camera.right = right
      lightRef.current.shadow.camera.top = top
      lightRef.current.shadow.camera.bottom = bottom
      lightRef.current.shadow.camera.near = near
      lightRef.current.shadow.camera.far = far
      lightRef.current.shadow.camera.updateProjectionMatrix()
      cameraHelper?.update()
    }
  }, [lightConfig, shadowFrustumSize])

  React.useEffect(() => {
    if (showHelpers && lightRef.current && !cameraHelper) {
      setCameraHelper(new CameraHelper(lightRef.current.shadow.camera))
    }

    return () => {
      if (cameraHelper) {
        cameraHelper.dispose()
        setCameraHelper(null)
      }
    }
  }, [showHelpers, lightRef, cameraHelper])

  return (
    <>
      <directionalLight
        ref={lightRef}
        color={lightConfig.color}
        castShadow={lightConfig.castShadow}
        intensity={intensity}
        position={[0, 0, 0]}
      />
      {cameraHelper && <primitive object={cameraHelper} />}
    </>
  )
}

const PointLightComponent: React.FC<ILightComponent> = (
  {lightConfig, showHelpers, intensity}
) => {
  const lightRef = useRef<PointLight>()
  const shadowCameraRef = useRef<PerspectiveCamera>()
  useHelper(showHelpers ? lightRef : null, PointLightHelper)
  useHelper(showHelpers ? shadowCameraRef : null, CameraHelper)

  useLayoutEffect(() => {
    if (lightRef.current) {
      lightRef.current.castShadow = lightConfig.castShadow
      lightRef.current.shadow.bias = lightConfig.shadowBias
      lightRef.current.shadow.normalBias = lightConfig.shadowNormalBias
      lightRef.current.shadow.radius = lightConfig.shadowRadius
      lightRef.current.shadow.autoUpdate = lightConfig.shadowAutoUpdate
      lightRef.current.shadow.blurSamples = lightConfig.shadowBlurSamples

      if (
        lightRef.current.shadow.mapSize.width !== lightConfig.shadowMapSize[0] ||
        lightRef.current.shadow.mapSize.height !== lightConfig.shadowMapSize[1]
      ) {
        lightRef.current.shadow.mapSize.set(lightConfig.shadowMapSize[0],
          lightConfig.shadowMapSize[1])
        lightRef.current.shadow.map?.dispose()
        lightRef.current.shadow.map = null
      }
    }
  }, [lightConfig])

  return (
    <pointLight
      ref={lightRef}
      color={lightConfig.color}
      castShadow={lightConfig.castShadow}
      intensity={intensity}
      decay={lightConfig.decay}
      distance={lightConfig.distance}
      position={[0, 0, 0]}
      rotation={[0, Math.PI, 0]}
    >
      <perspectiveCamera
        ref={shadowCameraRef}
        attach='shadow-camera'
        near={lightConfig.shadowCamera[4]}
        far={lightConfig.shadowCamera[5]}
      />
    </pointLight>
  )
}

const SpotLightComponent: React.FC<ILightComponent> = (
  {lightConfig, showHelpers, intensity}
) => {
  const lightRef = useRef<SpotLight>()
  const shadowCameraRef = useRef<PerspectiveCamera>()
  const texture = useTexture(lightConfig.colorMap)

  useHelper(showHelpers ? lightRef : null, SpotLightHelper)
  useHelper(showHelpers ? shadowCameraRef : null, CameraHelper)

  useLayoutEffect(() => {
    if (lightRef.current) {
      lightRef.current.castShadow = lightConfig.castShadow
      lightRef.current.shadow.bias = lightConfig.shadowBias
      lightRef.current.shadow.normalBias = lightConfig.shadowNormalBias
      lightRef.current.shadow.radius = lightConfig.shadowRadius
      lightRef.current.shadow.autoUpdate = lightConfig.shadowAutoUpdate
      lightRef.current.shadow.blurSamples = lightConfig.shadowBlurSamples
      lightRef.current.map = texture

      if (
        lightRef.current.shadow.mapSize.width !== lightConfig.shadowMapSize[0] ||
        lightRef.current.shadow.mapSize.height !== lightConfig.shadowMapSize[1]
      ) {
        lightRef.current.shadow.mapSize.set(lightConfig.shadowMapSize[0],
          lightConfig.shadowMapSize[1])
        lightRef.current.shadow.map?.dispose()
        lightRef.current.shadow.map = null
      }
    }
  }, [lightConfig, texture])

  const target = React.useMemo(() => {
    const object = new Object3D()
    object.position.z = 1
    return object
  }, [])

  return (
    <spotLight
      ref={lightRef}
      color={lightConfig.color}
      castShadow={lightConfig.castShadow}
      intensity={intensity}
      decay={lightConfig.decay}
      distance={lightConfig.distance}
      angle={lightConfig.angle}
      penumbra={lightConfig.penumbra}
      target={target}
    >
      <perspectiveCamera
        ref={shadowCameraRef}
        attach='shadow-camera'
        near={lightConfig.shadowCamera[4]}
        far={lightConfig.shadowCamera[5]}
      />
      <primitive object={target} />
    </spotLight>
  )
}

const AreaLightComponent: React.FC<ILightComponent> = (
  {lightConfig, showHelpers, intensity}
) => {
  const width = lightConfig.width ?? LIGHT_DEFAULTS.width
  const height = lightConfig.height ?? LIGHT_DEFAULTS.height

  return (
    <group>
      <rectAreaLight
        width={width}
        height={height}
        intensity={intensity}
        color={lightConfig.color}
      />

      {showHelpers && (
        <mesh>
          <planeGeometry args={[width, height]} />
          <meshBasicMaterial visible={false} />
          <Edges color={lightConfig.color} />
        </mesh>
      )}
    </group>
  )
}

interface ILight {
  baseLightConfig: DeepReadonly<LightConfig>
  showHelpers: boolean
  disableLight?: boolean
}

const Light: React.FC<ILight> = ({baseLightConfig, showHelpers, disableLight}) => {
  const lightConfig = useMemo(() => (
    {...LIGHT_CONFIG_DEFAULTS, ...baseLightConfig}
  ), [baseLightConfig])

  const intensity = disableLight ? 0 : lightConfig.intensity * Math.PI

  switch (lightConfig.type) {
    case 'directional':
      return (
        <DirectionalLightComponent
          lightConfig={lightConfig}
          showHelpers={showHelpers}
          intensity={intensity}
        />
      )
    case 'point':
      return (
        <PointLightComponent
          lightConfig={lightConfig}
          showHelpers={showHelpers}
          intensity={intensity}
        />
      )
    case 'ambient':
      return (
        <ambientLight
          color={lightConfig.color}
          intensity={intensity}
        />
      )
    case 'spot':
      return (
        <SpotLightComponent
          lightConfig={lightConfig}
          showHelpers={showHelpers}
          intensity={intensity}
        />
      )
    case 'area':
      return (
        <AreaLightComponent
          lightConfig={lightConfig}
          intensity={intensity}
          showHelpers={showHelpers}
        />
      )
    default:
      // eslint-disable-next-line no-console
      console.error('Unknown Light', lightConfig.type)
      return null
  }
}

export {
  Light,
}

import THREE from './three'
import {Light} from './components'
import type * as THREE_TYPES from './three-types'
import {assets} from './assets'
import type {World} from './world'
import type {Eid, ReadData} from '../shared/schema'
import type {Lights, ShadowLights} from './lights-types'
import type {SchemaOf} from './world-attribute'
import {addChild} from './matrix-refresh'

type LightConfig = ReadData<SchemaOf<typeof Light>>

const sameLightType = (object: Lights, type: string) => {
  switch (type) {
    case 'directional':
      return object instanceof THREE.DirectionalLight
    case 'ambient':
      return object instanceof THREE.AmbientLight
    case 'point':
      return object instanceof THREE.PointLight
    case 'spot':
      return object instanceof THREE.SpotLight
    case 'area':
      return object instanceof THREE.RectAreaLight
    default:
      throw new Error('Unexpected light type')
  }
}

const editOrthographicShadowCamera = (
  object: THREE_TYPES.DirectionalLight, config: LightConfig
) => {
  object.shadow.camera.left = config.shadowCameraLeft
  object.shadow.camera.right = config.shadowCameraRight
  object.shadow.camera.top = config.shadowCameraTop
  object.shadow.camera.bottom = config.shadowCameraBottom
  object.shadow.camera.near = config.shadowCameraNear
  object.shadow.camera.far = config.shadowCameraFar
  object.shadow.camera.updateProjectionMatrix()
}

const editLightShadow = (object: ShadowLights, config: LightConfig) => {
  object.castShadow = config.castShadow
  object.shadow.normalBias = config.shadowNormalBias
  object.shadow.bias = config.shadowBias
  object.shadow.radius = config.shadowRadius
  object.shadow.autoUpdate = config.shadowAutoUpdate
  object.shadow.blurSamples = config.shadowBlurSamples

  // force a shadow map update (needed for map size change)
  if (
    object.shadow.mapSize.width !== config.shadowMapSizeWidth ||
    object.shadow.mapSize.height !== config.shadowMapSizeHeight
  ) {
    object.shadow.mapSize.set(config.shadowMapSizeWidth, config.shadowMapSizeHeight)
    object.shadow.map?.dispose()
    object.shadow.map = null
  }
}

const editLightObject = (world: World, eid: Eid, object: Lights, config: LightConfig) => {
  if (object instanceof THREE.DirectionalLight) {
    if (!config.followCamera) {
      object.target.position.set(config.targetX, config.targetY, config.targetZ)
      object.target.updateMatrixWorld()
      editOrthographicShadowCamera(object, config)
    }

    object.castShadow = config.castShadow
    editLightShadow(object, config)
  } else if (object instanceof THREE.PointLight) {
    object.distance = config.distance
    object.decay = config.decay
    editLightShadow(object, config)
  } else if (object instanceof THREE.AmbientLight) {
    // Do nothing
  } else if (object instanceof THREE.SpotLight) {
    object.distance = config.distance
    object.decay = config.decay
    object.angle = config.angle
    object.penumbra = config.penumbra

    if (config.colorMap) {
      const url = config.colorMap
      assets.load({url}).then((asset) => {
        if (!Light.has(world, eid)) {
          return
        }

        // get the updated light cursor in case it has changed
        const lightObj = Light.get(world, eid)

        // to avoid a race condition where this loaded asset is no longer the current map asset
        if (url === lightObj.colorMap) {
          object.map = new THREE.TextureLoader().load(asset.localUrl)
        }
      })
    }

    editLightShadow(object, config)
  } else if (object instanceof THREE.RectAreaLight) {
    object.width = config.width
    object.height = config.height
  } else {
    throw new Error('Unexpected light type')
  }
  // NOTE(johnny): https://discourse.threejs.org/t/updates-to-lighting-in-three-js-r155/53733
  object.intensity = config.intensity * Math.PI
  object.color.setRGB(config.r / 255, config.g / 255, config.b / 255)
  object.color.convertSRGBToLinear()
}

const createLightObject = (world: World, eid: Eid, config: LightConfig): Lights => {
  let lightObject: Lights
  switch (config.type) {
    case 'directional':
      lightObject = new THREE.DirectionalLight()
      break
    case 'ambient':
      lightObject = new THREE.AmbientLight()
      break
    case 'point':
      lightObject = new THREE.PointLight()
      break
    case 'spot':
      lightObject = new THREE.SpotLight()
      lightObject.target.position.z = 1
      addChild(lightObject, lightObject.target)
      break
    case 'area':
      lightObject = new THREE.RectAreaLight()
      break
    default:
      throw new Error('Unexpected light type')
  }
  // NOTE(christoph): Directional and spotlights default to y=1
  lightObject.position.set(0, 0, 0)
  editLightObject(world, eid, lightObject, config)
  return lightObject
}

export {
  createLightObject,
  editLightObject,
  sameLightType,
}

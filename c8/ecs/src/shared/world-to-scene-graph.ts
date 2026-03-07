import type {DeepReadonly} from 'ts-essentials'

import type {Ecs} from '../runtime/index'
import type {
  EntityReference, GraphObject, LightType, MaterialBlending, SceneGraph, Side,
  DistanceModel, UiGraphSettings, XrCameraType, DeviceSupportType, CameraDirectionType, CameraType,
  Faces, LocationVisualization, TextureWrap,
} from './scene-graph'
import type {Eid, ReadData, WriteData} from './schema'
import type {SchemaOf} from '../runtime/world-attribute'
import type {SceneHandle} from '../runtime/scene-types'
import type {IApplication} from './application-type'
import {inferResourceObject, inferFontResourceObject} from './resource'
import type {UiRuntimeSettings} from '../runtime/ui-types'
import {resolveSpaceForObject} from './object-hierarchy'
import type {World} from '../runtime/world'
import {UI_DEFAULTS} from './ui-constants'

const resolveId = (sceneHandle: SceneHandle, eid: Eid): string | undefined => {
  const originalObject = sceneHandle.eidToObject.get(eid)
  if (originalObject) {
    return originalObject.id
  }
  if (!eid) {
    return undefined
  }
  return eid.toString()
}

const resolveEntityReference = (
  sceneHandle: SceneHandle, eid: Eid
): EntityReference | undefined => {
  const id = resolveId(sceneHandle, eid)
  if (!id) {
    return undefined
  }
  return {type: 'entity', id}
}

type ResolvedEntitySchema<T extends {}> = {
  [K in keyof T]: T[K] extends Eid ? EntityReference | undefined : T[K]
}

const resolveEntityReferences = <T extends {}>(sceneHandle: SceneHandle, data: ReadData<T>) => {
  const res = {} as any
  Object.entries(data).forEach(([key, value]) => {
    if (typeof value === 'bigint') {
      const reference = resolveEntityReference(sceneHandle, value)
      if (reference) {
        res[key] = reference
      }
    } else {
      res[key] = value
    }
  })
  return res as ResolvedEntitySchema<ReadData<T>>
}

const colorToString = (r: number, g: number, b: number) => `#${
  [r, g, b].map(e => e.toString(16).padStart(2, '0')).join('')}`

// If the component is stored on the GraphObject directly instead of in the components map, remap
// here.
const maybeDirectAssignComponent = (
  ecs: Ecs, app: IApplication, entity: Eid, object: GraphObject, component: string
): boolean => {
  const world = app.getWorld()
  const sceneHandle = app.getScene()
  switch (component) {
    case 'position': {
      const position = ecs.Position.get(world, entity)
      object.position = [position.x, position.y, position.z]
      return true
    }
    case 'scale': {
      const scale = ecs.Scale.get(world, entity)
      object.scale = [scale.x, scale.y, scale.z]
      return true
    }
    case 'quaternion': {
      const quaternion = ecs.Quaternion.get(world, entity)
      object.rotation = [quaternion.x, quaternion.y, quaternion.z, quaternion.w]
      return true
    }
    case 'three-object':
      object.order = ecs.ThreeObject.get(world, entity).order
      return true
    case 'material': {
      const material = ecs.Material.get(world, entity)
      object.material = {
        type: 'basic',
        color: colorToString(material.r, material.g, material.b),
        textureSrc: material.textureSrc,
        roughness: material.roughness,
        metalness: material.metalness,
        opacity: material.opacity,
        emissiveIntensity: material.emissiveIntensity,
        roughnessMap: material.roughnessMap,
        metalnessMap: material.metalnessMap,
        opacityMap: material.opacityMap,
        emissiveMap: material.emissiveMap,
        emissiveColor: colorToString(material.emissiveR, material.emissiveG, material.emissiveB),
        side: material.side as Side,
        blending: material.blending as MaterialBlending,
        repeatX: material.repeatX,
        repeatY: material.repeatY,
        offsetX: material.offsetX,
        offsetY: material.offsetY,
        wrap: material.wrap as TextureWrap,
        depthTest: material.depthTest,
        depthWrite: material.depthWrite,
        wireframe: material.wireframe,
        forceTransparent: material.forceTransparent,
      }
      return true
    }
    case 'unlit-material': {
      const material = ecs.UnlitMaterial.get(world, entity)
      object.material = {
        type: 'unlit',
        color: colorToString(material.r, material.g, material.b),
        textureSrc: material.textureSrc,
        opacity: material.opacity,
        opacityMap: material.opacityMap,
        side: material.side as Side,
        blending: material.blending as MaterialBlending,
        repeatX: material.repeatX,
        repeatY: material.repeatY,
        offsetX: material.offsetX,
        offsetY: material.offsetY,
        wrap: material.wrap as TextureWrap,
        depthTest: material.depthTest,
        depthWrite: material.depthWrite,
        wireframe: material.wireframe,
        forceTransparent: material.forceTransparent,
      }
      return true
    }
    case 'shadow-material': {
      const shadowMaterial = ecs.ShadowMaterial.get(world, entity)
      object.material = {
        type: 'shadow',
        color: colorToString(shadowMaterial.r, shadowMaterial.g, shadowMaterial.b),
        opacity: shadowMaterial.opacity,
        side: shadowMaterial.side as Side,
        depthTest: shadowMaterial.depthTest,
        depthWrite: shadowMaterial.depthWrite,
      }
      return true
    }
    case 'hider-material': {
      object.material = {type: 'hider'}
      return true
    }
    case 'gltf-model': {
      const gltfModel = ecs.GltfModel.get(world, entity)
      object.gltfModel = {
        src: inferResourceObject(gltfModel.url),
        animationClip: gltfModel.animationClip,
        loop: gltfModel.loop,
        paused: gltfModel.paused,
        timeScale: gltfModel.timeScale,
        reverse: gltfModel.reverse,
        repetitions: gltfModel.repetitions,
        crossFadeDuration: gltfModel.crossFadeDuration,
      }
      return true
    }
    case 'splat': {
      const splat = ecs.Splat.get(world, entity)
      object.splat = {
        src: inferResourceObject(splat.url),
        skybox: !!splat.skybox,
      }
      return true
    }
    case 'sphere-geometry':
      object.geometry = {type: 'sphere', ...ecs.SphereGeometry.get(world, entity)}
      return true
    case 'box-geometry':
      object.geometry = {type: 'box', ...ecs.BoxGeometry.get(world, entity)}
      return true
    case 'plane-geometry':
      object.geometry = {type: 'plane', ...ecs.PlaneGeometry.get(world, entity)}
      return true
    case 'circle-geometry':
      object.geometry = {type: 'circle', ...ecs.CircleGeometry.get(world, entity)}
      return true
    case 'capsule-geometry':
      object.geometry = {type: 'capsule', ...ecs.CapsuleGeometry.get(world, entity)}
      return true
    case 'cylinder-geometry':
      object.geometry = {type: 'cylinder', ...ecs.CylinderGeometry.get(world, entity)}
      return true
    case 'cone-geometry':
      object.geometry = {type: 'cone', ...ecs.ConeGeometry.get(world, entity)}
      return true
    case 'tetrahedron-geometry':
      object.geometry = {type: 'tetrahedron', ...ecs.TetrahedronGeometry.get(world, entity)}
      return true
    case 'polyhedron-geometry': {
      const polyhedron = ecs.PolyhedronGeometry.get(world, entity)
      object.geometry = {
        type: 'polyhedron',
        faces: polyhedron.faces as Faces,
        radius: polyhedron.radius,
      }
      return true
    }
    case 'ring-geometry':
      object.geometry = {type: 'ring', ...ecs.RingGeometry.get(world, entity)}
      return true
    case 'torus-geometry':
      object.geometry = {type: 'torus', ...ecs.TorusGeometry.get(world, entity)}
      return true
    case 'hidden':
      object.hidden = true
      return true
    case 'light': {
      const light = ecs.Light.get(world, entity)

      const color = `#${[light.r, light.g, light.b]
        .map(e => e.toString(16).padStart(2, '0')).join('')}`

      const lightObject: Partial<WriteData<SchemaOf<Ecs['Light']>>> = {
        ...light,
      }

      delete lightObject.r
      delete lightObject.g
      delete lightObject.b

      delete lightObject.targetX
      delete lightObject.targetY
      delete lightObject.targetZ

      delete lightObject.shadowCameraLeft
      delete lightObject.shadowCameraRight
      delete lightObject.shadowCameraTop
      delete lightObject.shadowCameraBottom
      delete lightObject.shadowCameraNear
      delete lightObject.shadowCameraFar

      delete lightObject.shadowMapSizeWidth
      delete lightObject.shadowMapSizeHeight

      delete lightObject.colorMap

      object.light = {
        ...lightObject,
        color,
        type: light.type as LightType,
        target: [light.targetX, light.targetY, light.targetZ],
        shadowCamera: [
          light.shadowCameraLeft,
          light.shadowCameraRight,
          light.shadowCameraTop,
          light.shadowCameraBottom,
          light.shadowCameraNear,
          light.shadowCameraFar,
        ],
        shadowMapSize: [light.shadowMapSizeWidth, light.shadowMapSizeHeight],
        colorMap: inferResourceObject(light.colorMap),
      }
      return true
    }
    case 'collider': {
      const collider = ecs.Collider.get(world, entity)

      object.collider = {
        mass: collider.mass,
        linearDamping: collider.linearDamping,
        angularDamping: collider.angularDamping,
        restitution: collider.restitution,
        friction: collider.friction,
        eventOnly: !!collider.eventOnly,
        lockXPosition: !!collider.lockXPosition,
        lockYPosition: !!collider.lockYPosition,
        lockZPosition: !!collider.lockZPosition,
        lockXAxis: !!collider.lockXAxis,
        lockYAxis: !!collider.lockYAxis,
        lockZAxis: !!collider.lockZAxis,
        gravityFactor: collider.gravityFactor,
      }

      switch (collider.shape) {
        case ecs.ColliderShape.Box:
          object.collider.geometry = {
            type: 'box',
            width: collider.width,
            height: collider.height,
            depth: collider.depth,
          }
          break
        case ecs.ColliderShape.Sphere:
          object.collider.geometry = {
            type: 'sphere',
            radius: collider.radius,
          }
          break
        case ecs.ColliderShape.Plane:
          object.collider.geometry = {
            type: 'plane',
            width: collider.width,
            height: collider.height,
          }
          break
        case ecs.ColliderShape.Capsule:
          object.collider.geometry = {
            type: 'capsule',
            radius: collider.radius,
            height: collider.height,
          }
          break
        case ecs.ColliderShape.Cone:
          object.collider.geometry = {
            type: 'cone',
            radius: collider.radius,
            height: collider.height,
          }
          break
        case ecs.ColliderShape.Cylinder:
          object.collider.geometry = {
            type: 'cylinder',
            radius: collider.radius,
            height: collider.height,
          }
          break
        default:
          break
      }

      if (collider.eventOnly) {
        object.collider.eventOnly = true
      }
      return true
    }
    case 'ui': {
      // NOTE(christoph): These properties are strings in the ECS but we'll
      // assume they're still set to one of the supported enum values.
      type StringEnumUiProperties = (
        'alignContent' | 'alignItems' | 'alignSelf' | 'direction' |
        'display' | 'flexDirection' | 'justifyContent' | 'flexWrap' |
        'overflow' | 'position' | 'type' | 'textAlign' | 'backgroundSize' | 'verticalTextAlign'
      )

      type UiRuntimeSettingsWithIgnoredEnums = (
        Omit<UiRuntimeSettings, StringEnumUiProperties> &
        Pick<UiGraphSettings, StringEnumUiProperties>
      )

      const ui = ecs.Ui.get(world, entity) as UiRuntimeSettingsWithIgnoredEnums
      object.ui = {
        ...ui,
        image: inferResourceObject(ui.image),
        video: inferResourceObject(ui.video),
        font: inferFontResourceObject(ui.font),
      }

      if (ui.backgroundOpacity === UI_DEFAULTS.backgroundOpacity) {
        delete object.ui.backgroundOpacity
      }
      return true
    }
    case 'shadow': {
      const shadow = ecs.Shadow.get(world, entity)
      object.shadow = {
        castShadow: shadow.castShadow,
        receiveShadow: shadow.receiveShadow,
      }
      return true
    }
    case 'face': {
      const face = ecs.Face.get(world, entity)
      object.face = {
        ...face,
      }
      return true
    }
    case 'camera': {
      const camera = ecs.Camera.get(world, entity)

      object.camera = {
        type: camera.type as CameraType,
        fov: camera.fov,
        zoom: camera.zoom,
        left: camera.left,
        right: camera.right,
        top: camera.top,
        bottom: camera.bottom,
        nearClip: camera.nearClip,
        farClip: camera.farClip,
      }

      if (!object.camera?.xr) { object.camera.xr = {} }

      object.camera.xr = {
        xrCameraType: camera.xrCameraType as XrCameraType,
        phone: camera.phone as DeviceSupportType,
        desktop: camera.desktop as DeviceSupportType,
        headset: camera.headset as DeviceSupportType,
        leftHandedAxes: camera.leftHandedAxes,
        uvType: camera.uvType as 'standard' | 'projected',
        direction: camera.direction as CameraDirectionType,
      }

      if (!object.camera.xr.world) { object.camera.xr.world = {} }

      object.camera.xr.world = {
        scale: camera.scale as 'absolute' | 'responsive',
        disableWorldTracking: camera.disableWorldTracking,
        enableLighting: camera.enableLighting,
        enableWorldPoints: camera.enableWorldPoints,
        enableVps: camera.enableVps,
      }

      if (!object.camera.xr.face) { object.camera.xr.face = {} }

      object.camera.xr.face = {
        mirroredDisplay: camera.mirroredDisplay,
        meshGeometryFace: camera.meshGeometryFace,
        meshGeometryEyes: camera.meshGeometryEyes,
        meshGeometryIris: camera.meshGeometryIris,
        meshGeometryMouth: camera.meshGeometryMouth,
        maxDetections: camera.maxDetections,
        enableEars: camera.enableEars,
      }

      return true
    }
    case 'audio': {
      const audio = ecs.Audio.get(world, entity)

      const audioObject: Partial<WriteData<SchemaOf<Ecs['Audio']>>> = {
        ...audio,
      }

      delete audioObject.url

      // TODO(johnny) We can't differentiate between url and asset types here so
      // debug sync will convert assets into urls.
      object.audio = {
        ...audioObject,
        src: inferResourceObject(audio.url),
        distanceModel: audio.distanceModel as DistanceModel,
      }
      return true
    }
    case 'image-target': {
      const imageTarget = ecs.ImageTarget.get(world, entity)
      object.imageTarget = {
        ...imageTarget,
      }
      return true
    }
    case 'location': {
      const location = ecs.Location.get(world, entity)
      object.location = {
        ...location,
        visualization: location.visualization as LocationVisualization,
      }
      return true
    }
    case 'map': {
      const map = ecs.Map.get(world, entity)
      object.map = {
        ...map,
        targetEntity: resolveEntityReference(sceneHandle, map.targetEntity),
      }
      return true
    }
    case 'map-theme': {
      const mapTheme = ecs.MapTheme.get(world, entity)
      object.mapTheme = {
        ...mapTheme,
      }
      return true
    }
    case 'map-point': {
      const mapPoint = ecs.MapPoint.get(world, entity)
      object.mapPoint = {
        ...mapPoint,
        targetEntity: resolveEntityReference(sceneHandle, mapPoint.targetEntity),
      }
      return true
    }
    case 'disabled': {
      object.disabled = true
      return true
    }
    case 'persistent': {
      object.persistent = true
      return true
    }
    default:
      return false
  }
}

const resolveParentId = (
  sceneHandle: SceneHandle, world: World, entity: Eid, sceneGraph: DeepReadonly<SceneGraph>,
  activeSpaceId?: string, loadedSpaces?: string[]
): string | undefined => {
  const parent = world.getParent(entity)
  if (parent) {
    return resolveId(sceneHandle, parent)
  }
  const graphIdOrEid = sceneHandle.eidToObject.get(entity)?.id ?? entity.toString()
  const objectSpace = resolveSpaceForObject(sceneGraph, graphIdOrEid)?.id

  if (objectSpace && loadedSpaces?.includes(objectSpace)) {
    return objectSpace
  } else {
    // this is for handling persistent object when its original space was unmounted.
    return activeSpaceId
  }
}

// (cindy): is debugSceneGraph the right graph to use here?
const worldToSceneGraph = (
  ecs: Ecs, application: IApplication, ephemeralIdIndices: Map<Eid, number>,
  ephemeralIdCounter: {value: number}, baseSceneGraph: DeepReadonly<SceneGraph>,
  debugSceneGraph: DeepReadonly<SceneGraph>
): DeepReadonly<SceneGraph> => {
  const namedIds: Record<string, DeepReadonly<GraphObject>> = {}
  const unnamedIds: Record<string, DeepReadonly<GraphObject>> = {}

  const components = ecs.listAttributes()

  const world = application.getWorld()
  const sceneHandle = application.getScene()

  const activeSpaceId = sceneHandle.getActiveSpace()?.id
  const spawnedSpaceIds = sceneHandle.listSpaces()?.filter(s => s.spawned).map(s => s.id)

  world.allEntities.forEach((entity) => {
    const object: GraphObject = {
      id: entity.toString(),
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      components: {},
      geometry: null,
      material: null,
    }

    const parentId = resolveParentId(
      sceneHandle, world, entity, debugSceneGraph, activeSpaceId, spawnedSpaceIds
    )
    if (parentId) {
      object.parentId = parentId
    }

    const originalObject = sceneHandle.eidToObject.get(entity)

    if (originalObject) {
      object.id = originalObject.id
      object.name = originalObject.name
      namedIds[object.id] = object
    } else {
      unnamedIds[object.id] = object

      let entityIndex = ephemeralIdIndices.get(entity)
      if (!entityIndex) {
        entityIndex = ephemeralIdCounter.value++
        ephemeralIdIndices.set(entity, entityIndex)
        ephemeralIdIndices.set(entity, entityIndex)
      }
      object.name = `Entity (${entityIndex})`
      object.ephemeral = true
    }

    components.forEach((name) => {
      const value = ecs.getAttribute(name)!
      if (value.has(world, entity)) {
        const isCustom = maybeDirectAssignComponent(ecs, application, entity, object, name)
        if (!isCustom) {
          let id = `${name}/${object.id}`
          if (originalObject) {
            const existingComponent = Object.values(originalObject.components)
              .find(e => e.name === name)
            if (existingComponent) {
              id = existingComponent.id
            }
          }
          object.components[id] = {
            id,
            name,
            parameters: resolveEntityReferences(sceneHandle, value.get(world, entity)),
          }
        }
      }
    })
  })

  // reset non-ephemeral objects to its original state if it's not in the world
  const unloadedObjects = Object.entries(debugSceneGraph.objects)
    .reduce((acc, [id, object]) => {
      if (!object.ephemeral && !namedIds[id] && baseSceneGraph.objects[id]) {
        acc[id] = {
          ...baseSceneGraph.objects[id],
        }
      }
      return acc
    }, {} as Record<string, DeepReadonly<GraphObject>>)

  ephemeralIdIndices.forEach((_, eid) => {
    if (!world.allEntities.has(eid)) {
      ephemeralIdIndices.delete(eid)
    }
  })

  const activeCamera = sceneHandle.eidToObject.get(world.camera.getActiveEid())?.id
  const updatedSpaces = debugSceneGraph.spaces ? {...debugSceneGraph.spaces} : undefined
  if (updatedSpaces && activeSpaceId) {
    updatedSpaces[activeSpaceId] = {
      ...updatedSpaces[activeSpaceId],
      activeCamera,
    }
  }

  return {
    ...debugSceneGraph,
    activeCamera: debugSceneGraph.spaces ? undefined : activeCamera,
    activeMap: world.input.getActiveMap(),
    objects: {...unloadedObjects, ...namedIds, ...unnamedIds},
    spaces: updatedSpaces,
  }
}

export {
  worldToSceneGraph,
}

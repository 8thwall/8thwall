import type {DeepReadonly} from 'ts-essentials'
import {isEqual} from 'lodash-es'

import {
  BoxGeometry, Material, PlaneGeometry, SphereGeometry, ThreeObject, GltfModel, Hidden, Shadow,
  Audio, Light, CapsuleGeometry, ConeGeometry, CylinderGeometry, TetrahedronGeometry,
  CircleGeometry, FaceGeometry, Camera, Ui, Face, ShadowMaterial, UnlitMaterial,
  RingGeometry, Splat, HiderMaterial, PolyhedronGeometry, TorusGeometry,
  Persistent, ImageTarget, VideoMaterial, VideoControls,
} from './components'
import type {
  Geometry, Material as SceneMaterial, GraphComponent,
  Vec3Tuple,
  Vec4Tuple,
  Shadow as SceneShadow,
  ImageTarget as SceneImageTarget,
  Camera as SceneCamera,
  Light as SceneLight,
  GltfModel as SceneGltfModel,
  Splat as SceneSplat,
  Collider as SceneCollider,
  UiGraphSettings,
  Face as SceneFace,
  AudioSettings,
  VideoControlsGraphSettings,
} from '../shared/scene-graph'
import type {World} from './world'
import {getAttribute} from './registry'
import {Collider, ColliderShape, ColliderType} from './physics'
import type {Eid} from '../shared/schema'
import {
  DEFAULT_VOLUME, DEFAULT_PITCH, DEFAULT_REF_DISTANCE, DEFAULT_DISTANCE_MODEL,
  DEFAULT_ROLLOFF_FACTOR, DEFAULT_MAX_DISTANCE,
} from '../shared/audio-constants'
import {
  DEFAULT_ANGULAR_DAMPING, DEFAULT_FRICTION, DEFAULT_GRAVITY_FACTOR, DEFAULT_LINEAR_DAMPING,
  DEFAULT_OFFSET, DEFAULT_RESTITUTION, DEFAULT_OFFSET_QUATERNION_W, DEFAULT_OFFSET_QUATERNION_X,
  DEFAULT_OFFSET_QUATERNION_Y, DEFAULT_OFFSET_QUATERNION_Z,
} from '../shared/physic-constants'
import {DEFAULT_EMISSIVE_COLOR, MATERIAL_DEFAULTS} from '../shared/material-constants'
import {extractResourceUrl} from '../shared/resource'
import {getUiAttributes} from '../shared/get-ui-attributes'
import {Disabled} from './disabled'

const GEOMETRY_TYPES = {
  'plane': PlaneGeometry,
  'sphere': SphereGeometry,
  'box': BoxGeometry,
  'capsule': CapsuleGeometry,
  'cone': ConeGeometry,
  'cylinder': CylinderGeometry,
  'tetrahedron': TetrahedronGeometry,
  'polyhedron': PolyhedronGeometry,
  'circle': CircleGeometry,
  'ring': RingGeometry,
  'torus': TorusGeometry,
  'face': FaceGeometry,
} as const

const MATERIAL_TYPES = {
  'basic': Material,
  'unlit': UnlitMaterial,
  'shadow': ShadowMaterial,
  'hider': HiderMaterial,
  'video': VideoMaterial,
} as const

const convertEntityReference = (graphIdToEid: Map<string, Eid>, id: string | undefined) => {
  if (id === undefined) {
    // No entity is set
    return BigInt(0)
  } else {
    const parameterEid = graphIdToEid.get(id)
    return parameterEid || BigInt(0)
  }
}

// Set up the attributes of the component in the scene when the entity for the component has
// been created.
const setComponentAttributes = (
  world: World, eid: Eid, graphIdToEid: Map<string, Eid>, component: GraphComponent
) => {
  const attribute = getAttribute(component.name)
  attribute.reset(world, eid)
  const cursor =
    attribute.cursor(world, eid) as Record<string, any>
  if (component.parameters) {
    for (const [k, v] of Object.entries(component.parameters)) {
      if (typeof v === 'object' && v?.type === 'entity') {
        cursor[k] = convertEntityReference(graphIdToEid, v.id)
      } else if (v !== undefined) {
        cursor[k] = v
      }
    }
  }
}

const applyGeometryComponent = (
  world: World,
  eid: Eid,
  geometry: DeepReadonly<Geometry> | undefined,
  prevGeometry?: DeepReadonly<Geometry>
) => {
  if (prevGeometry && (!geometry || prevGeometry.type !== geometry.type)) {
    if (prevGeometry) {
      GEOMETRY_TYPES[prevGeometry.type].remove(world, eid)
    }
  }

  if (!geometry) {
    return
  }

  const {type, ...geometryWithoutType} = geometry

  const geometryClass = GEOMETRY_TYPES[type]
  if (!geometryClass) {
    throw new Error('Unexpected geometry')
  }

  geometryClass.set(world, eid, geometryWithoutType)
}

const applyMaterialComponent = (
  world: World,
  eid: Eid,
  material: DeepReadonly<SceneMaterial> | undefined,
  prevMaterial?: DeepReadonly<SceneMaterial>
) => {
  if (prevMaterial && (!material || prevMaterial.type !== material.type)) {
    MATERIAL_TYPES[prevMaterial.type].remove(world, eid)
  }

  if (!material) {
    return
  }

  if (material.type === 'hider') {
    HiderMaterial.set(world, eid, {})
    return
  }

  const [, r, g, b] = material.color.match(/^#(.{2})(.{2})(.{2})$/) || []

  switch (material.type) {
    case 'basic': {
      const emissiveColor = material.emissiveColor ?? DEFAULT_EMISSIVE_COLOR
      const [, emissiveR, emissiveG, emissiveB] = emissiveColor.match(/^#(.{2})(.{2})(.{2})$/) || []

      Material.set(world, eid, {
        r: parseInt(r, 16),
        g: parseInt(g, 16),
        b: parseInt(b, 16),
        textureSrc: extractResourceUrl(material.textureSrc),
        roughness: material.roughness ?? MATERIAL_DEFAULTS.roughness,
        metalness: material.metalness ?? MATERIAL_DEFAULTS.metalness,
        opacity: material.opacity ?? MATERIAL_DEFAULTS.opacity,
        roughnessMap: extractResourceUrl(material.roughnessMap),
        metalnessMap: extractResourceUrl(material.metalnessMap),
        opacityMap: extractResourceUrl(material.opacityMap),
        side: material.side || MATERIAL_DEFAULTS.side,
        normalScale: material.normalScale ?? MATERIAL_DEFAULTS.normalScale,
        emissiveIntensity: material.emissiveIntensity ?? MATERIAL_DEFAULTS.emissiveIntensity,
        emissiveR: parseInt(emissiveR, 16),
        emissiveG: parseInt(emissiveG, 16),
        emissiveB: parseInt(emissiveB, 16),
        normalMap: extractResourceUrl(material.normalMap),
        emissiveMap: extractResourceUrl(material.emissiveMap),
        blending: material.blending ?? MATERIAL_DEFAULTS.blending,
        repeatX: material.repeatX ?? MATERIAL_DEFAULTS.repeatX,
        repeatY: material.repeatY ?? MATERIAL_DEFAULTS.repeatY,
        offsetX: material.offsetX ?? MATERIAL_DEFAULTS.offsetX,
        offsetY: material.offsetY ?? MATERIAL_DEFAULTS.offsetY,
        wrap: material.wrap || MATERIAL_DEFAULTS.wrap,
        depthTest: material.depthTest ?? MATERIAL_DEFAULTS.depthTest,
        depthWrite: material.depthWrite ?? MATERIAL_DEFAULTS.depthWrite,
        wireframe: material.wireframe ?? MATERIAL_DEFAULTS.wireframe,
        forceTransparent: material.forceTransparent ?? MATERIAL_DEFAULTS.forceTransparent,
        textureFiltering: material.textureFiltering ?? MATERIAL_DEFAULTS.textureFiltering,
        mipmaps: material.mipmaps ?? MATERIAL_DEFAULTS.mipmaps,
      })
      break
    }
    case 'unlit': {
      UnlitMaterial.set(world, eid, {
        r: parseInt(r, 16),
        g: parseInt(g, 16),
        b: parseInt(b, 16),
        textureSrc: extractResourceUrl(material.textureSrc),
        opacity: material.opacity ?? MATERIAL_DEFAULTS.opacity,
        side: material.side || MATERIAL_DEFAULTS.side,
        opacityMap: extractResourceUrl(material.opacityMap),
        blending: material.blending ?? MATERIAL_DEFAULTS.blending,
        repeatX: material.repeatX ?? MATERIAL_DEFAULTS.repeatX,
        repeatY: material.repeatY ?? MATERIAL_DEFAULTS.repeatY,
        offsetX: material.offsetX ?? MATERIAL_DEFAULTS.offsetX,
        offsetY: material.offsetY ?? MATERIAL_DEFAULTS.offsetY,
        wrap: material.wrap || MATERIAL_DEFAULTS.wrap,
        depthTest: material.depthTest ?? MATERIAL_DEFAULTS.depthTest,
        depthWrite: material.depthWrite ?? MATERIAL_DEFAULTS.depthWrite,
        wireframe: material.wireframe ?? MATERIAL_DEFAULTS.wireframe,
        forceTransparent: material.forceTransparent ?? MATERIAL_DEFAULTS.forceTransparent,
        textureFiltering: material.textureFiltering ?? MATERIAL_DEFAULTS.textureFiltering,
        mipmaps: material.mipmaps ?? MATERIAL_DEFAULTS.mipmaps,
      })
      break
    }
    case 'shadow': {
      ShadowMaterial.set(world, eid, {
        r: parseInt(r, 16),
        g: parseInt(g, 16),
        b: parseInt(b, 16),
        opacity: material.opacity ?? MATERIAL_DEFAULTS.opacity,
        side: material.side || MATERIAL_DEFAULTS.side,
        depthTest: material.depthTest ?? MATERIAL_DEFAULTS.depthTest,
        depthWrite: material.depthWrite ?? MATERIAL_DEFAULTS.depthWrite,
      })
      break
    }
    case 'video': {
      VideoMaterial.set(world, eid, {
        r: parseInt(r, 16),
        g: parseInt(g, 16),
        b: parseInt(b, 16),
        textureSrc: extractResourceUrl(material.textureSrc),
        opacity: material.opacity ?? MATERIAL_DEFAULTS.opacity,
      })
      break
    }
    default:
      throw new Error('Unexpected material')
  }
}

const applyPositionComponent = (
  world: World, eid: Eid, position: DeepReadonly<Vec3Tuple>
) => {
  world.setPosition(eid, ...position)
}

const applyScaleComponent = (
  world: World, eid: Eid, scale: DeepReadonly<Vec3Tuple>
) => {
  world.setScale(eid, ...scale)
}

const applyQuaternionComponent = (
  world: World, eid: Eid, rotation: DeepReadonly<Vec4Tuple>
) => {
  world.setQuaternion(eid, ...rotation)
}

const applyShadowComponent = (
  world: World, eid: Eid, shadow: DeepReadonly<SceneShadow> | undefined
) => {
  if (shadow) {
    Shadow.set(world, eid, shadow)
  } else {
    Shadow.remove(world, eid)
  }
}

const applyCameraComponent = (
  world: World, eid: Eid, camera: DeepReadonly<SceneCamera> | undefined
) => {
  if (camera) {
    const cameraData = {
      type: camera.type,
      fov: camera.fov,
      zoom: camera.zoom,
      left: camera.left,
      right: camera.right,
      top: camera.top,
      bottom: camera.bottom,
      // XR
      xrCameraType: camera.xr?.xrCameraType,
      phone: camera.xr?.phone,
      desktop: camera.xr?.desktop,
      headset: camera.xr?.headset,
      nearClip: camera.nearClip,
      farClip: camera.farClip,
      leftHandedAxes: camera.xr?.leftHandedAxes,
      uvType: camera.xr?.uvType,
      direction: camera.xr?.direction,
      // World
      disableWorldTracking: camera.xr?.world?.disableWorldTracking,
      enableLighting: camera.xr?.world?.enableLighting,
      enableWorldPoints: camera.xr?.world?.enableWorldPoints,
      scale: camera.xr?.world?.scale,
      enableVps: camera.xr?.world?.enableVps,
      // Face
      mirroredDisplay: camera.xr?.face?.mirroredDisplay,
      meshGeometryFace: camera.xr?.face?.meshGeometryFace,
      meshGeometryEyes: camera.xr?.face?.meshGeometryEyes,
      meshGeometryIris: camera.xr?.face?.meshGeometryIris,
      meshGeometryMouth: camera.xr?.face?.meshGeometryMouth,
      enableEars: camera.xr?.face?.enableEars,
      maxDetections: camera.xr?.face?.maxDetections,
    }
    Camera.set(world, eid, Object.fromEntries(
      Object.entries(cameraData).filter(([, v]) => v !== undefined)
    ))
  } else {
    Camera.remove(world, eid)
  }
}

const applyLightComponent = (
  world: World, eid: Eid, light: DeepReadonly<SceneLight> | undefined
) => {
  if (light) {
    const color = light.color?.match(/^#(.{2})(.{2})(.{2})/)
    const lightData = {
      type: light.type,
      castShadow: light.castShadow,
      intensity: light.intensity,
      r: color && parseInt(color[1], 16),
      g: color && parseInt(color[2], 16),
      b: color && parseInt(color[3], 16),
      targetX: light.target?.[0],
      targetY: light.target?.[1],
      targetZ: light.target?.[2],
      shadowNormalBias: light.shadowNormalBias,
      shadowBias: light.shadowBias,
      shadowAutoUpdate: light.shadowAutoUpdate,
      shadowBlurSamples: light.shadowBlurSamples,
      shadowRadius: light.shadowRadius,
      shadowMapSizeWidth: light.shadowMapSize?.[0],
      shadowMapSizeHeight: light.shadowMapSize?.[1],
      shadowCameraLeft: light.shadowCamera?.[0],
      shadowCameraRight: light.shadowCamera?.[1],
      shadowCameraTop: light.shadowCamera?.[2],
      shadowCameraBottom: light.shadowCamera?.[3],
      shadowCameraNear: light.shadowCamera?.[4],
      shadowCameraFar: light.shadowCamera?.[5],
      distance: light.distance,
      decay: light.decay,
      followCamera: light.followCamera,
      angle: light.angle,
      penumbra: light.penumbra,
      width: light.width,
      height: light.height,
    }
    Light.set(world, eid, Object.fromEntries(
      Object.entries(lightData).filter(([, v]) => v !== undefined)
    ))
  } else {
    Light.remove(world, eid)
  }
}

const applyGltfModelComponent = (
  world: World, eid: Eid, gltfModel: DeepReadonly<SceneGltfModel> | null | undefined
) => {
  if (gltfModel) {
    GltfModel.set(world, eid, {
      url: extractResourceUrl(gltfModel.src),
      animationClip: gltfModel.animationClip,
      loop: !!gltfModel.loop,
      paused: !!gltfModel.paused,
      timeScale: gltfModel.timeScale ?? 1,
      reverse: !!gltfModel.reverse,
      repetitions: gltfModel.repetitions ?? 0,
      crossFadeDuration: gltfModel.crossFadeDuration ?? 0,
    })
  } else {
    GltfModel.remove(world, eid)
  }
}

const applySplatComponent = (
  world: World, eid: Eid, splat: DeepReadonly<SceneSplat> | null | undefined
) => {
  if (splat) {
    Splat.set(world, eid, {
      url: extractResourceUrl(splat.src),
      skybox: !!splat.skybox,
    })
  } else {
    Splat.remove(world, eid)
  }
}

const getColliderType = (type: string | undefined, mass: number | undefined) => {
  switch (type) {
    case 'static':
      return ColliderType.Static
    case 'dynamic':
      return ColliderType.Dynamic
    case 'kinematic':
      return ColliderType.Kinematic
    default:
      // fallback if type is not specified
      if (mass && mass > 0) {
        return ColliderType.Dynamic
      }
      return ColliderType.Static
  }
}

const applyColliderComponent = (
  world: World, eid: Eid,
  collider: DeepReadonly<SceneCollider> | null | undefined,
  sceneGeometry: DeepReadonly<Geometry> | null | undefined,
  gltfModel: DeepReadonly<SceneGltfModel> | null | undefined
) => {
  if (collider) {
    const geometry = collider.geometry?.type === 'auto' ? sceneGeometry : collider.geometry

    const rigidBodyInfo = {
      type: getColliderType(collider.type, collider.mass),
      mass: collider.mass || 0,
      linearDamping: collider.linearDamping ?? DEFAULT_LINEAR_DAMPING,
      angularDamping: collider.angularDamping ?? DEFAULT_ANGULAR_DAMPING,
      friction: collider.friction ?? DEFAULT_FRICTION,
      restitution: collider.restitution ?? DEFAULT_RESTITUTION,
      eventOnly: !!collider.eventOnly,
      lockXPosition: !!collider.lockXPosition,
      lockYPosition: !!collider.lockYPosition,
      lockZPosition: !!collider.lockZPosition,
      lockXAxis: !!collider.lockXAxis,
      lockYAxis: !!collider.lockYAxis,
      lockZAxis: !!collider.lockZAxis,
      gravityFactor: collider.gravityFactor ?? DEFAULT_GRAVITY_FACTOR,
      offsetX: collider.offsetX ?? DEFAULT_OFFSET,
      offsetY: collider.offsetY ?? DEFAULT_OFFSET,
      offsetZ: collider.offsetZ ?? DEFAULT_OFFSET,
      offsetQuaternionX: collider.offsetQuaternionX ?? DEFAULT_OFFSET_QUATERNION_X,
      offsetQuaternionY: collider.offsetQuaternionY ?? DEFAULT_OFFSET_QUATERNION_Y,
      offsetQuaternionZ: collider.offsetQuaternionZ ?? DEFAULT_OFFSET_QUATERNION_Z,
      offsetQuaternionW: collider.offsetQuaternionW ?? DEFAULT_OFFSET_QUATERNION_W,
      highPrecision: collider.highPrecision,  // Continuous Collision Detection
      simplificationMode: collider.simplificationMode ?? 'convex',
    }
    Collider.set(world, eid, rigidBodyInfo)
    if (geometry) {
      switch (geometry.type) {
        case 'box':
          Collider.set(world, eid, {
            shape: ColliderShape.Box,
            width: geometry.width,
            height: geometry.height,
            depth: geometry.depth,
          })
          break
        case 'sphere':
          Collider.set(world, eid, {
            shape: ColliderShape.Sphere,
            radius: geometry.radius,
          })
          break
        case 'plane':
          Collider.set(world, eid, {
            shape: ColliderShape.Plane,
            width: geometry.width,
            height: geometry.height,
          })
          break
        case 'capsule':
          Collider.set(world, eid, {
            shape: ColliderShape.Capsule,
            radius: geometry.radius,
            height: geometry.height,
          })
          break
        case 'cone':
          Collider.set(world, eid, {
            shape: ColliderShape.Cone,
            radius: geometry.radius,
            height: geometry.height,
          })
          break
        case 'cylinder':
          Collider.set(world, eid, {
            shape: ColliderShape.Cylinder,
            radius: geometry.radius,
            height: geometry.height,
          })
          break
        case 'circle':
          Collider.set(world, eid, {
            shape: ColliderShape.Circle,
            radius: geometry.radius,
          })
          break
        default:
          throw new Error('Unexpected collider geometry')
      }
    } else if (gltfModel) {
      GltfModel.set(world, eid, {collider: true})
    }
  } else {
    Collider.remove(world, eid)
  }
}

const applyAudioComponent = (
  world: World, eid: Eid, audio: DeepReadonly<AudioSettings> | null | undefined
) => {
  if (audio) {
    Audio.set(world, eid, {
      url: extractResourceUrl(audio.src),
      volume: audio.volume ?? DEFAULT_VOLUME,
      loop: !!audio.loop,
      paused: !!audio.paused,
      pitch: audio.pitch ?? DEFAULT_PITCH,
      positional: !!audio.positional,
      refDistance: audio.refDistance ?? DEFAULT_REF_DISTANCE,
      rolloffFactor: audio.rolloffFactor ?? DEFAULT_ROLLOFF_FACTOR,
      distanceModel: audio.distanceModel ?? DEFAULT_DISTANCE_MODEL,
      maxDistance: audio.maxDistance ?? DEFAULT_MAX_DISTANCE,
    })
  } else {
    Audio.remove(world, eid)
  }
}

const applyVideoControlsComponent = (
  world: World, eid: Eid, video: DeepReadonly<VideoControlsGraphSettings> | null | undefined
) => {
  if (video) {
    VideoControls.set(world, eid, video)
  } else {
    VideoControls.remove(world, eid)
  }
}

const applyOrderComponent = (world: World, eid: Eid, order: DeepReadonly<number> | undefined) => {
  ThreeObject.set(world, eid, {order})
}

const applyUiComponent = (
  world: World, eid: Eid, ui: DeepReadonly<UiGraphSettings> | null | undefined
) => {
  if (ui) {
    Ui.set(world, eid, getUiAttributes(ui))
  } else {
    Ui.remove(world, eid)
  }
}

const applyFaceComponent = (
  world: World, eid: Eid, face: DeepReadonly<SceneFace> | null | undefined
) => {
  if (face) {
    Face.set(world, eid, {
      id: face.id,
      addAttachmentState: false,
    })
  } else {
    Face.remove(world, eid)
  }
}

const applyImageTargetComponent = (
  world: World, eid: Eid, imageTarget: DeepReadonly<SceneImageTarget> | null | undefined
) => {
  if (imageTarget) {
    ImageTarget.set(world, eid, imageTarget)
  } else {
    ImageTarget.remove(world, eid)
  }
}

const applyHiddenComponent = (
  world: World, eid: Eid, hidden: DeepReadonly<boolean> | null | undefined
) => {
  if (hidden) {
    Hidden.set(world, eid)
  } else {
    Hidden.remove(world, eid)
  }
}

const applyDisabledComponent = (
  world: World, eid: Eid, disabled: DeepReadonly<boolean> | null | undefined
) => {
  if (disabled) {
    Disabled.set(world, eid)
  } else {
    Disabled.remove(world, eid)
  }
}

const applyPersistentComponent = (
  world: World, eid: Eid, persistent: DeepReadonly<boolean> | null | undefined
) => {
  if (persistent) {
    Persistent.set(world, eid)
  } else {
    Persistent.remove(world, eid)
  }
}

const applyComponentUpdates = (
  world: World,
  eid: Eid,
  components: DeepReadonly<Record<GraphComponent['id'], GraphComponent>>,
  graphIdToEid: Map<string, Eid>,
  prevComponents?: DeepReadonly<Record<GraphComponent['id'], GraphComponent>>
) => {
  if (prevComponents) {
    Object.values(prevComponents).forEach((component) => {
      if (!components[component.id]) {
        const attribute = getAttribute(component.name)
        attribute.remove(world, eid)
      }
    })
  }

  Object.values(components).forEach((component) => {
    const shouldUpdateComponent = !prevComponents || !isEqual(
      components[component.id], prevComponents[component.id]
    )
    if (shouldUpdateComponent) {
      setComponentAttributes(world, eid, graphIdToEid, component)
    }
  })
}

export {
  applyGeometryComponent,
  applyMaterialComponent,
  applyPositionComponent,
  applyScaleComponent,
  applyQuaternionComponent,
  applyShadowComponent,
  applyCameraComponent,
  applyLightComponent,
  applyGltfModelComponent,
  applySplatComponent,
  applyColliderComponent,
  applyAudioComponent,
  applyVideoControlsComponent,
  applyOrderComponent,
  applyUiComponent,
  applyFaceComponent,
  applyImageTargetComponent,
  applyHiddenComponent,
  applyDisabledComponent,
  applyPersistentComponent,
  applyComponentUpdates,
}

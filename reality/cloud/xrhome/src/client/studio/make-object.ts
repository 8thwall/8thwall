import {v4 as uuid} from 'uuid'

import type {
  BoxGeometry, CapsuleGeometry, CircleGeometry, ConeGeometry, CylinderGeometry, Geometry,
  GeometryType, GraphObject, Material, PlaneGeometry, SphereGeometry, TetrahedronGeometry,
  FaceGeometry,
  Collider as SceneCollider,
  ColliderGeometryType,
  Light,
  LightType,
  GltfModel,
  RingGeometry,
  Splat,
  StaticImageTargetOrientation,
  PolyhedronGeometry,
  TorusGeometry,
  MapPoint,
} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {DEFAULT_MATERIAL_COLOR} from '@ecs/shared/material-constants'

import {inferResourceObject} from '@ecs/shared/resource'
import type {ParticlesSchema} from '@ecs/runtime'
import {THEME_PRESETS, MAP_DEFAULTS} from '@ecs/shared/map-constants'
import type {MapPointOverrides} from '@ecs/shared/map-controller'

import {CIRCLE_05_URL} from '../../shared/studio/cdn-assets'

const SUPPORTED_PRIMITIVE_COLLIDER_GEOMETRIES: DeepReadonly<GeometryType[]> = [
  'box', 'sphere', 'plane', 'capsule', 'cone', 'cylinder', 'circle',
]

const DEFAULT_POSITION: DeepReadonly<[number, number, number]> = [0, 0, 0]
const DEFAULT_DIRECTIONAL_LIGHT_POSITION: DeepReadonly<[number, number, number]> = [10, 5, 5]
const DEFAULT_ROTATION: DeepReadonly<[number, number, number, number]> = [0, 0, 0, 1]
const DEFAULT_SCALE: DeepReadonly<[number, number, number]> = [1, 1, 1]

const PrimaryGeometryTypes = [
  {type: 'plane', label: 'mesh_configurator.geometry_type.option.plane', stroke: 'plane'},
  {type: 'box', label: 'mesh_configurator.geometry_type.option.box', stroke: 'box'},
  {type: 'sphere', label: 'mesh_configurator.geometry_type.option.sphere', stroke: 'sphere'},
  {type: 'cylinder', label: 'mesh_configurator.geometry_type.option.cylinder', stroke: 'cylinder'},
]

const SecondaryGeometryTypes = [
  {type: 'capsule', label: 'mesh_configurator.geometry_type.option.capsule', stroke: 'capsule'},
  {type: 'cone', label: 'mesh_configurator.geometry_type.option.cone', stroke: 'cone'},
  {type: 'circle', label: 'mesh_configurator.geometry_type.option.circle', stroke: 'circle'},
  {
    type: 'polyhedron',
    label: 'mesh_configurator.geometry_type.option.polyhedron',
    stroke: 'tetrahedron',
  },
  {type: 'ring', label: 'mesh_configurator.geometry_type.option.ring', stroke: 'ring'},
  {type: 'torus', label: 'mesh_configurator.geometry_type.option.torus', stroke: 'torus'},
]

const LightTypes = [
  {
    type: 'directional',
    label: 'new_primitive_button.option.light.directional',
    stroke: 'directionalLight',
  },
  {type: 'ambient', label: 'new_primitive_button.option.light.ambient', stroke: 'ambientLight'},
  {type: 'point', label: 'new_primitive_button.option.light.point', stroke: 'pointLight'},
  {type: 'spot', label: 'new_primitive_button.option.light.spot', stroke: 'spotLight'},
  {type: 'area', label: 'new_primitive_button.option.light.area', stroke: 'areaLight'},
]

const DEFAULT_BOX_GEOMETRY: DeepReadonly<BoxGeometry> = {
  type: 'box',
  depth: 1,
  height: 1,
  width: 1,
}

const DEFAULT_SPHERE_GEOMETRY: DeepReadonly<SphereGeometry> = {
  type: 'sphere',
  radius: 0.5,
}

const DEFAULT_PLANE_GEOMETRY: DeepReadonly<PlaneGeometry> = {
  type: 'plane',
  width: 1,
  height: 1,
}

const DEFAULT_CAPSULE_GEOMETRY: DeepReadonly<CapsuleGeometry> = {
  type: 'capsule',
  radius: 0.5,
  height: 1,
}

const DEFAULT_CONE_GEOMETRY: DeepReadonly<ConeGeometry> = {
  type: 'cone',
  radius: 0.5,
  height: 1,
}

const DEFAULT_CYLINDER_GEOMETRY: DeepReadonly<CylinderGeometry> = {
  type: 'cylinder',
  radius: 0.5,
  height: 1,
}

const DEFAULT_TETRAHEDRON_GEOMETRY: DeepReadonly<TetrahedronGeometry> = {
  type: 'tetrahedron',
  radius: 1,
}

const DEFAULT_POLYHEDRON_GEOMETRY: DeepReadonly<PolyhedronGeometry> = {
  type: 'polyhedron',
  radius: 1,
  faces: 4,
}

const DEFAULT_CIRCLE_GEOMETRY: DeepReadonly<CircleGeometry> = {
  type: 'circle',
  radius: 0.5,
}

const DEFAULT_RING_GEOMETRY: DeepReadonly<RingGeometry> = {
  type: 'ring',
  innerRadius: 0.25,
  outerRadius: 0.5,
}

const DEFAULT_TORUS_GEOMETRY: DeepReadonly<TorusGeometry> = {
  type: 'torus',
  radius: 0.5,
  tubeRadius: 0.2,
}

const DEFAULT_FACE_GEOMETRY: DeepReadonly<FaceGeometry> = {
  type: 'face',
  id: 1,
}

const DEFAULT_GEOMETRIES: Record<GeometryType, Geometry> = {
  box: DEFAULT_BOX_GEOMETRY,
  sphere: DEFAULT_SPHERE_GEOMETRY,
  plane: DEFAULT_PLANE_GEOMETRY,
  capsule: DEFAULT_CAPSULE_GEOMETRY,
  cone: DEFAULT_CONE_GEOMETRY,
  cylinder: DEFAULT_CYLINDER_GEOMETRY,
  tetrahedron: DEFAULT_TETRAHEDRON_GEOMETRY,
  polyhedron: DEFAULT_POLYHEDRON_GEOMETRY,
  circle: DEFAULT_CIRCLE_GEOMETRY,
  ring: DEFAULT_RING_GEOMETRY,
  torus: DEFAULT_TORUS_GEOMETRY,
  face: DEFAULT_FACE_GEOMETRY,
} as const

const makeEmptyObject = (parentId: string): GraphObject => ({
  id: uuid(),
  position: [...DEFAULT_POSITION],
  rotation: [...DEFAULT_ROTATION],
  scale: [...DEFAULT_SCALE],
  geometry: null,
  material: null,
  parentId,
  components: {},
})

const makeGeometry = (type: GeometryType): Geometry => {
  const geometry = DEFAULT_GEOMETRIES[type]
  if (!geometry) {
    throw new Error(`Unexpected type: ${type}`)
  }
  return {...geometry}
}

const makeGltfModel = (url: string): GltfModel => ({
  src: inferResourceObject(url),
  animationClip: '',
  loop: true,
})

const makeSplat = (url: string): Splat => ({
  src: inferResourceObject(url),
  skybox: true,
})

const makeColliderGeometry = (type: ColliderGeometryType): SceneCollider['geometry'] => {
  if (type === 'auto') {
    return {type: 'auto'}
  }
  const geometry = DEFAULT_GEOMETRIES[type] as SceneCollider['geometry']
  if (!geometry) {
    throw new Error(`Unexpected type: ${type}`)
  }
  return {...geometry}
}

const makeMaterial = (): Material => ({
  type: 'basic',
  color: DEFAULT_MATERIAL_COLOR,
})

const makeLight = (type: LightType): Light => ({
  type,
})

const makeGeometryComponents = (type: GeometryType): Partial<GraphObject> => {
  const geometry = makeGeometry(type)
  return {
    geometry,
    material: makeMaterial(),
  }
}

const makePrimitive = (parentId: string, type: GeometryType | LightType): GraphObject => {
  const isLightType = LightTypes.some(({type: lightType}) => lightType === type)
  const components: Partial<GraphObject> = isLightType
    ? (
      {light: makeLight(type as LightType), position: [...DEFAULT_DIRECTIONAL_LIGHT_POSITION]}
    )
    : makeGeometryComponents(type as GeometryType)
  return ({
    ...makeEmptyObject(parentId),
    // Note: plane is rotated -90 degrees in the x axis for convenience
    rotation: type === 'plane' ? [-0.7071068, 0, 0, 0.7071068] : [0, 0, 0, 1],
    ...components,
  })
}

const makeCamera = (parentId: string, objectName: string): GraphObject => ({
  ...makeEmptyObject(parentId),
  position: [0, 2, 0],
  name: objectName,
  camera: {
    type: 'perspective',
  },
})

const makeFace = (parentId: string): GraphObject => {
  const components: Partial<GraphObject> = {
    face: {
      id: 1,
      addAttachmentState: false,
    },
    position: [0, 2, 0],
  }
  return ({
    ...makeEmptyObject(parentId),
    ...components,
  })
}

const makeFaceMeshObject = (parentId: string, objectName: string): GraphObject => {
  const newObject = makeEmptyObject(parentId)
  newObject.name = objectName
  newObject.geometry = DEFAULT_FACE_GEOMETRY
  newObject.material = makeMaterial()
  // Adding in face mesh anchor component.
  const id = uuid()
  const name = 'face-mesh-anchor'
  newObject.components = {
    [id]: {
      id,
      name,
      parameters: {},
    },
  }
  return newObject
}

const makeLocation = (
  parentId: string,
  name: string,
  title: string,
  poiId: string,
  lat: number,
  lng: number,
  anchorNodeId: string,
  anchorSpaceId: string,
  imageUrl: string,
  anchorPayload: string
): GraphObject => {
  const anchorId = uuid()
  const components: Partial<GraphObject> = {
    name: title,
    position: [0, 0, 0],
    location: {
      name,
      poiId,
      lat,
      lng,
      title,
      anchorNodeId,
      anchorSpaceId,
      imageUrl,
      anchorPayload,
      visualization: 'mesh',
    },
    components: {
      [anchorId]: {
        id: anchorId,
        name: 'location-anchor',
        parameters: {},
      },
    },
  }
  return ({
    ...makeEmptyObject(parentId),
    ...components,
  })
}

const makeMap = (parentId: string, objectName: string): GraphObject => ({
  ...makeEmptyObject(parentId),
  name: objectName,
  map: {...MAP_DEFAULTS},
  mapTheme: {...THEME_PRESETS.natural},
})

const makeMapPoint = (
  parentId: string,
  objectName: string,
  mapPoint: MapPoint,
  overrides: MapPointOverrides
): GraphObject => ({
  ...makeEmptyObject(parentId),
  name: objectName,
  mapPoint,
  ...overrides,
})

const makeParticles = (): Partial<ParticlesSchema> => ({
  emitterLife: 0,
  particlesPerShot: 20,
  emitDelay: 0.1,
  minimumLifespan: 1,
  maximumLifespan: 1,
  mass: 1,
  gravity: 0.2,
  scale: 0.2,
  forceX: 5,
  forceY: 12,
  forceZ: 5,
  spread: 360,
  radialVelocity: 5,
  spawnAreaType: 'point',
  boundingZoneType: 'none',
  resourceType: 'sprite',
  resourceUrl: CIRCLE_05_URL,
})

const makeImageTarget = (
  parentId: string,
  imageTargetName?: string,
  staticOrientation?: StaticImageTargetOrientation
): GraphObject => {
  const components: Partial<GraphObject> = {
    name: 'image-target',
    position: [0, 0, 0],
    imageTarget: {
      name: imageTargetName ?? '',
      staticOrientation: staticOrientation ?? undefined,
    },
  }
  return ({
    ...makeEmptyObject(parentId),
    ...components,
  })
}

export {
  makeEmptyObject,
  makeGeometry,
  makeGltfModel,
  makeSplat,
  makeColliderGeometry,
  makeMaterial,
  makePrimitive,
  makeCamera,
  makeFace,
  makeFaceMeshObject,
  makeImageTarget,
  makeLocation,
  makeMap,
  makeMapPoint,
  makeParticles,
  PrimaryGeometryTypes,
  SecondaryGeometryTypes,
  LightTypes,
  SUPPORTED_PRIMITIVE_COLLIDER_GEOMETRIES,
  DEFAULT_DIRECTIONAL_LIGHT_POSITION,
}

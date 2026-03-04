import type * as FlexStyles from './flex-style-types'
import type {RuntimeVersionTarget} from './runtime-version'

/* eslint-disable max-len */
/**
 * @schema array(z.number()).length(2).describe('A 2D vector represented by [x, y] coordinates. Can represent a point in a plane, a directional vector, or other 2D quantities.')
 */
type Vec2Tuple = [number, number]

/**
 * @schema array(z.number()).length(3).describe('A 3D vector represented by [x, y, z] coordinates. Can represent a point in space, a directional vector, or other 3D quantities.')
 */
type Vec3Tuple = [number, number, number]

/**
 * @schema array(z.number()).length(4).describe('A 4D vector represented by [x, y, z, w] coordinates. Often used for homogeneous coordinates in 3D transformations or for quaternions.')
 */
type Vec4Tuple = [number, number, number, number]

/**
 * @schema array(z.number()).length(6).describe('Used to represent shadow camera frustum bounds: left, right, top, bottom, near, far.')
 */
type Vec6Tuple = [number, number, number, number, number, number]
/* eslint-enable max-len */

/**
 * @description An object ID.
 */
type ObjectId = string

type GraphComponent = {
  id: ObjectId
  name: string
  parameters: Record<string, string | number | boolean | EntityReference>
}

type BoxGeometry = {
  type: 'box'
  width: number
  height: number
  depth: number
}

type SphereGeometry = {
  type: 'sphere'
  radius: number
}

type PlaneGeometry = {
  type: 'plane'
  width: number
  height: number
}

type CapsuleGeometry = {
  type: 'capsule'
  radius: number
  height: number
}

type ConeGeometry = {
  type: 'cone'
  radius: number
  height: number
}

type CylinderGeometry = {
  type: 'cylinder'
  radius: number
  height: number
}

type TetrahedronGeometry = {
  type: 'tetrahedron'
  radius: number
}

type Faces = 4 | 8 | 12 | 20

type PolyhedronGeometry = {
  type: 'polyhedron'
  radius: number
  faces: Faces
}

type CircleGeometry = {
  type: 'circle'
  radius: number
}

type RingGeometry = {
  type: 'ring'
  innerRadius: number
  outerRadius: number
}

type TorusGeometry = {
  type: 'torus'
  radius: number
  tubeRadius: number
}

type FaceGeometry = {
  type: 'face'
  id: number
}

type Side = 'front' | 'back' | 'double'
type MaterialBlending = 'no' | 'normal' | 'additive' | 'subtractive' | 'multiply'
type TextureWrap = 'clamp' | 'repeat' | 'mirroredRepeat'
type TextureFiltering = 'smooth' | 'sharp'

type HiderMaterial = {
  type: 'hider'
}

type ShadowMaterial = {
  type: 'shadow'
  color: string
  opacity?: number
  side?: Side
  depthTest?: boolean
  depthWrite?: boolean
}

type UnlitMaterial = {
  type: 'unlit'
  color: string
  textureSrc?: string | Resource
  opacity?: number
  opacityMap?: string | Resource
  side?: Side
  blending?: MaterialBlending
  repeatX?: number
  repeatY?: number
  offsetX?: number
  offsetY?: number
  wrap?: TextureWrap
  depthTest?: boolean
  depthWrite?: boolean
  wireframe?: boolean
  forceTransparent?: boolean
  textureFiltering?: TextureFiltering
  mipmaps?: boolean
}

type BasicMaterial = {
  type: 'basic'
  color: string
  // TODO(christoph): Remove the string after migration
  textureSrc?: string | Resource
  roughness?: number
  metalness?: number
  opacity?: number
  normalScale?: number
  emissiveIntensity?: number
  roughnessMap?: string | Resource
  metalnessMap?: string | Resource
  opacityMap?: string | Resource
  normalMap?: string | Resource
  emissiveMap?: string | Resource
  emissiveColor?: string
  side?: Side
  blending?: MaterialBlending
  repeatX?: number
  repeatY?: number
  offsetX?: number
  offsetY?: number
  wrap?: TextureWrap
  depthTest?: boolean
  depthWrite?: boolean
  wireframe?: boolean
  forceTransparent?: boolean
  textureFiltering?: TextureFiltering
  mipmaps?: boolean
}

type VideoMaterial = {
  type: 'video'
  color: string
  textureSrc?: string | Resource
  opacity?: number
}

type EntityReference = {type: 'entity', id: ObjectId}

type Geometry = BoxGeometry | SphereGeometry | PlaneGeometry | CapsuleGeometry | ConeGeometry |
  CylinderGeometry | TetrahedronGeometry | PolyhedronGeometry | CircleGeometry | RingGeometry |
  TorusGeometry | FaceGeometry | null

type GeometryType = 'box' | 'sphere' | 'plane' | 'capsule' | 'cone' | 'cylinder' | 'tetrahedron' |
  'polyhedron' | 'circle' | 'ring' | 'torus' | 'face'

type Material = BasicMaterial | UnlitMaterial | ShadowMaterial | HiderMaterial | VideoMaterial
| null

type Url = {type: 'url', url: string}

type Asset = {type: 'asset', asset: string}

type Resource = Url | Asset

type SimplificationMode = 'convex' | 'concave'

type GltfModel = {
  src: Resource
  animationClip?: string
  loop?: boolean
  paused?: boolean
  timeScale?: number
  reverse?: boolean
  repetitions?: number
  crossFadeDuration?: number
}

type Splat = {
  src: Resource
  skybox?: boolean
}

type ColliderGeometryType = 'box' | 'sphere' | 'plane' | 'capsule' | 'cone' | 'cylinder' | 'auto'
type ColliderType = 'static' | 'dynamic' | 'kinematic'

type Collider = {
  type?: ColliderType
  geometry?: BoxGeometry | SphereGeometry | PlaneGeometry | CapsuleGeometry |
    ConeGeometry | CylinderGeometry | {type: 'auto'}
  mass?: number
  linearDamping?: number
  angularDamping?: number
  friction?: number
  rollingFriction?: number
  spinningFriction?: number
  restitution?: number
  eventOnly?: boolean
  lockXPosition?: boolean
  lockYPosition?: boolean
  lockZPosition?: boolean
  lockXAxis?: boolean
  lockYAxis?: boolean
  lockZAxis?: boolean
  gravityFactor?: number
  highPrecision?: boolean
  offsetX?: number
  offsetY?: number
  offsetZ?: number
  offsetQuaternionX?: number
  offsetQuaternionY?: number
  offsetQuaternionZ?: number
  offsetQuaternionW?: number
  simplificationMode?: SimplificationMode
}

type FontResource = {type: 'font', font: string} | Resource
type UiRootType = 'overlay' | '3d'
type BackgroundSize = 'contain' | 'cover' | 'stretch' | 'nineslice'

type UiGraphSettings = Partial<{
  top: string
  left: string
  right: string
  bottom: string
  width: number | string
  height: number | string
  type: UiRootType
  ignoreRaycast: boolean
  font: FontResource
  background: string
  backgroundOpacity: number
  backgroundSize: BackgroundSize
  nineSliceBorderTop: string
  nineSliceBorderBottom: string
  nineSliceBorderLeft: string
  nineSliceBorderRight: string
  nineSliceScaleFactor: number
  opacity: number
  color: string
  text: string
  textAlign: FlexStyles.TextAlignContent
  verticalTextAlign: FlexStyles.VerticalTextAlignContent
  image: Resource
  fixedSize: boolean
  borderColor: string
  borderRadius: number
  borderRadiusTopLeft: string
  borderRadiusTopRight: string
  borderRadiusBottomRight: string
  borderRadiusBottomLeft: string
  fontSize: number
  alignContent: FlexStyles.AlignContent
  alignItems: FlexStyles.AlignItems
  alignSelf: FlexStyles.AlignItems
  borderWidth: number
  direction: FlexStyles.Direction
  display: FlexStyles.Display
  flex: number
  flexBasis: string
  flexDirection: FlexStyles.FlexDirection
  rowGap: string
  gap: string
  columnGap: string
  flexGrow: number
  flexShrink: number
  flexWrap: FlexStyles.FlexWrap
  justifyContent: FlexStyles.JustifyContent
  margin: string
  marginBottom: string
  marginLeft: string
  marginRight: string
  marginTop: string
  maxHeight: string
  maxWidth: string
  minHeight: string
  minWidth: string
  overflow: FlexStyles.Overflow
  padding: string
  paddingBottom: string
  paddingLeft: string
  paddingRight: string
  paddingTop: string
  position: FlexStyles.PositionMode
  video: Resource
  stackingOrder: number
}>

type Shadow = {
  castShadow?: boolean
  receiveShadow?: boolean
}

type DistanceModel = 'exponential' | 'inverse' | 'linear'

type AudioSettings = {
  src?: Resource
  volume?: number
  loop?: boolean
  paused?: boolean
  pitch?: number
  positional?: boolean
  refDistance?: number
  rolloffFactor?: number
  distanceModel?: DistanceModel
  maxDistance?: number
}

type VideoControlsGraphSettings = {
  volume?: number
  loop?: boolean
  paused?: boolean
  speed?: number
  positional?: boolean
  refDistance?: number
  rolloffFactor?: number
  distanceModel?: DistanceModel
  maxDistance?: number
}

type LightType = 'directional' | 'ambient' | 'point' | 'spot' | 'area'

type Light = {
  type: LightType
  color?: string
  intensity?: number
  castShadow?: boolean
  target?: Vec3Tuple
  shadowNormalBias?: number
  shadowBias?: number
  shadowAutoUpdate?: boolean
  shadowBlurSamples?: number
  shadowRadius?: number
  // NOTE(johnny): width, height
  shadowMapSize?: Vec2Tuple
  // NOTE(johnny): left, right, top, bottom, near, far
  shadowCamera?: Vec6Tuple
  distance?: number
  decay?: number
  followCamera?: boolean
  angle?: number
  penumbra?: number
  colorMap?: Resource
  width?: number
  height?: number
}

type CameraType = 'perspective' | 'orthographic'

type Camera = {
  type?: CameraType
  left?: number
  right?: number
  top?: number
  bottom?: number
  fov?: number
  zoom?: number
  nearClip?: number
  farClip?: number
  xr?: XrConfig
}

type XrCameraType = 'world' | 'face' | 'hand' | 'layers' | 'worldLayers' | '3dOnly'

type DeviceSupportType = 'AR' | 'VR' | '3D' | 'disabled'
type CameraDirectionType = 'front' | 'back'

type XrConfig = {
  xrCameraType?: XrCameraType
  phone?: DeviceSupportType
  desktop?: DeviceSupportType
  headset?: DeviceSupportType
  leftHandedAxes?: boolean
  uvType?: 'standard' | 'projected'
  direction?: CameraDirectionType
  world?: XrWorldConfig
  face?: XrFaceConfig
}

type XrWorldConfig = {
  scale?: 'absolute' | 'responsive'
  disableWorldTracking?: boolean
  enableLighting?: boolean
  enableWorldPoints?: boolean
  enableVps?: boolean
}

type XrFaceConfig = {
  mirroredDisplay?: boolean
  meshGeometryFace?: boolean
  meshGeometryEyes?: boolean
  meshGeometryIris?: boolean
  meshGeometryMouth?: boolean
  enableEars?: boolean
  maxDetections?: number
}

type Face = {
  id: number
  addAttachmentState: boolean
}

type StaticImageTargetOrientation = {
  rollAngle: number
  pitchAngle: number
}

type ImageTarget = {
  name: string
  staticOrientation?: StaticImageTargetOrientation
}

type LocationVisualization = 'mesh' | 'splat' | 'none'

type Location = {
  name: string
  poiId: string
  lat: number
  lng: number
  title: string
  anchorNodeId: string
  anchorSpaceId: string
  imageUrl: string
  anchorPayload: string
  visualization?: LocationVisualization
}

type Map = {
  latitude: number
  longitude: number
  targetEntity?: EntityReference
  radius: number
  spawnLocations: boolean
  useGps: boolean
}

type MapTheme = {
  landColor?: string
  buildingColor?: string
  parkColor?: string
  parkingColor?: string
  roadColor?: string
  sandColor?: string
  transitColor?: string
  waterColor?: string

  landOpacity?: number
  buildingOpacity?: number
  parkOpacity?: number
  parkingOpacity?: number
  roadOpacity?: number
  sandOpacity?: number
  transitOpacity?: number
  waterOpacity?: number

  lod?: number

  buildingBase?: number
  parkBase?: number
  parkingBase?: number
  roadBase?: number
  sandBase?: number
  transitBase?: number
  waterBase?: number

  buildingMinMeters?: number
  buildingMaxMeters?: number
  roadLMeters?: number
  roadMMeters?: number
  roadSMeters?: number
  roadXLMeters?: number
  transitMeters?: number
  waterMeters?: number

  roadLMin?: number
  roadMMin?: number
  roadSMin?: number
  roadXLMin?: number
  transitMin?: number
  waterMin?: number

  landVisibility?: boolean
  buildingVisibility?: boolean
  parkVisibility?: boolean
  parkingVisibility?: boolean
  roadVisibility?: boolean
  sandVisibility?: boolean
  transitVisibility?: boolean
  waterVisibility?: boolean
}

type MapPoint = {
  latitude: number
  longitude: number
  targetEntity?: EntityReference
  meters: number
  minScale: number
}

type BaseGraphObject = {
  id: ObjectId
  name?: string
  parentId?: string
  prefab?: true
  position: Vec3Tuple
  rotation: Vec4Tuple
  scale: Vec3Tuple
  geometry: Geometry
  material: Material
  gltfModel?: GltfModel | null | undefined
  splat?: Splat | null | undefined
  collider?: Collider | null | undefined
  audio?: AudioSettings | null | undefined
  videoControls?: VideoControlsGraphSettings | null | undefined
  ui?: UiGraphSettings | null | undefined
  hidden?: boolean
  shadow?: Shadow
  light?: Light
  camera?: Camera
  face?: Face
  imageTarget?: ImageTarget
  location?: Location
  map?: Map
  mapTheme?: MapTheme
  mapPoint?: MapPoint
  components: Record<GraphComponent['id'], GraphComponent>
  ephemeral?: boolean
  disabled?: true
  persistent?: true
  order?: number
}

type PrefabInstanceDeletions = Partial<{
  [K in keyof Omit<BaseGraphObject, 'id' | 'name' | 'parentId' | 'prefab' | 'components'>]: true
}> & Partial<{
  components: Record<GraphComponent['id'], true>
}>

type PrefabInstanceChildrenData = Omit<Partial<BaseGraphObject>, 'id' | 'prefab'> & {
  id?: string
  deletions?: PrefabInstanceDeletions
  deleted?: true
}

type PrefabInstanceChildren = Record<string, PrefabInstanceChildrenData>

type InstanceData = {
  instanceOf: string
  deletions: PrefabInstanceDeletions
  children?: PrefabInstanceChildren
}

type GraphObject = Partial<BaseGraphObject> & {
  id: ObjectId
  components: Record<GraphComponent['id'], GraphComponent>
  instanceData?: InstanceData
}

type Space = {
  id: string
  name: string
  sky?: Sky
  activeCamera?: string
  includedSpaces?: string[]
  reflections?: string | Resource | null
  fog?: Fog
}

type Spaces = Record<string, Space>

type Binding = {
  input: string
  modifiers: string[]
}

type Action = {
  name: string
  bindings: Binding[]
}

type InputMap = Record<string, Action[]>

type Color = {type: 'color', color?: string}

type GradientStyle = 'linear' | 'radial'
type Gradient = {
  type: 'gradient'
  style?: GradientStyle
  colors?: string[]
}
type Image<T = Resource> = {type: 'image', src?: T}
type NoSky = {type: 'none'}

type Sky<T = Resource> = Color | Gradient | Image<T> | NoSky

type NoFog = {type: 'none'}

type LinearFog = {
  type: 'linear'
  near: number
  far: number
  color: string
}

type ExponentialFog = {
  type: 'exponential'
  density: number
  color: string
}

type Fog = NoFog | LinearFog | ExponentialFog

type SceneGraph = {
  activeCamera?: string
  activeMap?: string
  inputs?: InputMap
  sky?: Sky
  reflections?: string | Resource
  entrySpaceId?: string
  spaces?: Spaces
  objects: Record<string, GraphObject>
  runtimeVersion?: RuntimeVersionTarget
}

type Expanse = SceneGraph & {
  // Stores a base64 encoded automerge document for the purpose of resolving merge conflicts,
  // should not be used or made accessible at runtime.
  history?: string
  historyVersion?: string
}

export type {
  Action,
  AudioSettings,
  BackgroundSize,
  BaseGraphObject,
  BasicMaterial,
  Binding,
  BoxGeometry,
  Camera,
  CameraDirectionType,
  CameraType,
  CapsuleGeometry,
  CircleGeometry,
  Collider,
  ColliderType,
  ColliderGeometryType,
  ConeGeometry,
  CylinderGeometry,
  DeviceSupportType,
  DistanceModel,
  EntityReference,
  Expanse,
  Face,
  FaceGeometry,
  Faces,
  Fog,
  FontResource,
  Geometry,
  GeometryType,
  SimplificationMode,
  GltfModel,
  GradientStyle,
  GraphComponent,
  GraphObject,
  HiderMaterial,
  ImageTarget,
  InputMap,
  InstanceData,
  Light,
  LightType,
  Location,
  LocationVisualization,
  Map,
  MapPoint,
  MapTheme,
  Material,
  MaterialBlending,
  PlaneGeometry,
  PolyhedronGeometry,
  PrefabInstanceChildren,
  PrefabInstanceChildrenData,
  PrefabInstanceDeletions,
  Resource,
  RingGeometry,
  SceneGraph,
  Shadow,
  ShadowMaterial,
  Side,
  Sky,
  Space,
  Spaces,
  SphereGeometry,
  Splat,
  StaticImageTargetOrientation,
  TetrahedronGeometry,
  TextureFiltering,
  TextureWrap,
  TorusGeometry,
  UiGraphSettings,
  UiRootType,
  UnlitMaterial,
  Url,
  Vec2Tuple,
  Vec3Tuple,
  Vec4Tuple,
  Vec6Tuple,
  VideoControlsGraphSettings,
  VideoMaterial,
  XrCameraType,
  XrConfig,
  XrFaceConfig,
  XrWorldConfig,
}

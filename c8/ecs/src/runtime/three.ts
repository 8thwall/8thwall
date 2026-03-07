// @visibility(//visibility:public)

// eslint-disable-next-line no-restricted-imports
import type {
  Mesh, MeshStandardMaterial, BufferGeometry, MeshBasicMaterial, SphereGeometry, BoxGeometry,
  PlaneGeometry, Vector3, Matrix4, Quaternion, Scene, AnimationMixer, AnimationClip,
  LoopOnce, LoopRepeat, LoopPingPong, Audio, AudioListener, PositionalAudio, Raycaster, Vector2,
  DirectionalLight, MeshPhysicalMaterial, TextureLoader, CapsuleGeometry, ConeGeometry,
  CylinderGeometry, TetrahedronGeometry, CircleGeometry, AmbientLight, Euler, PerspectiveCamera,
  OrthographicCamera, Sprite, SpriteMaterial, PointLight, WebGLRenderer, WebGLRenderTarget,
  Color, Blending, Wrapping, DepthTexture, RGBAFormat, HalfFloatType, UnsignedInt248Type,
  BufferAttribute, CanvasTexture, ShaderMaterial, ShadowMaterial, RingGeometry, PolyhedronGeometry,
  ColorSpace, TorusGeometry, SpotLight, Vector4, DepthFormat, SkinnedMesh, DataTexture,
  VideoTexture, Group, Texture, RectAreaLight, Fog, FogExp2, MinificationTextureFilter,
  MagnificationTextureFilter, LinearFilter, NearestFilter,
  LinearMipMapLinearFilter,
  NearestMipMapLinearFilter,
} from 'three'

import type {GLTFLoader, DRACOLoader, RGBELoader, Object3D, skeletonClone} from './three-types'

type Three = {
  Mesh: typeof Mesh
  MeshStandardMaterial: typeof MeshStandardMaterial
  MeshPhysicalMaterial: typeof MeshPhysicalMaterial
  ShadowMaterial: typeof ShadowMaterial
  BufferAttribute: typeof BufferAttribute
  BufferGeometry: typeof BufferGeometry
  MeshBasicMaterial: typeof MeshBasicMaterial
  DoubleSide: any
  BackSide: any
  FrontSide: any
  SphereGeometry: typeof SphereGeometry
  BoxGeometry: typeof BoxGeometry
  PlaneGeometry: typeof PlaneGeometry
  CapsuleGeometry: typeof CapsuleGeometry
  ConeGeometry: typeof ConeGeometry
  CylinderGeometry: typeof CylinderGeometry
  TetrahedronGeometry: typeof TetrahedronGeometry
  PolyhedronGeometry: typeof PolyhedronGeometry
  CircleGeometry: typeof CircleGeometry
  RingGeometry: typeof RingGeometry
  TorusGeometry: typeof TorusGeometry
  Object3D: Object3D
  Vector3: typeof Vector3
  Vector4: typeof Vector4
  Matrix4: typeof Matrix4
  Quaternion: typeof Quaternion
  Euler: typeof Euler
  Scene: typeof Scene
  Group: typeof Group
  GLTFLoader: typeof GLTFLoader
  RGBELoader: typeof RGBELoader
  DRACOLoader: typeof DRACOLoader
  AnimationMixer: typeof AnimationMixer
  AnimationClip: typeof AnimationClip
  LoopOnce: typeof LoopOnce
  LoopRepeat: typeof LoopRepeat
  LoopPingPong: typeof LoopPingPong
  Audio: typeof Audio
  AudioListener: typeof AudioListener
  PositionalAudio: typeof PositionalAudio
  Raycaster: typeof Raycaster
  Vector2: typeof Vector2
  DirectionalLight: typeof DirectionalLight
  TextureLoader: typeof TextureLoader
  AmbientLight: typeof AmbientLight
  SpotLight: typeof SpotLight
  RectAreaLight: typeof RectAreaLight
  PerspectiveCamera: typeof PerspectiveCamera
  OrthographicCamera: typeof OrthographicCamera
  Sprite: typeof Sprite
  SpriteMaterial: typeof SpriteMaterial
  PointLight: typeof PointLight
  WebGLRenderer: typeof WebGLRenderer
  WebGLRenderTarget: typeof WebGLRenderTarget
  DepthTexture: typeof DepthTexture
  Color: typeof Color
  Blending: Blending
  NoBlending: Blending
  NormalBlending: Blending
  AdditiveBlending: Blending
  MultiplyBlending: Blending
  SubtractiveBlending: Blending
  RepeatWrapping: Wrapping
  ClampToEdgeWrapping: Wrapping
  MirroredRepeatWrapping: Wrapping
  CanvasTexture: typeof CanvasTexture
  DataTexture: typeof DataTexture
  ShaderMaterial: typeof ShaderMaterial
  SkinnedMesh: typeof SkinnedMesh
  SRGBColorSpace: ColorSpace
  BufferGeometryUtils: {
    mergeVertices: (geometry: BufferGeometry, tolerance?: number) => BufferGeometry
  }
  RGBAFormat: typeof RGBAFormat
  HalfFloatType: typeof HalfFloatType
  UnsignedInt248Type: typeof UnsignedInt248Type
  DepthFormat: typeof DepthFormat
  EquirectangularReflectionMapping: any
  VideoTexture: typeof VideoTexture
  skeletonClone: typeof skeletonClone
  Texture: typeof Texture
  Fog: typeof Fog
  FogExp2: typeof FogExp2
  MinificationTextureFilter: MinificationTextureFilter
  MagnificationTextureFilter: MagnificationTextureFilter
  LinearFilter: typeof LinearFilter
  NearestFilter: typeof NearestFilter
  LinearMipmapLinearFilter: typeof LinearMipMapLinearFilter
  NearestMipmapLinearFilter: typeof NearestMipMapLinearFilter
}

const {THREE} = window as any as {THREE: Three}

export default THREE

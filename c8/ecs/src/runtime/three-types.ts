// eslint-disable-next-line import/extensions
import type {GLTF, GLTFLoader} from 'three/examples/jsm/loaders/GLTFLoader.js'
// eslint-disable-next-line import/extensions
import type {DRACOLoader} from 'three/examples/jsm/loaders/DRACOLoader.js'
// eslint-disable-next-line import/extensions
import type {RGBELoader} from 'three/examples/jsm/loaders/RGBELoader.js'

// eslint-disable-next-line import/extensions
import type {clone} from 'three/examples/jsm/utils/SkeletonUtils.js'

// @attr[](srcs = "object-3d-extended.ts")
// @attr[](srcs = "user-data.ts")
// @attr[](srcs = "lights-types.ts")
// @attr[](srcs = "camera-manager-types.ts")
// @dep(//c8/ecs/src/shared:schema)

// @inliner-skip-next
import type {Object3D, RelaxedObject3D} from './object-3d-extended'

// eslint-disable-next-line no-restricted-imports
export type {
  Vector3, Vector4, Matrix4, Quaternion, Scene, AnimationMixer, AnimationClip,
  Mesh, AmbientLight, DirectionalLight, MeshStandardMaterial,
  MeshPhysicalMaterial, Intersection, PositionalAudio, Audio, Camera,
  WebGLRenderer, PerspectiveCamera, OrthographicCamera, AudioListener,
  Euler, PointLight, Group, ShadowMaterial, Color,
  ShaderMaterial, MeshBasicMaterial, Texture, ColorSpace, Event, Material,
  AnimationAction, SpotLight, BufferGeometry, WebGLRenderTarget, Raycaster,
  Object3DEventMap, TextureLoader, VideoTexture, RectAreaLight, WebGLRenderList,
  MinificationTextureFilter, MagnificationTextureFilter,
} from 'three'

export type {
  DRACOLoader,
  GLTFLoader,
  RGBELoader,
  GLTF,
  Object3D,
  RelaxedObject3D,
  clone as skeletonClone,
}

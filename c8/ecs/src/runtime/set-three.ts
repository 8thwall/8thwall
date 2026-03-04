// eslint-disable-next-line no-restricted-imports
import * as THREE from 'three'
// @ts-ignore
import {GLTFLoader} from 'three/examples/jsm/loaders/GLTFLoader'
// @ts-ignore
import {DRACOLoader} from 'three/examples/jsm/loaders/DRACOLoader'
// @ts-ignore
import * as BufferGeometryUtils from 'three/examples/jsm/utils/BufferGeometryUtils'
// @ts-ignore
import {RGBELoader} from 'three/examples/jsm/loaders/RGBELoader'

import {
  acceleratedRaycast, computeBoundsTree, disposeBoundsTree,
} from 'three-mesh-bvh'

// eslint-disable-next-line import/extensions
import {clone as skeletonClone} from 'three/examples/jsm/utils/SkeletonUtils.js'

THREE.BufferGeometry.prototype.computeBoundsTree = computeBoundsTree
THREE.BufferGeometry.prototype.disposeBoundsTree = disposeBoundsTree
THREE.Mesh.prototype.raycast = acceleratedRaycast

Object.assign(THREE, {GLTFLoader, DRACOLoader, RGBELoader, BufferGeometryUtils, skeletonClone})

// NOTE(christoph): Set three on the window so the runtime can access it.
Object.assign(window, {THREE})

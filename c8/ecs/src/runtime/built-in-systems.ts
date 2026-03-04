import {
  BoxGeometry, PlaneGeometry, SphereGeometry, CapsuleGeometry, ConeGeometry,
  CylinderGeometry, TetrahedronGeometry, CircleGeometry,
  RingGeometry,
  PolyhedronGeometry,
  TorusGeometry,
} from './components'
import type {World} from './world'
import THREE from './three'
import {
  DEFAULT_CAP_SEGMENTS, DEFAULT_RADIAL_SEGMENTS, getPolyhedronGeometry,
} from '../shared/geometry-constants'

import {makeSplatSystem} from './systems/splat-system'
import {makeGltfSystem} from './systems/gltf-system'
import {makeGeometrySystem} from './systems/geometry-system'
import {makeTransformSystem} from './systems/transform-system'
import {makeUiSystem} from './systems/ui-system'
import {makeHiddenSystem} from './systems/hidden-system'
import {makeCameraSystem} from './systems/camera-system'
import {makeMaterialSystem} from './systems/material-system'
import {makeHiderMaterialSystem} from './systems/hider-material-system'
import {makeShadowMaterialSystem} from './systems/shadow-material-system'
import {makeUnlitMaterialSystem} from './systems/unlit-material-system'
import {makeVideoMaterialSystem} from './systems/video-material-system'
import {makeLightSystem, makeShadowSystem} from './systems/light-systems'
import {makeAudioSystem} from './systems/audio-system'
import {makeImageTargetSystem} from './systems/image-target-system'
import {makeFaceSystem} from './systems/face-system'
import type {Faces} from '../shared/scene-graph'
import {makeVideoControlsSystem} from './systems/video-system'

// System responsible for keeping the scene in sync with the ECS state
const makeSceneSyncSystem = (world: World, registerPostRender: (cb: () => void) => void) => {
  const spheresSystem = makeGeometrySystem(
    world,
    SphereGeometry,
    'SphereGeometry',
    c => new THREE.SphereGeometry(c.radius)
  )

  const boxesSystem = makeGeometrySystem(
    world,
    BoxGeometry,
    'BoxGeometry',
    c => new THREE.BoxGeometry(c.width, c.height, c.depth)
  )

  const planesSystem = makeGeometrySystem(
    world,
    PlaneGeometry,
    'PlaneGeometry',
    c => new THREE.PlaneGeometry(c.width, c.height)
  )

  const capsuleSystem = makeGeometrySystem(
    world,
    CapsuleGeometry,
    'CapsuleGeometry',
    c => new THREE.CapsuleGeometry(
      c.radius, c.height, DEFAULT_CAP_SEGMENTS, DEFAULT_RADIAL_SEGMENTS
    )
  )

  const coneSystem = makeGeometrySystem(
    world,
    ConeGeometry,
    'ConeGeometry',
    c => new THREE.ConeGeometry(c.radius, c.height, DEFAULT_RADIAL_SEGMENTS)
  )

  const cylinderSystem = makeGeometrySystem(
    world,
    CylinderGeometry,
    'CylinderGeometry',
    c => new THREE.CylinderGeometry(c.radius, c.radius, c.height, DEFAULT_RADIAL_SEGMENTS)
  )

  const tetrahedronSystem = makeGeometrySystem(
    world,
    TetrahedronGeometry,
    'TetrahedronGeometry',
    c => new THREE.TetrahedronGeometry(c.radius)
  )

  const polyhedronSystem = makeGeometrySystem(
    world,
    PolyhedronGeometry,
    'PolyhedronGeometry',
    (c) => {
      const polyhedron = getPolyhedronGeometry(c.faces as Faces)
      return new THREE.PolyhedronGeometry(polyhedron?.vertices, polyhedron?.indices, c.radius)
    }
  )

  const circleSystem = makeGeometrySystem(
    world,
    CircleGeometry,
    'CircleGeometry',
    c => new THREE.CircleGeometry(c.radius, DEFAULT_RADIAL_SEGMENTS)
  )

  const ringSystem = makeGeometrySystem(
    world,
    RingGeometry,
    'RingGeometry',
    c => new THREE.RingGeometry(c.innerRadius, c.outerRadius, DEFAULT_RADIAL_SEGMENTS)
  )

  const torusSystem = makeGeometrySystem(
    world,
    TorusGeometry,
    'TorusGeometry',
    c => new THREE.TorusGeometry(
      c.radius, c.tubeRadius, DEFAULT_RADIAL_SEGMENTS, DEFAULT_RADIAL_SEGMENTS
    )
  )
  const faceSystem = makeFaceSystem(world)

  const materialSystem = makeMaterialSystem(world)

  const unlitMaterialSystem = makeUnlitMaterialSystem(world)

  const shadowMaterialSystem = makeShadowMaterialSystem(world)

  const hiderMaterialSystem = makeHiderMaterialSystem(world)

  const videoMaterialSystem = makeVideoMaterialSystem(world)

  const transformSystem = makeTransformSystem(world)

  const gltfSystem = makeGltfSystem(world)

  const splatSystem = makeSplatSystem(world)

  const hiddenSystem = makeHiddenSystem(world)

  const shadowSystem = makeShadowSystem(world)

  const audioSystem = makeAudioSystem(world)

  const uiSystem = makeUiSystem(world, registerPostRender)

  const lightSystem = makeLightSystem(world)

  const cameraSystem = makeCameraSystem(world)

  const imageTargetSystem = makeImageTargetSystem(world)

  const videoControlsSystem = makeVideoControlsSystem(world)

  return () => {
    faceSystem()
    imageTargetSystem()
    transformSystem()
    spheresSystem()
    boxesSystem()
    planesSystem()
    capsuleSystem()
    coneSystem()
    cylinderSystem()
    tetrahedronSystem()
    polyhedronSystem()
    circleSystem()
    ringSystem()
    torusSystem()
    gltfSystem()
    splatSystem()
    materialSystem()
    unlitMaterialSystem()
    shadowMaterialSystem()
    hiderMaterialSystem()
    videoMaterialSystem()
    hiddenSystem()
    shadowSystem()
    audioSystem()
    uiSystem()
    cameraSystem()
    lightSystem()
    videoControlsSystem()
  }
}

export {
  makeSceneSyncSystem,
}
